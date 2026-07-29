# Запуск расчётного ядра sety — теми же аргументами, что формирует
# приложение (gidview/gidr_calc.cpp, функция getDoIt).
#
# Ядро работает через pyodbc, поэтому нужны ODBC-драйверы PostgreSQL
# и отдельное окружение: требования sety конфликтуют с глобальным Python
# (networkx 3.4.1 против 2.8.8).
#
# Примеры:
#   # один прогон
#   .\tools\run_calc.ps1 -Db tgid_gis -Fragment 2
#
#   # прогон на обеих БД + сверка *_OUT
#   .\tools\run_calc.ps1 -Compare -Fragment 2 -Report docs\schema\calc_regression_report.json
#
# Пароль: $env:PGPASSWORD или ключ -Password. Файл .env подхватывается,
# если лежит в корне репозитория.

param(
    [string]$Db        = 'tgid_gis',
    [string]$DbHost    = '',
    [int]$Port         = 0,
    [string]$User      = '',
    [string]$Password  = '',
    [int]$Fragment     = 80,
    [double]$Tn        = -25,
    [string]$Venv      = 'H:\venv\sety\Scripts\python.exe',
    [string]$Sety      = 'H:\projects\tgid-app-new\gid8\python\sety\sety\ww.py',
    [string]$OutFile   = '',
    [switch]$Compare,
    [string]$DbA       = 'almatygid',
    [string]$DbB       = 'tgid_gis',
    [string]$Report    = '',
    [double]$Atol      = 1e-6,
    [double]$Rtol      = 1e-6
)

$ErrorActionPreference = 'Continue'
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

# Подхват .env (не перезаписывает уже заданные переменные).
$envFile = Join-Path $root '.env'
if (Test-Path $envFile) {
    Get-Content $envFile | ForEach-Object {
        $line = $_.Trim()
        if (-not $line -or $line.StartsWith('#')) { return }
        $i = $line.IndexOf('=')
        if ($i -lt 1) { return }
        $k = $line.Substring(0, $i).Trim()
        $v = $line.Substring($i + 1).Trim().Trim('"').Trim("'")
        if (-not [string]::IsNullOrEmpty($k) -and -not (Test-Path "Env:$k")) {
            Set-Item -Path "Env:$k" -Value $v
        }
    }
}

if (-not $DbHost) { $DbHost = if ($env:TGID_HOST) { $env:TGID_HOST } else { 'localhost' } }
if ($Port -le 0) { $Port = if ($env:TGID_PORT) { [int]$env:TGID_PORT } else { 5440 } }
if (-not $User) { $User = if ($env:TGID_USER) { $env:TGID_USER } else { 'postgres' } }
if (-not $Password) { $Password = if ($env:PGPASSWORD) { $env:PGPASSWORD } else { '' } }

if (-not (Test-Path $Venv)) {
    Write-Host "ПРОВАЛ: нет интерпретатора sety: $Venv"
    Write-Host "Создайте venv с зависимостями sety или укажите -Venv."
    exit 2
}
if (-not (Test-Path $Sety)) {
    Write-Host "ПРОВАЛ: нет ww.py: $Sety"
    Write-Host "Ожидается gid8/python/sety/sety/ww.py."
    exit 2
}

function Invoke-Sety([string]$database) {
    # В режиме -Compare у каждой БД свой out_file — иначе второй прогон затрёт первый.
    $stamp = Get-Date -Format 'yyyyMMdd_HHmmss'
    $out = if ($OutFile -and -not $Compare) {
        $OutFile
    } else {
        Join-Path $env:TEMP ("calc_{0}_f{1}_{2}.txt" -f $database, $Fragment, $stamp)
    }
    $logDir = Join-Path $root 'docs\schema'
    if (-not (Test-Path $logDir)) { New-Item -ItemType Directory -Path $logDir | Out-Null }
    $log = Join-Path $logDir ("calc_log_{0}_f{1}_{2}.txt" -f $database, $Fragment, $stamp)
    $stdout = Join-Path $env:TEMP ("sety_stdout_{0}_{1}.txt" -f $database, $stamp)
    $stderr = Join-Path $env:TEMP ("sety_stderr_{0}_{1}.txt" -f $database, $stamp)

    $setyArgs = @(
        $Sety,
        '-type_of_net', '1',
        '-server',   "$DbHost",
        '-database', "$database",
        '-user',     "$User",
        '-port',     "$Port",
        '-fileID',   "$Fragment",
        '-Tn',       "$Tn",
        '-GWS',  '1', '-GWS2', '1',
        '-sopr', '0', '-roP', '0.975', '-roO', '0.975', '-ro_temp',
        '-rdbms', 'postgreSQL',
        '-out_file', "$out",
        '-user_gid', 'migration-check'
    )
    if ($Password) { $setyArgs += @('-password', "$Password") }

    Write-Host "расчёт: БД $database, фрагмент $Fragment, Tn $Tn"
    Write-Host "вывод:  $out"
    Write-Host "лог:    $log"

    $safe = @()
    for ($i = 0; $i -lt $setyArgs.Count; $i++) {
        if ($setyArgs[$i] -eq '-password' -and ($i + 1) -lt $setyArgs.Count) {
            $safe += @('-password', '***')
            $i++
            continue
        }
        $safe += $setyArgs[$i]
    }
    $header = @(
        "start $(Get-Date -Format o)",
        "venv: $Venv",
        "args: $($safe -join ' ')",
        '---'
    ) -join "`n"
    [System.IO.File]::WriteAllText($log, $header + "`n", [System.Text.UTF8Encoding]::new($false))

    # Start-Process на PS 5.1 ломает аргументы с пробелами, если передать массив.
    # Собираем одну строку с кавычками.
    $argLine = ($setyArgs | ForEach-Object {
        $a = "$_"
        if ($a -match '[\s"]') { '"' + ($a -replace '"', '\"') + '"' } else { $a }
    }) -join ' '

    $p = Start-Process -FilePath $Venv -ArgumentList $argLine -NoNewWindow -Wait -PassThru `
        -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    foreach ($f in @($stdout, $stderr)) {
        if (Test-Path $f) {
            [System.IO.File]::AppendAllText($log, [System.IO.File]::ReadAllText($f) + "`n",
                [System.Text.UTF8Encoding]::new($false))
        }
    }
    $code = 0
    if ($null -ne $p.ExitCode) { $code = [int]$p.ExitCode }
    [System.IO.File]::AppendAllText($log, "exit=$code end=$(Get-Date -Format o)`n",
        [System.Text.UTF8Encoding]::new($false))
    Write-Host "код возврата: $code"
    return $code
}

if ($Compare) {
    $ca = Invoke-Sety $DbA
    if ($ca -ne 0) { Write-Host "ПРОВАЛ: расчёт на $DbA"; exit $ca }
    $cb = Invoke-Sety $DbB
    if ($cb -ne 0) { Write-Host "ПРОВАЛ: расчёт на $DbB"; exit $cb }

    $cmp = @(
        'tools/compare_calc.py',
        '--db-a', $DbA, '--db-b', $DbB,
        '--fragment', $Fragment,
        '--atol', $Atol, '--rtol', $Rtol
    )
    if ($Report) { $cmp += @('--report', $Report) }
    Write-Host "сверка: $DbA vs $DbB, фрагмент $Fragment"
    & python @cmp
    exit $LASTEXITCODE
}

exit (Invoke-Sety $Db)
