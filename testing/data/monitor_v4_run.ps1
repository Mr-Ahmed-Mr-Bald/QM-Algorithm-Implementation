# Monitor script for v4 run
# Prints periodic summary: tests completed, ERROR lines, last processed file.
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$v4dir = Join-Path $scriptDir 'v4'
$logFile = Join-Path $v4dir 'test_run_results.log'

if (-not (Test-Path $v4dir)) { Write-Host "v4 directory not found: $v4dir"; exit 1 }

$totalFiles = (Get-ChildItem -Path $v4dir -Filter 'test_v4_f*.txt' -File -ErrorAction SilentlyContinue).Count
Write-Host "Monitoring run. Total v4 files: $totalFiles. Log: $logFile`n"

$lastCount = -1
while ($true) {
    if (-not (Test-Path $logFile)) {
        Write-Host "$(Get-Date -Format 'HH:mm:ss')  Waiting for log to appear..."
    }
    else {
        # count completed tests by counting header markers
        try {
            $contentTail = Get-Content -Path $logFile -Tail 1000 -ErrorAction Stop
        }
        catch {
            Start-Sleep -Seconds 1
            continue
        }

        $completed = (Select-String -InputObject $contentTail -Pattern '=====+ TEST ' -AllMatches | Measure-Object).Count
        # fallback: full file search if tail misses
        if ($completed -eq 0) {
            $completed = (Get-Content $logFile | Select-String '=====+ TEST ' -AllMatches | Measure-Object).Count
        }

        $errors = (Select-String -Path $logFile -Pattern '^ERROR' -SimpleMatch -AllMatches -ErrorAction SilentlyContinue | Measure-Object).Count
        # get last File: line
        $lastFileLine = ($contentTail | Select-String -Pattern '^File:' | Select-Object -Last 1).ToString()
        if ($lastFileLine) { $lastFile = $lastFileLine -replace '^File:\s*','' } else { $lastFile = '(none yet)' }

        if ($completed -ne $lastCount) {
            Write-Host "$(Get-Date -Format 'HH:mm:ss')  Completed: $completed / $totalFiles    Errors: $errors    Last: $lastFile"
            $lastCount = $completed
        }

        # If we've completed all tests, break
        if ($totalFiles -gt 0 -and $completed -ge $totalFiles) {
            Write-Host "$(Get-Date -Format 'HH:mm:ss')  All tests appear completed (completed >= totalFiles). Final Errors: $errors"
            break
        }
    }
    Start-Sleep -Seconds 10
}

# Print last 200 lines of log for quick inspection
if (Test-Path $logFile) {
    Write-Host "\n==== Last 200 lines of log ===="
    Get-Content -Path $logFile -Tail 200
}
Write-Host "Monitor finished."