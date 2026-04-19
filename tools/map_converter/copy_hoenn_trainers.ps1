# Hoenn Trainer Data Converter
# Task 2.8: Convert Hoenn trainer data
# 
# This script extracts Hoenn trainer data from Pokemon Crossroads trainers.party
# and copies it to the FireRed project in a compatible format.

param(
    [string]$SourcePath = "pokemon-crossroads/src/data/trainers.party",
    [string]$DestPath = "src/data/hoenn_trainers.party"
)

Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Hoenn Trainer Data Converter - Task 2.8" -ForegroundColor Cyan
Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Source: $SourcePath"
Write-Host "Destination: $DestPath"
Write-Host "============================================================="

# Hoenn Gym Leaders (all versions)
$HoennGymLeaders = @(
    "TRAINER_ROXANNE_1", "TRAINER_ROXANNE_2", "TRAINER_ROXANNE_3", "TRAINER_ROXANNE_4", "TRAINER_ROXANNE_5",
    "TRAINER_BRAWLY_1", "TRAINER_BRAWLY_2", "TRAINER_BRAWLY_3", "TRAINER_BRAWLY_4", "TRAINER_BRAWLY_5",
    "TRAINER_WATTSON_1", "TRAINER_WATTSON_2", "TRAINER_WATTSON_3", "TRAINER_WATTSON_4", "TRAINER_WATTSON_5",
    "TRAINER_FLANNERY_1", "TRAINER_FLANNERY_2", "TRAINER_FLANNERY_3", "TRAINER_FLANNERY_4", "TRAINER_FLANNERY_5",
    "TRAINER_NORMAN_1", "TRAINER_NORMAN_2", "TRAINER_NORMAN_3", "TRAINER_NORMAN_4", "TRAINER_NORMAN_5",
    "TRAINER_WINONA_1", "TRAINER_WINONA_2", "TRAINER_WINONA_3", "TRAINER_WINONA_4", "TRAINER_WINONA_5",
    "TRAINER_TATE_AND_LIZA_1", "TRAINER_TATE_AND_LIZA_2", "TRAINER_TATE_AND_LIZA_3", "TRAINER_TATE_AND_LIZA_4", "TRAINER_TATE_AND_LIZA_5",
    "TRAINER_JUAN_1", "TRAINER_JUAN_2", "TRAINER_JUAN_3", "TRAINER_JUAN_4", "TRAINER_JUAN_5"
)

# Hoenn Elite Four and Champion
$HoennEliteFour = @(
    "TRAINER_SIDNEY",
    "TRAINER_PHOEBE",
    "TRAINER_GLACIA",
    "TRAINER_DRAKE",
    "TRAINER_WALLACE"
)

# Hoenn Team Aqua/Magma trainers
$HoennTeamTrainers = @(
    "TRAINER_GRUNT_AQUA_HIDEOUT_1", "TRAINER_GRUNT_AQUA_HIDEOUT_2", "TRAINER_GRUNT_AQUA_HIDEOUT_3",
    "TRAINER_GRUNT_AQUA_HIDEOUT_4", "TRAINER_GRUNT_AQUA_HIDEOUT_5", "TRAINER_GRUNT_AQUA_HIDEOUT_6",
    "TRAINER_GRUNT_SEAFLOOR_CAVERN_1", "TRAINER_GRUNT_SEAFLOOR_CAVERN_2", "TRAINER_GRUNT_SEAFLOOR_CAVERN_3",
    "TRAINER_GRUNT_SEAFLOOR_CAVERN_4",
    "TRAINER_GRUNT_PETALBURG_WOODS",
    "TRAINER_GRUNT_RUSTURF_TUNNEL",
    "TRAINER_GRUNT_WEATHER_INST_1", "TRAINER_GRUNT_WEATHER_INST_2", "TRAINER_GRUNT_WEATHER_INST_3",
    "TRAINER_GRUNT_WEATHER_INST_4",
    "TRAINER_GRUNT_MUSEUM_1", "TRAINER_GRUNT_MUSEUM_2",
    "TRAINER_GRUNT_SPACE_CENTER_1",
    "TRAINER_GRUNT_MT_PYRE_1", "TRAINER_GRUNT_MT_PYRE_2", "TRAINER_GRUNT_MT_PYRE_3",
    "TRAINER_MATT",
    "TRAINER_SHELLY_WEATHER_INSTITUTE", "TRAINER_SHELLY_SEAFLOOR_CAVERN",
    "TRAINER_ARCHIE"
)

