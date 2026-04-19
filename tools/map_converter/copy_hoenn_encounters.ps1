# Hoenn Wild Encounters Converter
# Task 2.9: Convert Hoenn wild encounters
# 
# This script extracts Hoenn wild encounter data from Pokemon Crossroads
# and copies it to the FireRed project in JSON format.

param(
    [string]$SourcePath = "pokemon-crossroads/src/data/wild_encounters.json",
    [string]$DestPath = "src/data/hoenn_wild_encounters.json"
)

Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Hoenn Wild Encounters Converter - Task 2.9" -ForegroundColor Cyan
Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Source: $SourcePath"
Write-Host "Destination: $DestPath"
Write-Host "============================================================="

# All Hoenn maps that need wild encounters
$HoennMaps = @(
    # Routes
    "MAP_ROUTE101", "MAP_ROUTE102", "MAP_ROUTE103", "MAP_ROUTE104", "MAP_ROUTE105",
    "MAP_ROUTE106", "MAP_ROUTE107", "MAP_ROUTE108", "MAP_ROUTE109", "MAP_ROUTE110",
    "MAP_ROUTE111", "MAP_ROUTE112", "MAP_ROUTE113", "MAP_ROUTE114", "MAP_ROUTE115",
    "MAP_ROUTE116", "MAP_ROUTE117", "MAP_ROUTE118", "MAP_ROUTE119", "MAP_ROUTE120",
    "MAP_ROUTE121", "MAP_ROUTE122", "MAP_ROUTE123", "MAP_ROUTE124", "MAP_ROUTE125",
    "MAP_ROUTE126", "MAP_ROUTE127", "MAP_ROUTE128", "MAP_ROUTE129", "MAP_ROUTE130",
    "MAP_ROUTE131", "MAP_ROUTE132", "MAP_ROUTE133", "MAP_ROUTE134",
    
    # Dungeons and caves
    "MAP_PETALBURG_WOODS",
    "MAP_GRANITE_CAVE_1F", "MAP_GRANITE_CAVE_B1F", "MAP_GRANITE_CAVE_B2F",
    "MAP_RUSTURF_TUNNEL",
    "MAP_METEOR_FALLS_1F_1R", "MAP_METEOR_FALLS_1F_2R", "MAP_METEOR_FALLS_B1F_1R", "MAP_METEOR_FALLS_B1F_2R",
    "MAP_MT_CHIMNEY",
    "MAP_JAGGED_PASS",
    "MAP_FIERY_PATH",
    "MAP_MT_PYRE_1F", "MAP_MT_PYRE_2F", "MAP_MT_PYRE_3F", "MAP_MT_PYRE_4F",
    "MAP_MT_PYRE_5F", "MAP_MT_PYRE_6F", "MAP_MT_PYRE_EXTERIOR", "MAP_MT_PYRE_SUMMIT",
    "MAP_SHOAL_CAVE_HIGH_TIDE_ENTRANCE_ROOM", "MAP_SHOAL_CAVE_HIGH_TIDE_INNER_ROOM",
    "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM",
    "MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM",
    "MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM",
    "MAP_CAVE_OF_ORIGIN_ENTRANCE", "MAP_CAVE_OF_ORIGIN_1F", "MAP_CAVE_OF_ORIGIN_B1F",
    "MAP_SEAFLOOR_CAVERN_ENTRANCE", "MAP_SEAFLOOR_CAVERN_ROOM1", "MAP_SEAFLOOR_CAVERN_ROOM2",
    "MAP_SEAFLOOR_CAVERN_ROOM3", "MAP_SEAFLOOR_CAVERN_ROOM4", "MAP_SEAFLOOR_CAVERN_ROOM5",
    "MAP_SEAFLOOR_CAVERN_ROOM6", "MAP_SEAFLOOR_CAVERN_ROOM7", "MAP_SEAFLOOR_CAVERN_ROOM8",
    "MAP_SEAFLOOR_CAVERN_ROOM9",
    "MAP_SKY_PILLAR_ENTRANCE", "MAP_SKY_PILLAR_OUTSIDE", "MAP_SKY_PILLAR_1F", "MAP_SKY_PILLAR_2F",
    "MAP_SKY_PILLAR_3F", "MAP_SKY_PILLAR_4F", "MAP_SKY_PILLAR_5F", "MAP_SKY_PILLAR_TOP",
    "MAP_VICTORY_ROAD_1F", "MAP_VICTORY_ROAD_B1F", "MAP_VICTORY_ROAD_B2F",
    
    # Safari Zone
    "MAP_SAFARI_ZONE_NORTHWEST", "MAP_SAFARI_ZONE_NORTH", "MAP_SAFARI_ZONE_NORTHEAST",
    "MAP_SAFARI_ZONE_SOUTHWEST", "MAP_SAFARI_ZONE_SOUTH", "MAP_SAFARI_ZONE_SOUTHEAST",
    
    # Other areas
    "MAP_NEW_MAUVILLE_INSIDE", "MAP_NEW_MAUVILLE_ENTRANCE",
    "MAP_ABANDONED_SHIP_DECK", "MAP_ABANDONED_SHIP_CORRIDORS_1F", "MAP_ABANDONED_SHIP_ROOMS_1F",
    "MAP_ABANDONED_SHIP_CORRIDORS_B1F", "MAP_ABANDONED_SHIP_ROOMS_B1F",
    "MAP_ABANDONED_SHIP_ROOMS2_B1F", "MAP_ABANDONED_SHIP_UNDERWATER1", "MAP_ABANDONED_SHIP_UNDERWATER2",
    "MAP_UNDERWATER_ROUTE124", "MAP_UNDERWATER_ROUTE126", "MAP_UNDERWATER_ROUTE127",
    "MAP_UNDERWATER_ROUTE128", "MAP_UNDERWATER_ROUTE129", "MAP_UNDERWATER_ROUTE105",
    "MAP_UNDERWATER_SEAFLOOR_CAVERN", "MAP_UNDERWATER_SOOTOPOLIS_CITY"
)

