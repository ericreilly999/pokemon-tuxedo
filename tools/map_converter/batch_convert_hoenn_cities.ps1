# Batch converter for all Hoenn cities from Pokemon Crossroads to FireRed format.
# Converts 16 cities total.

$ErrorActionPreference = "Stop"

# List of all Hoenn cities
$HOENN_CITIES = @(
    "LittlerootTown",
    "OldaleTown",
    "PetalburgCity",
    "RustboroCity",
    "DewfordTown",
    "SlateportCity",
    "MauvilleCity",
    "VerdanturfTown",
    "FallarborTown",
    "LavaridgeTown",
    "FortreeCity",
    "LilycoveCity",
    "MossdeepCity",
    "SootopolisCity",
    "PacifidlogTown",
    "EverGrandeCity"
)

function Convert-Map {
    param(
        [string]$InputPath,
        [string]$OutputPath
    )
    
    try {
        $emeraldMap = Get-Content -Path $InputPath -Raw | ConvertFrom-Json
        
        # Create FireRed format map (most fields are compatible)
        $fireredMap = [ordered]@{
            id = if ($emeraldMap.id) { $emeraldMap.id } else { "" }
            name = if ($emeraldMap.name) { $emeraldMap.name } else { "" }
            layout = if ($emeraldMap.layout) { $emeraldMap.layout } else { "" }
            music = if ($emeraldMap.music) { $emeraldMap.music } else { "" }
            region_map_section = if ($emeraldMap.region_map_section) { $emeraldMap.region_map_section } else { "" }
            requires_flash = if ($null -ne $emeraldMap.requires_flash) { $emeraldMap.requires_flash } else { $false }
            weather = if ($emeraldMap.weather) { $emeraldMap.weather } else { "WEATHER_SUNNY" }
            map_type = if ($emeraldMap.map_type) { $emeraldMap.map_type } else { "MAP_TYPE_TOWN" }
            allow_cycling = if ($null -ne $emeraldMap.allow_cycling) { $emeraldMap.allow_cycling } else { $true }
            allow_escaping = if ($null -ne $emeraldMap.allow_escaping) { $emeraldMap.allow_escaping } else { $false }
            allow_running = if ($null -ne $emeraldMap.allow_running) { $emeraldMap.allow_running } else { $true }
            show_map_name = if ($null -ne $emeraldMap.show_map_name) { $emeraldMap.show_map_name } else { $true }
            floor_number = if ($null -ne $emeraldMap.floor_number) { $emeraldMap.floor_number } else { 0 }
            battle_scene = if ($emeraldMap.battle_scene) { $emeraldMap.battle_scene } else { "MAP_BATTLE_SCENE_NORMAL" }
            region_id = 2  # Hoenn = region 2 (Kanto=0, Johto=1, Hoenn=2, Sinnoh=3)
            connections = if ($emeraldMap.connections) { @($emeraldMap.connections) } else { @() }
            object_events = if ($emeraldMap.object_events) { @($emeraldMap.object_events) } else { @() }
            warp_events = if ($emeraldMap.warp_events) { @($emeraldMap.warp_events) } else { @() }
            coord_events = if ($emeraldMap.coord_events) { @($emeraldMap.coord_events) } else { @() }
            bg_events = if ($emeraldMap.bg_events) { @($emeraldMap.bg_events) } else { @() }
        }
        
        # Ensure output directory exists
        $outputDir = Split-Path -Parent $OutputPath
        if (-not (Test-Path $outputDir)) {
            New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
        }
        
        # Write output
        $fireredMap | ConvertTo-Json -Depth 10 | Set-Content -Path $OutputPath -Encoding UTF8
        return @{ Success = $true }
    }
    catch {
        return @{ Success = $false; Error = $_.Exception.Message }
    }
}

# Main script
$scriptDir = $PSScriptRoot
$baseInputDir = Join-Path $scriptDir "..\..\pokemon-crossroads\data\maps"
$baseOutputDir = Join-Path $scriptDir "..\..\data\maps\hoenn_converted"

# Create output directory
if (-not (Test-Path $baseOutputDir)) {
    New-Item -ItemType Directory -Path $baseOutputDir -Force | Out-Null
}

$successCount = 0
$failCount = 0
$skipCount = 0
$conversionReport = @()

Write-Host ("=" * 70)
Write-Host "HOENN CITIES CONVERSION REPORT"
Write-Host ("=" * 70)
Write-Host "Input directory: $baseInputDir"
Write-Host "Output directory: $baseOutputDir"
Write-Host ""

foreach ($city in $HOENN_CITIES) {
    $inputPath = Join-Path $baseInputDir "$city\map.json"
    $outputPath = Join-Path $baseOutputDir "$city.json"
    
    Write-Host "Converting $city... " -NoNewline
    
    if (-not (Test-Path $inputPath)) {
        Write-Host "SKIP (source not found)"
        $conversionReport += @{
            city = $city
            status = "SKIP"
            reason = "Source file not found: $inputPath"
        }
        $skipCount++
        continue
    }
    
    $result = Convert-Map -InputPath $inputPath -OutputPath $outputPath
    
    if ($result.Success) {
        Write-Host "SUCCESS"
        $conversionReport += @{
            city = $city
            status = "SUCCESS"
            input = $inputPath
            output = $outputPath
        }
        $successCount++
    }
    else {
        Write-Host "FAILED: $($result.Error)"
        $conversionReport += @{
            city = $city
            status = "FAILED"
            reason = $result.Error
        }
        $failCount++
    }
}

Write-Host ""
Write-Host ("=" * 70)
Write-Host "SUMMARY"
Write-Host ("=" * 70)
Write-Host "Total cities: $($HOENN_CITIES.Count)"
Write-Host "Successfully converted: $successCount"
Write-Host "Failed: $failCount"
Write-Host "Skipped: $skipCount"
Write-Host ""

# Write detailed report
$reportPath = Join-Path $baseOutputDir "cities_conversion_report.json"
@{
    region = "Hoenn"
    type = "cities"
    total = $HOENN_CITIES.Count
    success = $successCount
    failed = $failCount
    skipped = $skipCount
    details = $conversionReport
} | ConvertTo-Json -Depth 10 | Set-Content -Path $reportPath -Encoding UTF8

Write-Host "Detailed report written to: $reportPath"
Write-Host ""

# Return success if all cities converted
if ($successCount -eq $HOENN_CITIES.Count) {
    Write-Host "All Hoenn cities converted successfully!" -ForegroundColor Green
    exit 0
}
elseif ($successCount -gt 0) {
    Write-Host "Partial success: $successCount/$($HOENN_CITIES.Count) cities converted" -ForegroundColor Yellow
    exit 1
}
else {
    Write-Host "No cities were converted" -ForegroundColor Red
    exit 2
}
