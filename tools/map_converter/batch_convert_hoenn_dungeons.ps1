# Batch converter for all Hoenn caves and dungeons from Pokemon Crossroads to FireRed format.
# Converts 13+ dungeon locations with multiple floors.

$ErrorActionPreference = "Stop"

# List of all Hoenn caves and dungeons (including all floors)
$HOENN_DUNGEONS = @(
    # Petalburg Woods
    "PetalburgWoods",
    
    # Granite Cave
    "GraniteCave_1F",
    "GraniteCave_B1F",
    "GraniteCave_B2F",
    "GraniteCave_StevensRoom",
    
    # Rusturf Tunnel
    "RusturfTunnel",
    
    # Meteor Falls
    "MeteorFalls_1F_1R",
    "MeteorFalls_1F_2R",
    "MeteorFalls_B1F_1R",
    "MeteorFalls_B1F_2R",
    "MeteorFalls_StevensCave",
    
    # Mt. Chimney
    "MtChimney",
    "MtChimney_CableCarStation",
    
    # Jagged Pass
    "JaggedPass",
    
    # Fiery Path
    "FieryPath",
    
    # Mt. Pyre
    "MtPyre_1F",
    "MtPyre_2F",
    "MtPyre_3F",
    "MtPyre_4F",
    "MtPyre_5F",
    "MtPyre_6F",
    "MtPyre_Exterior",
    "MtPyre_Summit",
    
    # Shoal Cave
    "ShoalCave_HighTideEntranceRoom",
    "ShoalCave_HighTideInnerRoom",
    "ShoalCave_LowTideEntranceRoom",
    "ShoalCave_LowTideIceRoom",
    "ShoalCave_LowTideInnerRoom",
    "ShoalCave_LowTideLowerRoom",
    "ShoalCave_LowTideStairsRoom",
    
    # Cave of Origin
    "CaveOfOrigin_Entrance",
    "CaveOfOrigin_1F",
    "CaveOfOrigin_B1F",
    
    # Seafloor Cavern
    "SeafloorCavern_Entrance",
    "SeafloorCavern_Room1",
    "SeafloorCavern_Room2",
    "SeafloorCavern_Room3",
    "SeafloorCavern_Room4",
    "SeafloorCavern_Room5",
    "SeafloorCavern_Room6",
    "SeafloorCavern_Room7",
    "SeafloorCavern_Room8",
    "SeafloorCavern_Room9",
    
    # Sky Pillar
    "SkyPillar_Entrance",
    "SkyPillar_Outside",
    "SkyPillar_1F",
    "SkyPillar_2F",
    "SkyPillar_3F",
    "SkyPillar_4F",
    "SkyPillar_5F",
    "SkyPillar_Top",
    
    # Victory Road (Hoenn version)
    "VictoryRoad_1F",
    "VictoryRoad_B1F",
    "VictoryRoad_B2F"
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
            map_type = if ($emeraldMap.map_type) { $emeraldMap.map_type } else { "MAP_TYPE_UNDERGROUND" }
            allow_cycling = if ($null -ne $emeraldMap.allow_cycling) { $emeraldMap.allow_cycling } else { $false }
            allow_escaping = if ($null -ne $emeraldMap.allow_escaping) { $emeraldMap.allow_escaping } else { $true }
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
Write-Host "HOENN CAVES AND DUNGEONS CONVERSION REPORT"
Write-Host ("=" * 70)
Write-Host "Input directory: $baseInputDir"
Write-Host "Output directory: $baseOutputDir"
Write-Host ""

foreach ($dungeon in $HOENN_DUNGEONS) {
    $inputPath = Join-Path $baseInputDir "$dungeon\map.json"
    $outputPath = Join-Path $baseOutputDir "$dungeon.json"
    
    Write-Host "Converting $dungeon... " -NoNewline
    
    if (-not (Test-Path $inputPath)) {
        Write-Host "SKIP (source not found)"
        $conversionReport += @{
            dungeon = $dungeon
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
            dungeon = $dungeon
            status = "SUCCESS"
            input = $inputPath
            output = $outputPath
        }
        $successCount++
    }
    else {
        Write-Host "FAILED: $($result.Error)"
        $conversionReport += @{
            dungeon = $dungeon
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
Write-Host "Total dungeon maps: $($HOENN_DUNGEONS.Count)"
Write-Host "Successfully converted: $successCount"
Write-Host "Failed: $failCount"
Write-Host "Skipped: $skipCount"
Write-Host ""

# Write detailed report
$reportPath = Join-Path $baseOutputDir "dungeons_conversion_report.json"
@{
    region = "Hoenn"
    type = "dungeons"
    total = $HOENN_DUNGEONS.Count
    success = $successCount
    failed = $failCount
    skipped = $skipCount
    details = $conversionReport
} | ConvertTo-Json -Depth 10 | Set-Content -Path $reportPath -Encoding UTF8

Write-Host "Detailed report written to: $reportPath"
Write-Host ""

# Return success if all dungeons converted
if ($successCount -eq $HOENN_DUNGEONS.Count) {
    Write-Host "All Hoenn caves and dungeons converted successfully!" -ForegroundColor Green
    exit 0
}
elseif ($successCount -gt 0) {
    Write-Host "Partial success: $successCount/$($HOENN_DUNGEONS.Count) dungeon maps converted" -ForegroundColor Yellow
    exit 1
}
else {
    Write-Host "No dungeon maps were converted" -ForegroundColor Red
    exit 2
}
