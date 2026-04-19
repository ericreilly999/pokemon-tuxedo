$bytes = [System.IO.File]::ReadAllBytes("charmap.txt")
$text = [System.Text.Encoding]::UTF8.GetString($bytes)
$text = $text.Replace("`r`n", "`n")
$bytes = [System.Text.Encoding]::UTF8.GetBytes($text)
[System.IO.File]::WriteAllBytes("charmap.txt", $bytes)
Write-Host "Done. File size: $($bytes.Length) bytes"
