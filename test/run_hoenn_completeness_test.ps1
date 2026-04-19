# Property Test Runner: Hoenn Map Completeness (Property 4)
# Validates: Requirements 3.1, 3.2, 3.3
#
# This script validates that all required Hoenn maps are present and correctly formatted.

param(
    [string]$MapPath = "..\data\maps\hoenn_converted"
)

Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Property Test: Hoenn Map Completeness (Property 4)" -ForegroundColor Cyan
Write-Host "=============================================================" -ForegroundColor Cyan
Write-Host "Validates: Requirements 3.1, 3.2, 3.3"
Write-Host "Map directory: $MapPath"
Write-Host "============================================================="

# Expected Hoenn maps
$HoennRoutes = @(
    "Route101", "Route102", "Route103", "Route104", "Route105",
    "Route106", "Route107", "Route108", "Route109", "Route110",
    "Route111", "Route112", "Route113", "Route114", "Route115",
    "Route116", "Route117", "Route118", "Route119", "Route120",
    "Route121", "Route122", "Route123", "Route124", "Route125",
    "Route126", "Route127", "Route128", "Route129", "Route130",
    "Route131", "Route132", "Route133", "Route134"
)

$HoennCities = @(
    "LittlerootTown", "OldaleTown", "PetalburgCity", "RustboroCity",
    "DewfordTown", "SlateportCity", "MauvilleCity", "VerdanturfTown",
    "FallarborTown", "LavaridgeTown", "FortreeCity", "LilycoveCity",
    "MossdeepCity", "SootopolisCity", "PacifidlogTown", "EverGrandeCity"
)

