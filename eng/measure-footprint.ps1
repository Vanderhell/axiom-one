param(
    [Parameter(Mandatory = $true)]
    [string]$BuildDirectory,
    [string]$Configuration = "Release",
    [string]$OutputPath = "docs/footprint-report.md"
)

$ErrorActionPreference = "Stop"
$root = (Resolve-Path $BuildDirectory).Path
$output = [System.IO.Path]::GetFullPath((Join-Path (Get-Location) $OutputPath))

function Get-PeSections([string]$Path) {
    $stream = [System.IO.File]::OpenRead($Path)
    $reader = [System.IO.BinaryReader]::new($stream)
    try {
        $stream.Position = 0x3c
        $peOffset = $reader.ReadUInt32()
        $stream.Position = [int64]$peOffset + 6
        $sectionCount = $reader.ReadUInt16()
        $stream.Position = [int64]$peOffset + 20
        $optionalHeaderSize = $reader.ReadUInt16()
        $stream.Position = [int64]$peOffset + 24 + $optionalHeaderSize
        $sections = @{}
        for ($index = 0; $index -lt $sectionCount; ++$index) {
            $nameBytes = $reader.ReadBytes(8)
            $name = [System.Text.Encoding]::ASCII.GetString($nameBytes).Trim([char]0)
            $virtualSize = $reader.ReadUInt32()
            $null = $reader.ReadUInt32()
            $rawSize = $reader.ReadUInt32()
            $stream.Position += 16
            $sections[$name] = [pscustomobject]@{ VirtualSize = $virtualSize; RawSize = $rawSize }
        }
        return $sections
    }
    finally {
        $reader.Dispose()
        $stream.Dispose()
    }
}

$executables = Get-ChildItem -Path $root -Recurse -File -Filter "one_footprint_*.exe" |
    Where-Object { $_.DirectoryName -match [regex]::Escape($Configuration) }
if ($executables.Count -eq 0) { throw "No footprint executables found for configuration '$Configuration' under $root." }

$rows = foreach ($exe in $executables) {
    $sections = Get-PeSections $exe.FullName
    $map = Join-Path $exe.DirectoryName ($exe.BaseName + ".map")
    $members = if (Test-Path $map) {
        (Select-String -Path $map -Pattern 'one_[a-z]+\.obj' -AllMatches | ForEach-Object {
            $_.Matches.Value.ToLowerInvariant()
        } | Sort-Object -Unique) -join ', '
    } else { "map unavailable" }
    [pscustomobject]@{
        Target = $exe.BaseName
        ExeBytes = $exe.Length
        TextRawBytes = if ($sections.ContainsKey('.text')) { $sections['.text'].RawSize } else { 0 }
        TextVirtualBytes = if ($sections.ContainsKey('.text')) { $sections['.text'].VirtualSize } else { 0 }
        Members = $members
    }
}

$baseline = $rows | Where-Object Target -eq 'one_footprint_baseline'
if ($null -eq $baseline) { throw "The baseline executable is missing." }
$rows = $rows | Sort-Object @{ Expression = { if ($_.Target -eq 'one_footprint_baseline') { 0 } else { 1 } } }, Target

$lines = @(
    '# ONE static-link footprint report',
    '',
    "Configuration: $Configuration",
    '',
    'Every consumer includes `one.h`; the baseline calls no ONE API. `Delta` therefore measures the incremental PE footprint relative to inclusion of the umbrella header alone.',
    '',
    '| Target | EXE bytes | Delta EXE | .text raw | Delta .text | Linked ONE objects |',
    '| --- | ---: | ---: | ---: | ---: | --- |'
)
foreach ($row in $rows) {
    $lines += "| $($row.Target) | $($row.ExeBytes) | $($row.ExeBytes - $baseline.ExeBytes) | $($row.TextRawBytes) | $($row.TextRawBytes - $baseline.TextRawBytes) | $($row.Members) |"
}

Set-Content -Path $output -Value $lines -Encoding utf8
Write-Output "Wrote $output"
