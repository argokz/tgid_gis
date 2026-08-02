# Пересчёт всех фрагментов, у которых есть узлы.
#
# Нужен после восстановления структуры таблиц результатов: сами
# результаты не переносились, потому что воспроизводимы прогоном.
# Карточки свойств, пьезометрика и выгрузки показывают именно расчётные
# величины, поэтому без прогона они пустые.
#
#   .\tools\recalc_all.ps1
#   .\tools\recalc_all.ps1 -Fragments 2,74

param(
    [string]$Db       = 'tgid_gis',
    [string]$Server   = 'localhost',
    [int]   $Port     = 5440,
    [string]$User     = 'postgres',
    [string]$Password = $env:PGPASSWORD,
    [string]$Tn       = '-25',
    [int[]] $Fragments,
    [string]$Venv     = 'H:\venv\sety\Scripts\python.exe',
    [string]$Ww       = 'H:\projects\tgid-app-new\gid8\python\sety\sety\ww.py'
)

# Движок пишет ход расчёта в поток ошибок. При 'Stop' PowerShell
# принимает это за сбой и прерывает скрипт на первом же фрагменте.
$ErrorActionPreference = 'Continue'
$env:PGCLIENTENCODING = 'UTF8'
if (-not $Password) { throw 'не задан пароль: -Password или $env:PGPASSWORD' }
$env:PGPASSWORD = $Password

if (-not $Fragments) {
    $q = @'
select f.id from net.fragment f
where exists (select 1 from net.node_reg n
              where n.fragment_id = f.id and not n.removed)
order by f.id
'@
    $Fragments = & psql -h $Server -p $Port -U $User -d $Db -tA -c $q |
                 Where-Object { $_ -match '^\d+$' } | ForEach-Object { [int]$_ }
}

Write-Host ("фрагментов к расчёту: {0}" -f $Fragments.Count)
$ok = 0; $bad = @()

foreach ($f in $Fragments) {
    $t0 = Get-Date
    # Номер расчёта до и после: если он не изменился, прогон не
    # состоялся, сколько бы времени ни прошло.
    $before = & psql -h $Server -p $Port -U $User -d $Db -tA -c `
        "select coalesce(max(id),0) from calc.calculation where fileid=$f"

    & $Venv $Ww -type_of_net 1 -server $Server -database $Db -user $User `
        -port $Port -fileID $f -Tn $Tn -GWS 1 -GWS2 1 -sopr 0 `
        -roP 0.975 -roO 0.975 -ro_temp -rdbms postgreSQL `
        -out_file (Join-Path $env:TEMP "recalc_$f.txt") `
        -user_gid recalc -password $Password *> `
        (Join-Path $env:TEMP "recalc_$f.log")

    $after = & psql -h $Server -p $Port -U $User -d $Db -tA -c `
        "select coalesce(max(id),0) from calc.calculation where fileid=$f"
    $sec = [int]((Get-Date) - $t0).TotalSeconds

    if ($before -eq $after) {
        $bad += $f
        Write-Host ("  фрагмент {0,-4} НЕ РАССЧИТАН ({1} с)" -f $f, $sec)
    } else {
        $ok++
        Write-Host ("  фрагмент {0,-4} готов, расчёт {1} ({2} с)" -f $f, $after, $sec)
    }
}

Write-Host ""
Write-Host ("рассчитано: {0}, не удалось: {1}" -f $ok, $bad.Count)
if ($bad.Count) { Write-Host ("не удались: " + ($bad -join ', ')) }
