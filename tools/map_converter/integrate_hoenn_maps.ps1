# Hoenn Map Build System Integration
# Task 2.10: Integrate Hoenn maps into build system
# 
# This script integrates the converted Hoenn maps into the pokefirered build system.
# It updates map_groups.json and creates the necessary directory structure.

param(
    [string]$ConvertedMapsPath = "data/maps/hoenn_converted",
    [string]$MapGroupsPath = "data/maps/map_groups.json",
    [string]$LayoutsPath = "data/layouts/layouts.json"
)

Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Hoenn Map Build System Integration - Task 2.10" -ForegroundColor Cyan
Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Converted Maps: $ConvertedMapsPath"
Write-Host "Map Groups: $MapGroupsPath"
Write-Host "============================================================="

# Hoenn map groups structure
$HoennMapGroups = @{
    # Hoenn Towns and Routes
    "gMapGroup_HoennTownsAndRoutes" = @(
        "LittlerootTown", "OldaleTown", "PetalburgCity", "RustboroCity",
        "DewfordTown", "SlateportCity", "MauvilleCity", "VerdanturfTown",
        "FallarborTown", "LavaridgeTown", "FortreeCity", "LilycoveCity",
        "MossdeepCity", "SootopolisCity", "PacifidlogTown", "EverGrandeCity",
        "Route101", "Route102", "Route103", "Route104", "Route105",
        "Route106", "Route107", "Route108", "Route109", "Route110",
        "Route111", "Route112", "Route113", "Route114", "Route115",
        "Route116", "Route117", "Route118", "Route119", "Route120",
        "Route121", "Route122", "Route123", "Route124", "Route125",
        "Route126", "Route127", "Route128", "Route129", "Route130",
        "Route131", "Route132", "Route133", "Route134"
    )
    
    # Hoenn Dungeons
    "gMapGroup_HoennDungeons" = @(
        "PetalburgWoods",
        "GraniteCave_1F", "GraniteCave_B1F", "GraniteCave_B2F", "GraniteCave_StevensRoom",
        "RusturfTunnel",
        "MeteorFalls_1F_1R", "MeteorFalls_1F_2R", "MeteorFalls_B1F_1R", 
        "MeteorFalls_B1F_2R", "MeteorFalls_StevensCave",
        "MtChimney", "MtChimney_CableCarStation",
        "JaggedPass", "FieryPath",
        "MtPyre_1F", "MtPyre_2F", "MtPyre_3F", "MtPyre_4F", 
        "MtPyre_5F", "MtPyre_6F", "MtPyre_Exterior", "MtPyre_Summit",
        "ShoalCave_HighTideEntranceRoom", "ShoalCave_HighTideInnerRoom",
        "ShoalCave_LowTideEntranceRoom", "ShoalCave_LowTideIceRoom",
        "ShoalCave_LowTideInnerRoom", "ShoalCave_LowTideLowerRoom", 
        "ShoalCave_LowTideStairsRoom",
        "CaveOfOrigin_Entrance", "CaveOfOrigin_1F", "CaveOfOrigin_B1F",
        "SeafloorCavern_Entrance", "SeafloorCavern_Room1", "SeafloorCavern_Room2",
        "SeafloorCavern_Room3", "SeafloorCavern_Room4", "SeafloorCavern_Room5",
        "SeafloorCavern_Room6", "SeafloorCavern_Room7", "SeafloorCavern_Room8",
        "SeafloorCavern_Room9",
        "SkyPillar_Entrance", "SkyPillar_Outside", "SkyPillar_1F", "SkyPillar_2F",
        "SkyPillar_3F", "SkyPillar_4F", "SkyPillar_5F", "SkyPillar_Top",
        "VictoryRoad_Hoenn_1F", "VictoryRoad_Hoenn_B1F", "VictoryRoad_Hoenn_B2F"
    )
}

# Check if converted maps exist
if (-not (Test-Path $ConvertedMapsPath)) {
    Write-Host "ERROR: Converted maps directory not found: $ConvertedMapsPath" -ForegroundColor Red
    exit 1
}

# Count converted maps
$convertedMaps = Get-ChildItem -Path $ConvertedMapsPath -Filter "*.json" -ErrorAction SilentlyContinue
Write-Host "Found $($convertedMaps.Count) converted Hoenn maps" -ForegroundColor Green

# Read existing map_groups.json
if (-not (Test-Path $MapGroupsPath)) {
    Write-Host "ERROR: Map groups file not found: $MapGroupsPath" -ForegroundColor Red
    exit 1
}

$mapGroups = Get-Content $MapGroupsPath -Raw | ConvertFrom-Json

# Check if Hoenn groups already exist
$hoennGroupsExist = $mapGroups.group_order -contains "gMapGroup_HoennTownsAndRoutes"