# Hoenn route/dungeon trainers (common Hoenn trainer classes)
$HoennRouteTrainers = @(
    # Hikers
    "TRAINER_SAWYER_1",
    # Pokemon Breeders
    "TRAINER_GABRIELLE_1",
    # Cooltrainers
    "TRAINER_MARCEL", "TRAINER_FELIX", "TRAINER_RANDALL", "TRAINER_PARKER", "TRAINER_GEORGE",
    "TRAINER_BERKE", "TRAINER_BRAXTON", "TRAINER_VINCENT", "TRAINER_LEROY",
    "TRAINER_WILTON_1", "TRAINER_WILTON_2", "TRAINER_WILTON_3", "TRAINER_WILTON_4", "TRAINER_WILTON_5",
    "TRAINER_EDGAR", "TRAINER_ALBERT", "TRAINER_SAMUEL", "TRAINER_VITO", "TRAINER_OWEN",
    # Bird Keepers
    "TRAINER_ALBERTO",
    # Collectors
    "TRAINER_ED",
    # Swimmers
    "TRAINER_DECLAN",
    # Experts
    "TRAINER_FREDRICK",
    # Black Belts
    "TRAINER_ZANDER",
    # Hex Maniacs
    "TRAINER_LEAH",
    # Aroma Ladies
    "TRAINER_DAISY", "TRAINER_VIOLET",
    "TRAINER_ROSE_1", "TRAINER_ROSE_2", "TRAINER_ROSE_3", "TRAINER_ROSE_4", "TRAINER_ROSE_5",
    # Ruin Maniacs
    "TRAINER_DUSTY_1", "TRAINER_DUSTY_2", "TRAINER_DUSTY_3", "TRAINER_DUSTY_4", "TRAINER_DUSTY_5",
    "TRAINER_CHIP", "TRAINER_FOSTER",
    # Interviewers
    "TRAINER_GABBY_AND_TY_1", "TRAINER_GABBY_AND_TY_2", "TRAINER_GABBY_AND_TY_3",
    "TRAINER_GABBY_AND_TY_4", "TRAINER_GABBY_AND_TY_5", "TRAINER_GABBY_AND_TY_6",
    # Tubers
    "TRAINER_LOLA_1", "TRAINER_LOLA_2", "TRAINER_LOLA_3", "TRAINER_LOLA_4", "TRAINER_LOLA_5",
    "TRAINER_AUSTINA", "TRAINER_GWEN",
    "TRAINER_RICKY_1", "TRAINER_RICKY_2", "TRAINER_RICKY_3", "TRAINER_RICKY_4", "TRAINER_RICKY_5",
    "TRAINER_SIMON", "TRAINER_CHARLIE"
)

# Combine all Hoenn trainers
$AllHoennTrainers = $HoennGymLeaders + $HoennEliteFour + $HoennTeamTrainers + $HoennRouteTrainers

Write-Host "`nSearching for $($AllHoennTrainers.Count) Hoenn trainers..." -ForegroundColor Yellow

# Check if source file exists
if (-not (Test-Path $SourcePath)) {
    Write-Host "ERROR: Source file not found: $SourcePath" -ForegroundColor Red
    exit 1
}

# Read the entire source file
$content = Get-Content $SourcePath -Raw

# Parse trainers from the file
# Format: === TRAINER_XXX === followed by trainer data until next === or EOF
$trainerPattern = '(?s)=== (TRAINER_[A-Z0-9_]+) ===\r?\n(.*?)(?===|\z)'
$matches = [regex]::Matches($content, $trainerPattern)

Write-Host "Found $($matches.Count) total trainers in source file" -ForegroundColor Gray

# Extract Hoenn trainers
$extractedTrainers = @{}
$foundCount = 0
$missingTrainers = @()

foreach ($trainer in $AllHoennTrainers) {
    $found = $false
    foreach ($match in $matches) {
        if ($match.Groups[1].Value -eq $trainer) {
            $extractedTrainers[$trainer] = $match.Value
            $found = $true
            $foundCount++
            break
        }
    }
    if (-not $found) {
        $missingTrainers += $trainer
    }
}

Write-Host "Extracted $foundCount Hoenn trainers" -ForegroundColor Green

if ($missingTrainers.Count -gt 0) {
    Write-Host "`nMissing trainers ($($missingTrainers.Count)):" -ForegroundColor Yellow
    foreach ($missing in $missingTrainers) {
        Write-Host "  - $missing" -ForegroundColor Yellow
    }
}

# Create destination directory if needed
$destDir = Split-Path $DestPath -Parent
if (-not (Test-Path $destDir)) {
    New-Item -ItemType Directory -Path $destDir -Force | Out-Null
}

