# Сборка продуктовой GIS-БД без legacy/compat-контракта приложения.
#
# Сборка двухступенчатая:
#   1. Во временной БД восстанавливается public и выполняется конвертация.
#   2. В продуктовую БД переносятся только net + ref + meta.
#
# Это позволяет получить действительно чистый продукт, не удаляя объекты
# PostGIS из public и не оставляя 600+ исходных таблиц рядом с net.
#
# Пример:
#   $env:PGPASSWORD='...'
#   .\tools\build_clean_gis_db.ps1 -Source almatygid -Target tgid_prod
#
# Существующие Target/BuildDatabase не удаляются без -ForceRecreate.

param(
    [ValidatePattern('^[A-Za-z_][A-Za-z0-9_]*$')]
    [string]$Source = 'almatygid',

    [ValidatePattern('^[A-Za-z_][A-Za-z0-9_]*$')]
    [string]$Target = 'tgid_prod',

    [ValidateScript({
        $_ -eq '' -or $_ -match '^[A-Za-z_][A-Za-z0-9_]*$'
    })]
    [string]$BuildDatabase = '',

    [string]$DbHost = 'localhost',
    [int]$Port = 5440,
    [string]$User = 'postgres',
    [string]$SourceDump = '',
    [string]$ProductDump = '',
    [switch]$ReuseSourceDump,
    [switch]$ForceRecreate,
    [switch]$KeepBuildDatabase
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

if (-not $BuildDatabase) { $BuildDatabase = "${Target}_build" }
if ($Source -eq $Target -or $Source -eq $BuildDatabase -or
    $Target -eq $BuildDatabase) {
    throw 'Source, Target и BuildDatabase должны быть разными БД'
}

if (-not $SourceDump) {
    $SourceDump = Join-Path $env:TEMP "$Source.dump"
}
if (-not $ProductDump) {
    $ProductDump = Join-Path $env:TEMP "$Target.product.dump"
}

function Invoke-PsqlCmd([string]$Database, [string]$Sql) {
    & psql.exe -h $DbHost -p $Port -U $User -d $Database `
        -v ON_ERROR_STOP=1 -At -c $Sql
    if ($LASTEXITCODE -ne 0) {
        throw "psql завершился с кодом $LASTEXITCODE (БД $Database)"
    }
}

function Invoke-PsqlFile([string]$Database, [string]$File) {
    & psql.exe -h $DbHost -p $Port -U $User -d $Database `
        -v ON_ERROR_STOP=1 -f $File
    if ($LASTEXITCODE -ne 0) {
        throw "Ошибка применения $File к БД $Database"
    }
}

function Test-DatabaseExists([string]$Database) {
    $value = & psql.exe -h $DbHost -p $Port -U $User -d postgres -At `
        -c "SELECT 1 FROM pg_database WHERE datname = '$Database'"
    if ($LASTEXITCODE -ne 0) {
        throw 'Не удалось получить список БД'
    }
    return $value -eq '1'
}

function Remove-Database([string]$Database) {
    Invoke-PsqlCmd 'postgres' @"
SELECT pg_terminate_backend(pid)
FROM pg_stat_activity
WHERE datname = '$Database' AND pid <> pg_backend_pid();
"@
    Invoke-PsqlCmd 'postgres' "DROP DATABASE IF EXISTS `"$Database`""
}

function New-PostgisDatabase([string]$Database) {
    Invoke-PsqlCmd 'postgres' "CREATE DATABASE `"$Database`" TEMPLATE template0 ENCODING 'UTF8'"
    Invoke-PsqlCmd $Database 'CREATE EXTENSION IF NOT EXISTS postgis'
}

function Assert-CanCreateDatabase([string]$Database) {
    if (-not (Test-DatabaseExists $Database)) { return }
    if (-not $ForceRecreate) {
        throw "БД $Database уже существует. Для пересоздания укажите -ForceRecreate."
    }
    Remove-Database $Database
}

Write-Host "1. Дамп исходной БД $Source"
if ((Test-DatabaseExists $Target) -and -not $ForceRecreate) {
    throw "БД $Target уже существует. Для пересоздания укажите -ForceRecreate."
}
if (-not $ReuseSourceDump -or -not (Test-Path -LiteralPath $SourceDump)) {
    & pg_dump.exe -h $DbHost -p $Port -U $User -d $Source `
        -Fc -f $SourceDump
    if ($LASTEXITCODE -ne 0) {
        throw "Не удалось создать дамп $SourceDump"
    }
}
Write-Host ("   {0} МБ: {1}" -f
    [math]::Round((Get-Item -LiteralPath $SourceDump).Length / 1MB, 1),
    $SourceDump)

Write-Host "2. Временная БД $BuildDatabase"
Assert-CanCreateDatabase $BuildDatabase
New-PostgisDatabase $BuildDatabase

Write-Host '3. Восстановление исходной схемы public'
$restoreOutput = & pg_restore.exe -h $DbHost -p $Port -U $User `
    -d $BuildDatabase --no-owner --no-privileges --schema=public `
    --exclude-schema=net -j 4 $SourceDump 2>&1
$restoreExitCode = $LASTEXITCODE
if ($restoreExitCode -ne 0) {
    $restoreOutput | Select-Object -Last 30
    throw "pg_restore завершился с кодом $restoreExitCode"
}

Write-Host '4. Полная целевая схема net'
foreach ($name in @(
    '005_net_functions.sql',
    '010_net_schema.sql',
    '011_net_registry_funcs.sql',
    '012_net_review.sql',
    '013_subtype_src_id.sql',
    '014_extra_tables.sql'
)) {
    Invoke-PsqlFile $BuildDatabase (Join-Path $root "sql\$name")
}

Write-Host '5. Конвертация public -> net'
$env:TGID_DB = $BuildDatabase
$env:TGID_HOST = $DbHost
$env:TGID_PORT = "$Port"
$env:TGID_USER = $User
& python (Join-Path $root 'converter\convert.py') `
    --mapping (Join-Path $root 'converter\mapping.json') --apply
if ($LASTEXITCODE -ne 0) {
    throw 'Конвертация завершилась с ошибкой'
}

Write-Host '6. Постобработка net'
foreach ($name in @(
    '015_stable_order.sql',
    '016_node_attr.sql',
    '020_fix_geometry.sql',
    '070_map_thin.sql',
    '080_concurrency.sql',
    '090_object_history.sql',
    '095_object_insert_history.sql',
    '096_line_topology.sql',
    '097_node_topology.sql'
)) {
    Invoke-PsqlFile $BuildDatabase (Join-Path $root "sql\$name")
}

Write-Host '7. Строгая сверка с исходными таблицами'
& python (Join-Path $root 'converter\validate.py') `
    --mapping (Join-Path $root 'converter\mapping.json') `
    --db $BuildDatabase `
    --report (Join-Path $root 'docs\schema\validation_report_prod.json')
if ($LASTEXITCODE -ne 0) {
    throw 'Строгая сверка переноса завершилась с ошибкой'
}

Write-Host '8. Справочники ref'
Invoke-PsqlFile $BuildDatabase (Join-Path $root 'sql\098_core_references.sql')

Write-Host '9. Каталог GIS'
Invoke-PsqlFile $BuildDatabase (Join-Path $root 'sql\100_gis_catalog.sql')
Invoke-PsqlFile $BuildDatabase (Join-Path $root 'sql\105_ui_field_catalog.sql')
Invoke-PsqlFile $BuildDatabase (Join-Path $root 'sql\106_complete_field_catalog.sql')

Write-Host '10. Строгий GIS-аудит'
& python (Join-Path $root 'tools\check_gis_ready.py') --db $BuildDatabase
if ($LASTEXITCODE -ne 0) {
    throw 'GIS-аудит не пройден: продуктовая БД не будет создана'
}

Write-Host '   Сохранение пользовательской системы координат SRID 9998'
Invoke-PsqlCmd $BuildDatabase @'
CREATE TABLE IF NOT EXISTS meta.spatial_ref_system (
    srid integer PRIMARY KEY,
    auth_name varchar(256),
    auth_srid integer,
    srtext varchar(2048),
    proj4text varchar(2048)
);
TRUNCATE TABLE meta.spatial_ref_system;
INSERT INTO meta.spatial_ref_system
    (srid, auth_name, auth_srid, srtext, proj4text)
SELECT srid, auth_name, auth_srid, srtext, proj4text
FROM public.spatial_ref_sys
WHERE srid = 9998;
DO $body$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM meta.spatial_ref_system WHERE srid = 9998
    ) THEN
        RAISE EXCEPTION 'SRID 9998 отсутствует в public.spatial_ref_sys';
    END IF;
