param(
    [Parameter(Mandatory = $true)]
    [string]$App,

    [Parameter(Mandatory = $true)]
    [string]$WindeployQt,

    [Parameter(Mandatory = $true)]
    [string]$PostgresBin,

    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$Configuration = 'Release'
)

$ErrorActionPreference = 'Stop'

function Resolve-ExistingFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "$Description not found: $Path"
    }

    return (Resolve-Path -LiteralPath $Path).Path
}

function Resolve-ExistingDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Container)) {
        throw "$Description not found: $Path"
    }

    return (Resolve-Path -LiteralPath $Path).Path
}

$appPath = Resolve-ExistingFile -Path $App -Description 'Application executable'
$windeployqtPath = Resolve-ExistingFile -Path $WindeployQt -Description 'windeployqt'
$postgresBinPath = Resolve-ExistingDirectory -Path $PostgresBin -Description 'PostgreSQL bin directory'
$destination = Split-Path -Parent $appPath
$qtBin = Split-Path -Parent $windeployqtPath
$qtRoot = Split-Path -Parent $qtBin
$qtPluginRoot = Join-Path $qtRoot 'plugins'

$deployArguments = @(
    '--force'
    '--no-translations'
    '--no-system-d3d-compiler'
    '--no-opengl-sw'
)

if ($Configuration -eq 'Debug') {
    $deployArguments += '--debug'
    $sqlDriverName = 'qsqlpsqld.dll'
} else {
    $deployArguments += '--release'
    $sqlDriverName = 'qsqlpsql.dll'
}

$deployArguments += $appPath

Write-Host "Deploying Qt runtime to $destination"
& $windeployqtPath @deployArguments
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}

$sqlDriverSource = Resolve-ExistingFile `
    -Path (Join-Path $qtPluginRoot "sqldrivers\$sqlDriverName") `
    -Description 'Qt PostgreSQL SQL driver'
$sqlDriverDestination = Join-Path $destination 'sqldrivers'
New-Item -ItemType Directory -Path $sqlDriverDestination -Force | Out-Null
Copy-Item -LiteralPath $sqlDriverSource -Destination $sqlDriverDestination -Force

$postgresLibraries = @(
    'libpq.dll'
    'libssl-3-x64.dll'
    'libcrypto-3-x64.dll'
    'libintl-9.dll'
    'libiconv-2.dll'
    'libwinpthread-1.dll'
)

foreach ($library in $postgresLibraries) {
    $source = Resolve-ExistingFile `
        -Path (Join-Path $postgresBinPath $library) `
        -Description "PostgreSQL runtime library $library"
    Copy-Item -LiteralPath $source -Destination $destination -Force
}

$requiredOutputFiles = @(
    (Join-Path $destination 'Qt6Core.dll')
    (Join-Path $destination 'Qt6Sql.dll')
    (Join-Path $destination 'platforms\qwindows.dll')
    (Join-Path $sqlDriverDestination $sqlDriverName)
    (Join-Path $destination 'libpq.dll')
)

foreach ($requiredFile in $requiredOutputFiles) {
    Resolve-ExistingFile -Path $requiredFile -Description 'Deployed runtime file' | Out-Null
}

Write-Host 'Qt SQL and PostgreSQL runtime deployment completed.'
