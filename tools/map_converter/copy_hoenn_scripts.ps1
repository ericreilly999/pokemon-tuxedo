# Hoenn Event Scripts Converter
# Task 2.7: Convert Hoenn event scripts
# 
# This script copies Hoenn map scripts from Pokemon Crossroads to the FireRed project.
# The scripts are in .inc format which is compatible with pokefirered decomp.

param(
    [string]$SourcePath = "pokemon-crossroads/data/maps",
    [string]$DestPath = "data/maps/hoenn_scripts"
)

Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Hoenn Event Scripts Converter - Task 2.7" -ForegroundColor Cyan
Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Source: $SourcePath"
Write-Host "Destination: $DestPath"
Write-Host "============================================================="

# All Hoenn maps that need scripts
$HoennMaps = @(
    # Routes
    "Route101", "Route102", "Route103", "Route104", "Route105",
    "Route106", "Route107", "Route108", "Route109", "Route110",
    "Route111", "Route112", "Route113", "Route114", "Route115",
    "Route116", "Route117", "Route118", "Route119", "Route120",
    "Route121", "Route122", "Route123", "Route124", "Route125",
    "Route126", "Route127", "Route128", "Route129", "Route130",
    "Route131", "Route132", "Route133", "Route134",
    
    # Cities
    "LittlerootTown", "OldaleTown", "PetalburgCity", "RustboroCity",
    "DewfordTown", "SlateportCity", "MauvilleCity", "VerdanturfTown",
    "FallarborTown", "LavaridgeTown", "FortreeCity", "LilycoveCity",
    "MossdeepCity", "SootopolisCity", "PacifidlogTown", "EverGrandeCity",
    
    # Dungeons
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
    "VictoryRoad_1F", "VictoryRoad_B1F", "VictoryRoad_B2F"
)

# Create destination directory
if (-not (Test-Path $DestPath)) {
    New-Item -ItemType Directory -Path $DestPath -Force | Out-Null
    Write-Host "Created destination directory: $DestPath" -ForegroundColor Green
}

$copied = 0
$skipped = 0
$failed = 0
$scriptsFound = @()
$scriptsMissing = @()

foreach ($map in $HoennMaps) {
    $sourceScript = Join-Path $SourcePath "$map/scripts.inc"
    $destDir = Join-Path $DestPath $map
    $destScript = Join-Path $destDir "scripts.inc"
    
    if (Test-Path $sourceScript) {
        try {
            # Create map directory
            if (-not (Test-Path $destDir)) {
                New-Item -ItemType Directory -Path $destDir -Force | Out-Null
            }
            
            # Copy script file
            Copy-Item -Path $sourceScript -Destination $destScript -Force
            
            # Also copy any other .inc files in the map directory
            $otherIncs = Get-ChildItem -Path (Join-Path $SourcePath $map) -Filter "*.inc" -ErrorAction SilentlyContinue
            foreach ($inc in $otherIncs) {
                if ($inc.Name -ne "scripts.inc") {
                    Copy-Item -Path $inc.FullName -Destination (Join-Path $destDir $inc.Name) -Force
                }
            }
            
            Write-Host "  Copied: $map" -ForegroundColor Green
            $copied++
            $scriptsFound += $map
        }
        catch {
            Write-Host "  FAILED: $map - $_" -ForegroundColor Red
            $failed++
        }
    }
    else {
        Write-Host "  SKIPPED: $map (no scripts.inc)" -ForegroundColor Yellow
        $skipped++
        $scriptsMissing += $map
    }
}

# Summary
Write-Host "`n=============================================================" -ForegroundColor Cyan
Write-Host "SCRIPT CONVERSION SUMMARY" -ForegroundColor Cyan
Write-Host "============================================================="
Write-Host "Copied:  $copied" -ForegroundColor Green
Write-Host "Skipped: $skipped" -ForegroundColor Yellow
Write-Host "Failed:  $failed" -ForegroundColor Red
Write-Host "============================================================="

# Save report
$report = @{
    TotalMaps = $HoennMaps.Count
    ScriptsCopied = $copied
    ScriptsSkipped = $skipped
    ScriptsFailed = $failed
    MapsWithScripts = $scriptsFound
    MapsWithoutScripts = $scriptsMissing
    Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
}

$reportPath = Join-Path $DestPath "scripts_conversion_report.json"
$report | ConvertTo-Json -Depth 3 | Set-Content $reportPath
Write-Host "Report saved to: $reportPath"

if ($failed -eq 0) {
    Write-Host "`nScript conversion completed successfully!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nScript conversion completed with errors." -ForegroundColor Yellow
    exit 1
}