# Write extracted trainers to destination file
$header = @"
/*
Hoenn Trainer Data - Extracted from Pokemon Crossroads
Task 2.8: Convert Hoenn trainer data

This file contains all Hoenn region trainers:
- 8 Gym Leaders (with rematch versions)
- 4 Elite Four members
- 1 Champion (Wallace)
- Team Aqua/Magma trainers
- Route and dungeon trainers

Format: Pokemon Showdown Competitive Syntax
Compatible with pokefirered decomp trainerproc tool

Generated: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Total Trainers: $foundCount
*/

"@

$output = $header

# Write trainers in order: Gym Leaders, Elite Four, Team, Route
Write-Host "`nWriting trainers to $DestPath..." -ForegroundColor Yellow

# Gym Leaders
$output += "/* ========== HOENN GYM LEADERS ========== */`n`n"
foreach ($trainer in $HoennGymLeaders) {
    if ($extractedTrainers.ContainsKey($trainer)) {
        $output += $extractedTrainers[$trainer] + "`n"
    }
}

# Elite Four and Champion
$output += "`n/* ========== HOENN ELITE FOUR & CHAMPION ========== */`n`n"
foreach ($trainer in $HoennEliteFour) {
    if ($extractedTrainers.ContainsKey($trainer)) {
        $output += $extractedTrainers[$trainer] + "`n"
    }
}

# Team Aqua/Magma
$output += "`n/* ========== TEAM AQUA/MAGMA ========== */`n`n"
foreach ($trainer in $HoennTeamTrainers) {
    if ($extractedTrainers.ContainsKey($trainer)) {
        $output += $extractedTrainers[$trainer] + "`n"
    }
}

# Route trainers
$output += "`n/* ========== HOENN ROUTE & DUNGEON TRAINERS ========== */`n`n"
foreach ($trainer in $HoennRouteTrainers) {
    if ($extractedTrainers.ContainsKey($trainer)) {
        $output += $extractedTrainers[$trainer] + "`n"
    }
}

# Write to file
$output | Set-Content $DestPath -Encoding UTF8

Write-Host "Trainer data written to: $DestPath" -ForegroundColor Green

# Generate summary report
$report = @{
    TotalTrainersRequested = $AllHoennTrainers.Count
    TrainersExtracted = $foundCount
    TrainersMissing = $missingTrainers.Count
    GymLeadersExtracted = ($HoennGymLeaders | Where-Object { $extractedTrainers.ContainsKey($_) }).Count
    EliteFourExtracted = ($HoennEliteFour | Where-Object { $extractedTrainers.ContainsKey($_) }).Count
    TeamTrainersExtracted = ($HoennTeamTrainers | Where-Object { $extractedTrainers.ContainsKey($_) }).Count
    RouteTrainersExtracted = ($HoennRouteTrainers | Where-Object { $extractedTrainers.ContainsKey($_) }).Count
    MissingTrainers = $missingTrainers
    Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
}

$reportPath = "src/data/hoenn_trainers_report.json"
$report | ConvertTo-Json -Depth 3 | Set-Content $reportPath
Write-Host "Report saved to: $reportPath" -ForegroundColor Gray

# Summary
Write-Host "`n=============================================================" -ForegroundColor Cyan
Write-Host "HOENN TRAINER CONVERSION SUMMARY" -ForegroundColor Cyan
Write-Host "============================================================="
Write-Host "Gym Leaders:     $($report.GymLeadersExtracted) / $($HoennGymLeaders.Count)" -ForegroundColor $(if ($report.GymLeadersExtracted -eq $HoennGymLeaders.Count) { "Green" } else { "Yellow" })
Write-Host "Elite Four:      $($report.EliteFourExtracted) / $($HoennEliteFour.Count)" -ForegroundColor $(if ($report.EliteFourExtracted -eq $HoennEliteFour.Count) { "Green" } else { "Yellow" })
Write-Host "Team Trainers:   $($report.TeamTrainersExtracted) / $($HoennTeamTrainers.Count)" -ForegroundColor $(if ($report.TeamTrainersExtracted -eq $HoennTeamTrainers.Count) { "Green" } else { "Yellow" })
Write-Host "Route Trainers:  $($report.RouteTrainersExtracted) / $($HoennRouteTrainers.Count)" -ForegroundColor $(if ($report.RouteTrainersExtracted -eq $HoennRouteTrainers.Count) { "Green" } else { "Yellow" })
Write-Host "-------------------------------------------------------------"
Write-Host "TOTAL:           $foundCount / $($AllHoennTrainers.Count)" -ForegroundColor $(if ($foundCount -eq $AllHoennTrainers.Count) { "Green" } else { "Yellow" })
Write-Host "============================================================="

if ($foundCount -gt 0) {
    Write-Host "`nHoenn trainer conversion completed!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`nERROR: No trainers were extracted!" -ForegroundColor Red
    exit 1
}
