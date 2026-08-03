# Замер времени запуска gid8: от старта процесса до момента, когда
# программа перестаёт писать в журнал.
#
# Мерить «на глаз» здесь нельзя: часть пауз в журнале — ожидание
# диалога, а часть — настоящая работа. Скрипт берёт объективное:
# сколько прошло от запуска до последней записи в логе и сколько
# строк при этом написано. Прогоны повторяются, берётся медиана —
# первый запуск всегда медленнее из-за холодного кэша файлов.
#
#   powershell -File tools/measure_startup.ps1 -Runs 3
param(
    [string]$Exe  = 'H:\build\gid8-audit\gid8.exe',
    [string]$Log  = "$env:APPDATA\Sirius\Tgid-07\tgid_qt.log",
    [int]$Runs    = 3,
    [int]$WaitSec = 40
)

$results = @()

for ($i = 1; $i -le $Runs; $i++) {
    Stop-Process -Name gid8 -Force -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 2

    $t0 = Get-Date
    Start-Process -FilePath $Exe
    Start-Sleep -Seconds $WaitSec
    Stop-Process -Name gid8 -Force -ErrorAction SilentlyContinue

    if (Test-Path $Log) {
        # Журнал открывается программой заново при каждом запуске
        # (Truncate), поэтому его размер и время последней записи
        # относятся именно к этому прогону.
        $lines = (Get-Content $Log -Encoding UTF8 | Measure-Object -Line).Lines
        $last  = (Get-Item $Log).LastWriteTime
        $sec   = [math]::Round(($last - $t0).TotalSeconds, 2)
        $results += [pscustomobject]@{ Progon = $i; Sekund = $sec; Strok = $lines }
        "прогон {0}: {1} с, строк в журнале {2}" -f $i, $sec, $lines
    }
}

if ($results.Count) {
    $med = ($results.Sekund | Sort-Object)[[int]($results.Count / 2)]
    $str = ($results.Strok  | Sort-Object)[[int]($results.Count / 2)]
    ""
    "медиана: {0} с, строк {1}" -f $med, $str
}
