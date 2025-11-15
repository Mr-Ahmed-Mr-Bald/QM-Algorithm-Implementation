How to generate v4 test files

This directory contains `generate_v4_truth_tables.ps1`, a PowerShell script that writes all possible truth-table test files for 4 variables.

- Script output folder: `testing/data/v4`
- Files produced: `test_v4_f00000.txt` .. `test_v4_f65535.txt` (65,536 files)
- Each file format:
  - Line 1: number of variables (`4`)
  - Line 2: comma-separated minterms (e.g. `m0, m1, m5`) or empty if no minterms

To run (PowerShell):

```powershell
Set-Location 'c:\Users\mahmo\OneDrive\Desktop\auc\DD1\Project\QM-Algorithm-Implementation\testing\data'
# Dry run: simulate generation for first N functions (uncomment and edit N)
# for ($f=0; $f -lt 16; $f++) { Write-Host "Would generate function $f" }

# Full generation (creates 65,536 files, may take a while):
.
\generate_v4_truth_tables.ps1
```

Notes & recommendations:
- Creating 65,536 files will consume disk entries; ensure you have enough free space / inode support.
- If you want a subset (e.g. all functions with small number of minterms, or random sample), I can modify the script to produce only those.
- I did not run the script; tell me to run it if you want me to generate the files now.