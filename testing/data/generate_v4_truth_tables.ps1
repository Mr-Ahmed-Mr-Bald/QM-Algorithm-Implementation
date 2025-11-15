# Generates all possible truth-tables for 4 variables (65536 functions)
# Output files go to a subfolder `v4` next to this script.
# Each file named: test_v4_fXXXXX.txt (XXXXX = function index 0..65535, zero-padded 5 digits)
# File format matches existing tests: first line = number of variables (4)
# second line = comma-separated minterms (e.g. m0, m1, ...) or an empty line for no minterms
# No don't-cares by default.

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$outDir = Join-Path $scriptDir 'v4'
if (-not (Test-Path $outDir)) { New-Item -ItemType Directory -Path $outDir | Out-Null }

$progressInterval = 1000
for ($f = 0; $f -le 65535; $f++) {
    # collect minterms where bit is 1
    $minterms = New-Object System.Collections.Generic.List[string]
    for ($i = 0; $i -lt 16; $i++) {
        if ((($f -shr $i) -band 1) -eq 1) { $minterms.Add("m$i") }
    }

    $fileName = Join-Path $outDir ('test_v4_f{0:D5}.txt' -f $f)
    $content = "4`n"
    if ($minterms.Count -gt 0) { $content += ($minterms -join ', ') + "`n" } else { $content += "`n" }

    Set-Content -Path $fileName -Value $content -Encoding ASCII

    if (($f % $progressInterval) -eq 0) { Write-Host "Generated $f / 65535" }
}
Write-Host "Done. Files created in: $outDir"