if ($hoennGroupsExist) {
    Write-Host "Hoenn map groups already exist in map_groups.json" -ForegroundColor Yellow
} else {
    Write-Host "Adding Hoenn map groups to map_groups.json..." -ForegroundColor Yellow
    
    # Add Hoenn groups to group_order
    $newGroupOrder = @($mapGroups.group_order) + @("gMapGroup_HoennTownsAndRoutes", "gMapGroup_HoennDungeons")
    
    # Create updated map groups object
    $updatedMapGroups = [ordered]@{
        group_order = $newGroupOrder
    }
    
    # Copy existing groups
    foreach ($prop in $mapGroups.PSObject.Properties) {
        if ($prop.Name -ne "group_order" -and $prop.Name -ne "connections_include_order") {
            $updatedMapGroups[$prop.Name] = $prop.Value
        }
    }
    
    # Add Hoenn groups
    $updatedMapGroups["gMapGroup_HoennTownsAndRoutes"] = $HoennMapGroups["gMapGroup_HoennTownsAndRoutes"]
    $updatedMapGroups["gMapGroup_HoennDungeons"] = $HoennMapGroups["gMapGroup_HoennDungeons"]
    
    # Add connections_include_order at the end
    if ($mapGroups.connections_include_order) {
        $hoennConnections = $HoennMapGroups["gMapGroup_HoennTownsAndRoutes"]
        $updatedMapGroups["connections_include_order"] = @($mapGroups.connections_include_order) + $hoennConnections
    }
    
    # Write updated map_groups.json
    $updatedMapGroups | ConvertTo-Json -Depth 10 | Set-Content $MapGroupsPath -Encoding UTF8
    Write-Host "Updated map_groups.json with Hoenn groups" -ForegroundColor Green
}

# Create map directories for Hoenn maps
$mapsDir = "data/maps"
$createdDirs = 0
$skippedDirs = 0

$allHoennMaps = $HoennMapGroups["gMapGroup_HoennTownsAndRoutes"] + $HoennMapGroups["gMapGroup_HoennDungeons"]

foreach ($mapName in $allHoennMaps) {
    $mapDir = Join-Path $mapsDir $mapName
    
    if (-not (Test-Path $mapDir)) {
        New-Item -ItemType Directory -Path $mapDir -Force | Out-Null
        $createdDirs++
        
        # Create a placeholder map.json if converted map exists
        $convertedMapFile = Join-Path $ConvertedMapsPath "$mapName.json"
        if (Test-Path $convertedMapFile) {
            # Copy converted map as map.json
            Copy-Item -Path $convertedMapFile -Destination (Join-Path $mapDir "map.json") -Force
        } else {
            # Create minimal placeholder
            $placeholder = @{
                id = "MAP_$($mapName.ToUpper())"
                name = $mapName
                layout = "LAYOUT_$($mapName.ToUpper())"
                music = "MUS_HOENN_ROUTE"
                region_map_section = "MAPSEC_$($mapName.ToUpper())"
                requires_flash = $false
                weather = "WEATHER_NONE"
                map_type = "MAP_TYPE_ROUTE"
                allow_cycling = $true
                allow_escaping = $true
                allow_running = $true
                show_map_name = $true
                battle_scene = "MAP_BATTLE_SCENE_NORMAL"
                connections = $null
                object_events = @()
                warp_events = @()
                coord_events = @()
                bg_events = @()
                region_id = 2
            }
            $placeholder | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $mapDir "map.json") -Encoding UTF8
        }
    } else {
        $skippedDirs++
    }
}

Write-Host "Created $createdDirs map directories" -ForegroundColor Green
Write-Host "Skipped $skippedDirs existing directories" -ForegroundColor Gray

# Generate summary report
$report = @{
    TotalHoennMaps = $allHoennMaps.Count
    ConvertedMapsFound = $convertedMaps.Count
    DirectoriesCreated = $createdDirs
    DirectoriesSkipped = $skippedDirs
    HoennGroupsAdded = -not $hoennGroupsExist
    MapGroups = @{
        TownsAndRoutes = $HoennMapGroups["gMapGroup_HoennTownsAndRoutes"].Count
        Dungeons = $HoennMapGroups["gMapGroup_HoennDungeons"].Count
    }
    Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
}

$reportPath = "data/maps/hoenn_integration_report.json"
$report | ConvertTo-Json -Depth 3 | Set-Content $reportPath
Write-Host "Report saved to: $reportPath" -ForegroundColor Gray

# Summary
Write-Host "`n=============================================================" -ForegroundColor Cyan
Write-Host "HOENN BUILD INTEGRATION SUMMARY" -ForegroundColor Cyan
Write-Host "============================================================="
Write-Host "Total Hoenn maps:     $($allHoennMaps.Count)" -ForegroundColor Gray
Write-Host "Towns & Routes:       $($HoennMapGroups['gMapGroup_HoennTownsAndRoutes'].Count)" -ForegroundColor Gray
Write-Host "Dungeons:             $($HoennMapGroups['gMapGroup_HoennDungeons'].Count)" -ForegroundColor Gray
Write-Host "-------------------------------------------------------------"
Write-Host "Directories created:  $createdDirs" -ForegroundColor Green
Write-Host "Directories skipped:  $skippedDirs" -ForegroundColor Yellow
Write-Host "============================================================="

Write-Host "`nHoenn build integration completed!" -ForegroundColor Green
Write-Host "Note: You may need to run 'make clean-generated' before rebuilding." -ForegroundColor Yellow
exit 0
