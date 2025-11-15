<#
Runs `QM_Algorithm_Implementation.exe` for every v4 test file and aggregates console output.

Behavior:
- Looks for files `testing/data/v4/test_v4_f*.txt` relative to this script.
- For each file it runs the program non-interactively by piping the menu commands:
    1         # load from file
    <path>    # path to test file (relative to exe working dir)
    3         # run minimization
    4         # display results
    6         # exit
- Appends program stdout/stderr to a single log: `testing/data/v4/test_run_results.log`.
- Adds headers with filename and timestamps to the log for clarity.

Notes:
- The script assumes the built executable is at: project_root/build/QM_Algorithm_Implementation.exe
  relative to this script it resolves to: ..\..\build\QM_Algorithm_Implementation.exe
- Running all 65,536 functions will take time. Consider testing a small sample first.

Usage:
  Set-Location <project-root> (optional)
  & .\testing\data\run_all_tests_v4.ps1

#>

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
# exe is expected in project root build folder
$exePathCandidate = Join-Path $scriptDir '..\..\build\QM_Algorithm_Implementation.exe'
$exePath = (Resolve-Path $exePathCandidate -ErrorAction SilentlyContinue).Path
if (-not $exePath) {
    Write-Error "Could not find executable at '$exePathCandidate'. Build the project first or set the correct path in the script."; exit 1
}

$v4dir = Join-Path $scriptDir 'v4'
if (-not (Test-Path $v4dir)) { Write-Error "v4 test directory not found: $v4dir"; exit 1 }

$logFile = Join-Path $v4dir 'test_run_results.log'
# Start fresh
if (Test-Path $logFile) { Remove-Item $logFile -Force }

# Control variables
$sample = $null # if you want to test only first N files set this to integer
$counter = 0

# iterate in sorted order
$files = Get-ChildItem -Path $v4dir -Filter 'test_v4_f*.txt' -File | Sort-Object Name
if ($sample -ne $null) { $files = $files | Select-Object -First $sample }

foreach ($f in $files) {
    $counter++
    $header = ("`n==================== TEST {0}: {1} ====================`n" -f $counter, $f.Name)
    $header += ("Started: {0}`n" -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'))
    $header += ("File: {0}`n" -f $f.FullName)
    $header | Out-File -FilePath $logFile -Append -Encoding UTF8

    # compute path relative to build folder (working dir will be exe's folder)
    # from build dir to file: ..\testing\data\v4\...
    $relativePathFromExe = Join-Path '..\testing\data\v4' $f.Name

    # prepare interactive input: 1 (load file), filename, 3 (minimize), 4 (display), 6 (exit)
    $inputLines = @(
        '1',
        $relativePathFromExe,
        '3',
        '4',
        '6'
    ) -join "`n"

    try {
        # Run the exe with the working directory set to its folder so relative paths resolve
        $exeDir = Split-Path -Parent $exePath
        Push-Location $exeDir
        # Run and capture output; pipe input lines to the process
        $procOutput = $inputLines | & $exePath 2>&1
        Pop-Location

        # append output
        $procOutput | Out-File -FilePath $logFile -Append -Encoding UTF8
        $footer = "Finished: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')`n"
        $footer += "Elapsed: N/A (wall-clock measured by user)" + "`n"
        $footer | Out-File -FilePath $logFile -Append -Encoding UTF8
    }
    catch {
        $err = "ERROR running test $($f.Name): $_"
        $err | Out-File -FilePath $logFile -Append -Encoding UTF8
    }

    # small throttle to avoid overloading I/O (adjust or remove if desired)
    Start-Sleep -Milliseconds 50
}

Write-Host "All done. Results appended to: $logFile" -ForegroundColor Green
