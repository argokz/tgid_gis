# Установка новых запросов чтения схемы в каталоги приложения.
#
# Приложение грузит запросы из файлов в рантайме (cxema/read_nodes.cpp,
# cxema/read_lines.cpp), поэтому перекомпиляция не нужна — достаточно
# подменить файлы. Оригиналы сохраняются рядом с суффиксом .orig.
#
# Файлы читаются функцией readQ (mystd.cpp), которая прогоняет
# содержимое через cp2utf, то есть ждёт CP1251. Генератор пишет именно
# в этой кодировке, поэтому копируем побайтово.
#
# Раскатываем во все известные каталоги gidr/runtime: раньше скрипт
# писал только в gid6\gidr, а собранный gid8.exe читал runtime/сборку —
# правка ST_X «устанавливалась», но приложение её не видело.
#
#   .\tools\deploy_sql.ps1                 # установить
#   .\tools\deploy_sql.ps1 -Rollback       # вернуть оригиналы

param(
    [string[]]$GidrDir = @(),
    [switch]$Rollback
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

if ($GidrDir.Count -eq 0) {
    $GidrDir = @(
        (Join-Path $root 'gid8\runtime'),
        (Join-Path $root 'gid6\gidr'),
        'H:\projects\tgid-app-new\gid6\gidr'
    )
    Get-ChildItem 'H:\build' -Directory -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -like 'gid8*' } |
        ForEach-Object {
            $cand = Join-Path $_.FullName 'gidr'
            if (Test-Path $cand) { $GidrDir += $cand }
            $cand2 = Join-Path $_.FullName 'runtime'
            if (Test-Path $cand2) { $GidrDir += $cand2 }
        }
    $GidrDir = $GidrDir | Select-Object -Unique
}

# Куда что кладём: у узлов есть отдельный вариант для PostgreSQL.
$targets = @(
    @{ Src = 'sql\sql3\us_net.sql'; Dst = 'postgresql\sql3\us.sql' },
    @{ Src = 'sql\sql3\ut_net.sql'; Dst = 'sql3\ut.sql' }
)

$deployed = 0
foreach ($dir in $GidrDir) {
    if (-not (Test-Path $dir)) {
        Write-Host "пропуск (нет каталога): $dir"
        continue
    }
    Write-Host "=== $dir ==="
    foreach ($t in $targets) {
        $dst = Join-Path $dir $t.Dst
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

        $dstParent = Split-Path -Parent $dst
        if (-not (Test-Path $dstParent)) {
            Write-Warning "нет каталога $dstParent — пропускаю $($t.Dst)"
            continue
        }
        if (-not (Test-Path $dst)) {
            # В runtime файл может ещё не существовать — создаём.
            Write-Host "целевого файла не было, создаю: $($t.Dst)"
        } elseif (-not (Test-Path $bak)) {
            Copy-Item $dst $bak
            Write-Host "сохранён оригинал: $($t.Dst).orig"
        }

        $src = Join-Path $root $t.Src
        if (-not (Test-Path $src)) { throw "не найден $src" }

        Copy-Item $src $dst -Force
        Write-Host ("установлен {0} -> {1} ({2:N0} байт)" -f $t.Src, $t.Dst,
                    (Get-Item $dst).Length)
        $deployed++
    }
}

if (-not $Rollback) {
    Write-Host ""
    Write-Host "развёрнуто файлов: $deployed"
    Write-Host "Откат: .\tools\deploy_sql.ps1 -Rollback"
}
