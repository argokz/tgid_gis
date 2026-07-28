# Установка новых запросов чтения схемы в каталог приложения.
#
# Приложение грузит запросы из файлов в рантайме (cxema/read_nodes.cpp:225,
# cxema/read_lines.cpp:192), поэтому перекомпиляция не нужна — достаточно
# подменить файлы. Оригиналы сохраняются рядом с суффиксом .orig.
#
# Файлы читаются функцией readQ (mystd.cpp:189), которая прогоняет
# содержимое через cp2utf, то есть ждёт CP1251. Генератор пишет именно
# в этой кодировке, поэтому копируем побайтово.
#
#   .\tools\deploy_sql.ps1                 # установить
#   .\tools\deploy_sql.ps1 -Rollback       # вернуть оригиналы

param(
    [string]$GidrDir  = 'H:\projects\tgid-app-new\gid6\gidr',
    [switch]$Rollback
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

# Куда что кладём: у узлов есть отдельный вариант для PostgreSQL.
$targets = @(
    @{ Src = 'sql\sql3\us_net.sql'; Dst = 'postgresql\sql3\us.sql' },
    @{ Src = 'sql\sql3\ut_net.sql'; Dst = 'sql3\ut.sql' }
)

foreach ($t in $targets) {
    $dst = Join-Path $GidrDir $t.Dst
    $bak = "$dst.orig"

    if ($Rollback) {
        if (Test-Path $bak) {
            Copy-Item $bak $dst -Force
            Remove-Item $bak -Force
            Write-Host "возвращён оригинал: $($t.Dst)"
        } else {
            Write-Host "нечего возвращать: $($t.Dst)"
        }
        continue
    }

    if (-not (Test-Path $dst)) {
        Write-Warning "не найден целевой файл $dst — пропускаю"
        continue
    }
    if (-not (Test-Path $bak)) {
        Copy-Item $dst $bak
        Write-Host "сохранён оригинал: $($t.Dst).orig"
    }

    $src = Join-Path $root $t.Src
    if (-not (Test-Path $src)) { throw "не найден $src" }

    Copy-Item $src $dst -Force
    Write-Host ("установлен {0} -> {1} ({2:N0} байт)" -f $t.Src, $t.Dst,
                (Get-Item $dst).Length)
}

if (-not $Rollback) {
    Write-Host ""
    Write-Host "Откат: .\tools\deploy_sql.ps1 -Rollback"
}
