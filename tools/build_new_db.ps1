# Создание новой БД на целевой схеме.
#
# Порядок важен: сначала PostGIS, потом только схема public из источника,
# затем функции и схема net, затем конвертер. Схема net НЕ переносится
# дампом — она строится заново, иначе восстановление вычисляемых колонок
# зависит от порядка создания объектов.
#
# Запуск:
#   $env:PGPASSWORD='...'; .\tools\build_new_db.ps1 -Source almatygid -Target tgid_gis

param(
    [string]$DbHost = 'localhost',
    [int]$Port = 5440,
    [string]$User = 'postgres',
    [string]$Source = 'almatygid',
    [string]$Target = 'tgid_gis',
    [string]$Dump = ''
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
if (-not $Dump) { $Dump = Join-Path $env:TEMP "$Source.dump" }

# Имена с глаголом обязательны: PowerShell не различает регистр, и функция
# с именем Psql перехватывала бы вызовы psql.exe, уходя в бесконечную рекурсию.
function Invoke-PsqlCmd([string]$db, [string]$sql) {
    & psql.exe -h $DbHost -p $Port -U $User -d $db -v ON_ERROR_STOP=1 -At -c $sql
}
function Invoke-PsqlFile([string]$db, [string]$file) {
    & psql.exe -h $DbHost -p $Port -U $User -d $db -v ON_ERROR_STOP=1 -f $file
}

Write-Host "1. Дамп исходной БД $Source"
if (-not (Test-Path $Dump)) {
    & pg_dump.exe -h $DbHost -p $Port -U $User -d $Source -Fc -f $Dump
}
Write-Host ("   {0} МБ" -f [math]::Round((Get-Item $Dump).Length / 1MB, 1))

Write-Host "2. Создание пустой БД $Target"
Invoke-PsqlCmd 'postgres' "DROP DATABASE IF EXISTS $Target"
Invoke-PsqlCmd 'postgres' "CREATE DATABASE $Target TEMPLATE template0 ENCODING 'UTF8'"
Invoke-PsqlCmd $Target 'CREATE EXTENSION IF NOT EXISTS postgis'

Write-Host "3. Восстановление только схемы public"
& pg_restore.exe -h $DbHost -p $Port -U $User -d $Target `
    --no-owner --no-privileges --schema=public --exclude-schema=net -j 4 $Dump 2>&1 |
    Select-String -Pattern 'error:' | Select-Object -First 5

Write-Host "4. Функции и целевая схема net"
Invoke-PsqlFile $Target (Join-Path $root 'sql\005_net_functions.sql') | Out-Null
Invoke-PsqlFile $Target (Join-Path $root 'sql\010_net_schema.sql')    | Out-Null

Write-Host "5. Конвертация"
$env:TGID_DB = $Target
$env:TGID_HOST = $DbHost
$env:TGID_PORT = $Port
$env:TGID_USER = $User
& python (Join-Path $root 'converter\convert.py') `
    --mapping (Join-Path $root 'converter\mapping.json') --apply

Write-Host "6. Слой совместимости"
Invoke-PsqlFile $Target (Join-Path $root 'sql\030_compat_views.sql') | Out-Null

Write-Host "7. Проверки"
& python (Join-Path $root 'converter\validate.py') `
    --mapping (Join-Path $root 'converter\mapping.json') `
    --report (Join-Path $root 'docs\schema\validation_report.json')

Write-Host "Готово: БД $Target"