$HoennDungeons = @(
    "PetalburgWoods",
    "GraniteCave_1F", "GraniteCave_B1F", "GraniteCave_B2F", "GraniteCave_StevensRoom",
    "RusturfTunnel",
    "MeteorFalls_1F_1R", "MeteorFalls_1F_2R", "MeteorFalls_B1F_1R", 
    "MeteorFalls_B1F_2R", "MeteorFalls_StevensCave",
    "MtChimney", "MtChimney_CableCarStation",
    "JaggedPass",
    "FieryPath",
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

$HOENN_REGION_ID = 2
$failedMaps = @()
$testResults = @{
    Routes = @{ Found = 0; Valid = 0; Expected = $HoennRoutes.Count }
    Cities = @{ Found = 0; Valid = 0; Expected = $HoennCities.Count }
    Dungeons = @{ Found = 0; Valid = 0; Expected = $HoennDungeons.Count }
}

function Test-MapFile {
    param([string]$MapName, [string]$BasePath)
    
    $filePath = Join-Path $BasePath "$MapName.json"
    
    if (-not (Test-Path $filePath)) {
        return @{ Exists = $false; Valid = $false; Error = "File not found" }
    }
    
    try {
        $content = Get-Content $filePath -Raw | ConvertFrom-Json
        
        # Check required fields
        $errors = @()
        
        if (-not $content.name) { $errors += "Missing 'name' field" }
        if (-not $content.layout) { $errors += "Missing 'layout' field" }
        if (-not $content.music) { $errors += "Missing 'music' field" }
        
        if ($null -eq $content.region_id) {
            $errors += "Missing 'region_id' field"
        } elseif ($content.region_id -ne $HOENN_REGION_ID) {
            $errors += "region_id=$($content.region_id), expected $HOENN_REGION_ID (Hoenn)"
        }
        
        if ($errors.Count -gt 0) {
            return @{ Exists = $true; Valid = $false; Error = ($errors -join "; ") }
        }
        
        return @{ Exists = $true; Valid = $true; Error = $null }
    }
    catch {
        return @{ Exists = $true; Valid = $false; Error = "JSON parse error: $_" }
    }
}

function Test-MapCategory {
    param([string]$CategoryName, [string[]]$Maps, [string]$BasePath)
    
    Write-Host "`n--- Testing $CategoryName ($($Maps.Count) expected) ---" -ForegroundColor Yellow
    
    $found = 0
    $valid = 0
    $failed = @()
    
    foreach ($map in $Maps) {
        $result = Test-MapFile -MapName $map -BasePath $BasePath
        
        if ($result.Exists) {
            $found++
            if ($result.Valid) {
                $valid++
            } else {
                Write-Host "  FAIL: $map - $($result.Error)" -ForegroundColor Red
                $failed += $map
            }
        } else {
            Write-Host "  MISSING: $map" -ForegroundColor Red
            $failed += $map
        }
    }
    
    Write-Host "$CategoryName found: $found/$($Maps.Count)"
    Write-Host "$CategoryName valid: $valid/$($Maps.Count)"
    
    return @{
        Found = $found
        Valid = $valid
        Expected = $Maps.Count
        Failed = $failed
        Pass = ($found -eq $Maps.Count -and $valid -eq $Maps.Count)
    }
}

# Check directory exists
if (-not (Test-Path $MapPath)) {
    Write-Host "`nFATAL: Map directory does not exist: $MapPath" -ForegroundColor Red
    Write-Host "Please run the Hoenn map conversion first."
    exit 1
}

# Run tests
$routeResults = Test-MapCategory -CategoryName "Routes" -Maps $HoennRoutes -BasePath $MapPath
$cityResults = Test-MapCategory -CategoryName "Cities" -Maps $HoennCities -BasePath $MapPath
$dungeonResults = Test-MapCategory -CategoryName "Dungeons" -Maps $HoennDungeons -BasePath $MapPath

# Count total maps
Write-Host "`n--- Testing Total Map Count ---" -ForegroundColor Yellow
$allJsonFiles = Get-ChildItem -Path $MapPath -Filter "*.json" | Where-Object { $_.Name -notmatch "report" }
$totalMaps = $allJsonFiles.Count
$expectedTotal = $HoennRoutes.Count + $HoennCities.Count + $HoennDungeons.Count

Write-Host "Total map files found: $totalMaps"
Write-Host "Expected map files: $expectedTotal"

$countPass = $totalMaps -ge $expectedTotal
if ($countPass) {
    Write-Host "  PASS: Map count meets or exceeds expected" -ForegroundColor Green
} else {
    Write-Host "  FAIL: Map count below expected ($totalMaps < $expectedTotal)" -ForegroundColor Red
}

# Summary
Write-Host "`n=============================================================" -ForegroundColor Cyan
Write-Host "TEST SUMMARY" -ForegroundColor Cyan
Write-Host "============================================================="

$routeStatus = if ($routeResults.Pass) { "PASS" } else { "FAIL" }
$cityStatus = if ($cityResults.Pass) { "PASS" } else { "FAIL" }
$dungeonStatus = if ($dungeonResults.Pass) { "PASS" } else { "FAIL" }
$countStatus = if ($countPass) { "PASS" } else { "FAIL" }

$routeColor = if ($routeResults.Pass) { "Green" } else { "Red" }
$cityColor = if ($cityResults.Pass) { "Green" } else { "Red" }
$dungeonColor = if ($dungeonResults.Pass) { "Green" } else { "Red" }
$countColor = if ($countPass) { "Green" } else { "Red" }

Write-Host "Routes (34):   " -NoNewline; Write-Host $routeStatus -ForegroundColor $routeColor
Write-Host "Cities (16):   " -NoNewline; Write-Host $cityStatus -ForegroundColor $cityColor
Write-Host "Dungeons (54): " -NoNewline; Write-Host $dungeonStatus -ForegroundColor $dungeonColor
Write-Host "Total Count:   " -NoNewline; Write-Host $countStatus -ForegroundColor $countColor

# Collect all failed maps
$allFailed = @()
$allFailed += $routeResults.Failed
$allFailed += $cityResults.Failed
$allFailed += $dungeonResults.Failed

if ($allFailed.Count -gt 0) {
    Write-Host "`nFailed/Missing Maps ($($allFailed.Count)):" -ForegroundColor Yellow
    $allFailed | Select-Object -First 20 | ForEach-Object { Write-Host "  - $_" }
    if ($allFailed.Count -gt 20) {
        Write-Host "  ... and $($allFailed.Count - 20) more"
    }
}

Write-Host "`n=============================================================" -ForegroundColor Cyan

$allPass = $routeResults.Pass -and $cityResults.Pass -and $dungeonResults.Pass -and $countPass

if ($allPass) {
    Write-Host "RESULT: ALL PROPERTY TESTS PASSED" -ForegroundColor Green
    Write-Host "Property 4 (Region Map Completeness) VERIFIED for Hoenn" -ForegroundColor Green
    Write-Host "============================================================="
    exit 0
} else {
    Write-Host "RESULT: SOME PROPERTY TESTS FAILED" -ForegroundColor Red
    Write-Host "Property 4 (Region Map Completeness) NOT VERIFIED for Hoenn" -ForegroundColor Red
    Write-Host "============================================================="
    exit 1
}