Write-Host "`nSearching for $($HoennMaps.Count) Hoenn map encounters..." -ForegroundColor Yellow

# Check if source file exists
if (-not (Test-Path $SourcePath)) {
    Write-Host "ERROR: Source file not found: $SourcePath" -ForegroundColor Red
    exit 1
}

# Read and parse the JSON file
try {
    $content = Get-Content $SourcePath -Raw
    $data = $content | ConvertFrom-Json
}
catch {
    Write-Host "ERROR: Failed to parse JSON: $_" -ForegroundColor Red
    exit 1
}

# Find the main encounter group
$encounterGroup = $data.wild_encounter_groups | Where-Object { $_.label -eq "gWildMonHeaders" }

if (-not $encounterGroup) {
    Write-Host "ERROR: Could not find gWildMonHeaders encounter group" -ForegroundColor Red
    exit 1
}

Write-Host "Found $($encounterGroup.encounters.Count) total map encounters in source" -ForegroundColor Gray

# Extract Hoenn encounters
$hoennEncounters = @()
$foundMaps = @()
$missingMaps = @()

foreach ($mapName in $HoennMaps) {
    $encounter = $encounterGroup.encounters | Where-Object { $_.map -eq $mapName }
    if ($encounter) {
        $hoennEncounters += $encounter
        $foundMaps += $mapName
    }
    else {
        $missingMaps += $mapName
    }
}

Write-Host "Extracted $($hoennEncounters.Count) Hoenn map encounters" -ForegroundColor Green

if ($missingMaps.Count -gt 0) {
    Write-Host "`nMaps without encounters ($($missingMaps.Count)):" -ForegroundColor Yellow
    foreach ($missing in $missingMaps[0..9]) {
        Write-Host "  - $missing" -ForegroundColor Yellow
    }
    if ($missingMaps.Count -gt 10) {
        Write-Host "  ... and $($missingMaps.Count - 10) more" -ForegroundColor Yellow
    }
}

# Create destination directory if needed
$destDir = Split-Path $DestPath -Parent
if (-not (Test-Path $destDir)) {
    New-Item -ItemType Directory -Path $destDir -Force | Out-Null
}

# Build output structure
$output = @{
    metadata = @{
        description = "Hoenn Wild Encounter Data - Extracted from Pokemon Crossroads"
        task = "Task 2.9: Convert Hoenn wild encounters"
        generated = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
        total_maps = $hoennEncounters.Count
        region = "Hoenn"
        region_id = 2
    }
    encounter_rates = $encounterGroup.fields
    encounters = $hoennEncounters
}

# Write to file
$output | ConvertTo-Json -Depth 10 | Set-Content $DestPath -Encoding UTF8

Write-Host "`nEncounter data written to: $DestPath" -ForegroundColor Green

# Count encounter types
$landCount = ($hoennEncounters | Where-Object { $_.land_mons }).Count
$waterCount = ($hoennEncounters | Where-Object { $_.water_mons }).Count
$fishingCount = ($hoennEncounters | Where-Object { $_.fishing_mons }).Count
$rockSmashCount = ($hoennEncounters | Where-Object { $_.rock_smash_mons }).Count

# Generate summary report
$report = @{
    TotalMapsRequested = $HoennMaps.Count
    MapsWithEncounters = $hoennEncounters.Count
    MapsMissing = $missingMaps.Count
    EncounterTypes = @{
        LandMons = $landCount
        WaterMons = $waterCount
        FishingMons = $fishingCount
        RockSmashMons = $rockSmashCount
    }
    MapsFound = $foundMaps
    MapsMissingList = $missingMaps
    Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
}

$reportPath = "src/data/hoenn_encounters_report.json"
$report | ConvertTo-Json -Depth 3 | Set-Content $reportPath
Write-Host "Report saved to: $reportPath" -ForegroundColor Gray

# Summary
Write-Host "`n=============================================================" -ForegroundColor Cyan
Write-Host "HOENN WILD ENCOUNTERS SUMMARY" -ForegroundColor Cyan
Write-Host "============================================================="
Write-Host "Maps with encounters: $($hoennEncounters.Count) / $($HoennMaps.Count)" -ForegroundColor $(if ($hoennEncounters.Count -gt 0) { "Green" } else { "Red" })
Write-Host "-------------------------------------------------------------"
Write-Host "Land encounters:      $landCount maps" -ForegroundColor Gray
Write-Host "Water encounters:     $waterCount maps" -ForegroundColor Gray
Write-Host "Fishing encounters:   $fishingCount maps" -ForegroundColor Gray
Write-Host "Rock Smash encounters: $rockSmashCount maps" -ForegroundColor Gray
Write-Host "============================================================="

if ($hoennEncounters.Count -gt 0) {
    Write-Host "`nHoenn wild encounters conversion completed!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nWARNING: No encounters were extracted!" -ForegroundColor Yellow
    exit 1
}