END
$body$;
'@

Write-Host "11. Дамп только net + ref + meta"
& pg_dump.exe -h $DbHost -p $Port -U $User -d $BuildDatabase `
    -Fc --schema=net --schema=ref --schema=meta -f $ProductDump
if ($LASTEXITCODE -ne 0) {
    throw "Не удалось создать продуктовый дамп $ProductDump"
}

Write-Host "12. Чистая продуктовая БД $Target"
Assert-CanCreateDatabase $Target
New-PostgisDatabase $Target
$productRestoreOutput = & pg_restore.exe -h $DbHost -p $Port -U $User `
    -d $Target --no-owner --no-privileges $ProductDump 2>&1
$productRestoreExitCode = $LASTEXITCODE
if ($productRestoreExitCode -ne 0) {
    $productRestoreOutput | Select-Object -Last 30
    throw "Восстановление продукта завершилось с кодом $productRestoreExitCode"
}

Invoke-PsqlCmd $Target @'
INSERT INTO public.spatial_ref_sys
    (srid, auth_name, auth_srid, srtext, proj4text)
SELECT srid, auth_name, auth_srid, srtext, proj4text
FROM meta.spatial_ref_system
WHERE srid = 9998
ON CONFLICT (srid) DO UPDATE SET
    auth_name = EXCLUDED.auth_name,
    auth_srid = EXCLUDED.auth_srid,
    srtext = EXCLUDED.srtext,
    proj4text = EXCLUDED.proj4text;
'@

& python (Join-Path $root 'tools\check_gis_ready.py') --db $Target
if ($LASTEXITCODE -ne 0) {
    throw 'Финальная продуктовая БД не прошла GIS-аудит'
}

if (-not $KeepBuildDatabase) {
    Write-Host "13. Удаление временной БД $BuildDatabase"
    Remove-Database $BuildDatabase
}

Write-Host @"

Готово: $Target
  net.*              — объектные GIS-слои
  ref.*              — справочники с PK/индексами и собственными sequence
  meta.layer_catalog — каталог слоёв
  public             — только объекты расширения PostGIS, legacy-таблиц нет
"@
