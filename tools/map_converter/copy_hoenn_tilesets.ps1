# Hoenn Tileset Converter
# Task 2.6: Convert Hoenn tilesets
# 
# This script copies Hoenn-specific tilesets from Pokemon Crossroads to the FireRed project.
# The tilesets are already in a compatible format (same structure as FireRed tilesets).

param(
    [string]$SourcePath = "pokemon-crossroads/data/tilesets",
    [string]$DestPath = "data/tilesets/secondary/hoenn"
)

Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Hoenn Tileset Converter - Task 2.6" -ForegroundColor Cyan
Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Source: $SourcePath"
Write-Host "Destination: $DestPath"
Write-Host "============================================================="

# Hoenn-specific tilesets (Emerald format, not FRLG)
$HoennTilesets = @(
    # Cities and Towns
    "dewford",
    "dewford_gym",
    "ever_grande",
    "elite_four",
    "fallarbor",
    "fortree",
    "fortree_gym",
    "lavaridge",
    "lavaridge_gym",
    "lilycove",
    "lilycove_museum",
    "mauville",
    "mauville_game_corner",
    "mauville_gym",
    "mossdeep",
    "mossdeep_game_corner",
    "mossdeep_gym",
    "pacifidlog",
    "petalburg",
    "petalburg_gym",
    "rustboro",
    "rustboro_gym",
    "slateport",
    "sootopolis",
    "sootopolis_gym",
    
    # Caves and Dungeons
    "cave",
    "meteor_falls",
    "mirage_tower",
    "rusturf_tunnel",
    "underwater",
    
    # Buildings and Interiors
    "brendans_mays_house",
    "contest",
    "facility",
    "generic_building",
    "inside_of_truck",
    "inside_ship",
    "island_harbor",
    "lab",
    "mystery_events_house",
    "oceanic_museum",
    "pokemon_center",
    "pokemon_day_care",
    "pokemon_fan_club",
    "pokemon_school",
    "pretty_petal_flower_shop",
    "seashore_house",
    "secret_base",
    "shop",
    "trick_house_puzzle",
    "union_room",
    
    # Battle Frontier (optional but included for completeness)
    "battle_arena",
    "battle_dome",
    "battle_factory",
    "battle_frontier",
    "battle_frontier_outside_east",
    "battle_frontier_outside_west",
    "battle_frontier_ranking_hall",
    "battle_palace",
    "battle_pike",
    "battle_pyramid",
    "battle_tent",
    "trainer_hill"
)

# Create destination directory
if (-not (Test-Path $DestPath)) {
    New-Item -ItemType Directory -Path $DestPath -Force | Out-Null
    Write-Host "Created destination directory: $DestPath" -ForegroundColor Green
}

$copied = 0
$skipped = 0
$failed = 0
$report = @()

foreach ($tileset in $HoennTilesets) {
    $sourceTileset = Join-Path $SourcePath "secondary/$tileset"
    $destTileset = Join-Path $DestPath $tileset
    
    if (Test-Path $sourceTileset) {
        try {
            # Copy the entire tileset directory
            if (Test-Path $destTileset) {
                Remove-Item -Path $destTileset -Recurse -Force
            }
            Copy-Item -Path $sourceTileset -Destination $destTileset -Recurse -Force
            
            Write-Host "  Copied: $tileset" -ForegroundColor Green
            $copied++
            $report += @{
                Name = $tileset
                Status = "Copied"
                Source = $sourceTileset
                Destination = $destTileset
            }
        }
        catch {
            Write-Host "  FAILED: $tileset - $_" -ForegroundColor Red
            $failed++
            $report += @{
                Name = $tileset
                Status = "Failed"
                Error = $_.ToString()
            }
        }
    }
    else {
        Write-Host "  SKIPPED: $tileset (not found in source)" -ForegroundColor Yellow
        $skipped++
        $report += @{
            Name = $tileset
            Status = "Skipped"
            Reason = "Not found in source"
        }
    }
}

# Also copy primary tilesets if needed
Write-Host "`nChecking primary tilesets..." -ForegroundColor Yellow
$primaryTilesets = @("general", "building")
$primaryDestPath = "data/tilesets/primary/hoenn"

if (-not (Test-Path $primaryDestPath)) {
    New-Item -ItemType Directory -Path $primaryDestPath -Force | Out-Null
}

foreach ($tileset in $primaryTilesets) {
    $sourceTileset = Join-Path $SourcePath "primary/$tileset"
    $destTileset = Join-Path $primaryDestPath $tileset
    
    if (Test-Path $sourceTileset) {
        try {
            if (Test-Path $destTileset) {
                Remove-Item -Path $destTileset -Recurse -Force
            }
            Copy-Item -Path $sourceTileset -Destination $destTileset -Recurse -Force
            Write-Host "  Copied primary: $tileset" -ForegroundColor Green
            $copied++
        }
        catch {
            Write-Host "  FAILED primary: $tileset - $_" -ForegroundColor Red
            $failed++
        }
    }
}

# Summary
Write-Host "`n=============================================================" -ForegroundColor Cyan
Write-Host "TILESET CONVERSION SUMMARY" -ForegroundColor Cyan
Write-Host "============================================================="
Write-Host "Copied:  $copied" -ForegroundColor Green
Write-Host "Skipped: $skipped" -ForegroundColor Yellow
Write-Host "Failed:  $failed" -ForegroundColor Red
Write-Host "============================================================="

# Save report
$reportPath = Join-Path $DestPath "tileset_conversion_report.json"
$report | ConvertTo-Json -Depth 3 | Set-Content $reportPath
Write-Host "Report saved to: $reportPath"

if ($failed -eq 0) {
    Write-Host "`nTileset conversion completed successfully!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nTileset conversion completed with errors." -ForegroundColor Yellow
    exit 1
}
