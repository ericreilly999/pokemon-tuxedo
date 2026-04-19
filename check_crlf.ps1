$files = Get-ChildItem -Path "src" -Filter "*.c" -Recurse
foreach ($f in $files) {
    $bytes = [System.IO.File]::ReadAllBytes($f.FullName)
    $hasCRLF = $false
    for ($i = 0; $i -lt $bytes.Length - 1; $i++) {
        if ($bytes[$i] -eq 13 -and $bytes[$i+1] -eq 10) {
            $hasCRLF = $true
            break
        }
    }
    if ($hasCRLF) {
        Write-Host "CRLF: $($f.Name)"
    }
}
# Also check charmap.txt
$bytes = [System.IO.File]::ReadAllBytes("charmap.txt")
$hasCRLF = $false
for ($i = 0; $i -lt $bytes.Length - 1; $i++) {
    if ($bytes[$i] -eq 13 -and $bytes[$i+1] -eq 10) {
        $hasCRLF = $true
        break
    }
}
Write-Host "charmap.txt CRLF: $hasCRLF"
# Check include headers too
$hfiles = Get-ChildItem -Path "include" -Filter "*.h" -Recurse
foreach ($f in $hfiles) {
    $bytes = [System.IO.File]::ReadAllBytes($f.FullName)
    $hasCRLF = $false
    for ($i = 0; $i -lt $bytes.Length - 1; $i++) {
        if ($bytes[$i] -eq 13 -and $bytes[$i+1] -eq 10) {
            $hasCRLF = $true
            break
        }
    }
    if ($hasCRLF) {
        Write-Host "CRLF: $($f.Name)"
    }
}
Write-Host "Done checking."
