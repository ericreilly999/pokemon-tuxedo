# TODO — Pokemon Tuxedo

Legend: [x] done · [ ] todo · [!] blocked · [~] deferred

---

## IMMEDIATE: Unblock the Build

- [x] DEV-001 — Fix compilation error in `src/system_integration.c:123`
  - Error: `too many arguments to function 'SetEliteFourDefeated'`
  - Depends On: —
  - Owner: Application Engineer

- [x] DEV-002 — Verify clean ROM build after fix
  - Run `make` and confirm zero errors, ROM ≤ 16 MB
  - Depends On: DEV-001
  - Owner: Application Engineer

---

## PHASE: Codebase Consolidation (from consolidation spec)

- [x] DEV-003 — Rename `pokemon-crossroads/` to `pokemon-tuxedo/`
  - Req 1: Directory rename + verify build system intact
  - Depends On: DEV-002
  - Owner: Application Engineer / DevOps

- [x] DEV-004 — Copy all Tuxedo game system source files into `pokemon-tuxedo/src/`
  - Files: region_manager.c, level_scaler.c, fly_location_discovery.c, multi_region_save.c,
    item_use.c, pokemon_tuxedo.c, exp_multipliers.c, full_party_exp.c, game_settings.c
  - Req 2
  - Depends On: DEV-003
  - Owner: Application Engineer

- [x] DEV-005 — Copy all Tuxedo header files into `pokemon-tuxedo/include/`
  - Req 2
  - Depends On: DEV-003
  - Owner: Application Engineer

- [x] DEV-006 — Wire build system: link all Tuxedo source files, verify pokemon-tuxedo ROM builds
  - Req 4: `build.ps1` or `make modern` produces .gba ≤ 16 MB
  - Depends On: DEV-004, DEV-005
  - Owner: Application Engineer

- [x] DEV-007 — Confirm all 104 Hoenn maps included in consolidated build
  - Req 3: 34 routes, 16 cities, 54 dungeons — verify map_groups.json, compile clean
  - Depends On: DEV-006
  - Owner: Application Engineer

- [ ] QA-001 — Run full property-based test suite in consolidated codebase
  - All 14 named properties (Props 1–3, 14–21, 23, 24, 27), min 100 iterations each
  - Hoenn map completeness property
  - Req 17
  - Depends On: DEV-007
  - Owner: QA Engineer

- [x] DEV-008 — Clean up root directory: delete obsolete build/checkpoint docs
  - Delete: BUILD_GUIDE.md, BUILD_IN_PROGRESS.md, BUILD_NOW.md, BUILD_SUCCESS.md,
    CHECKPOINT_PHASE_2_HOENN.md, CHECKPOINT_PHASE_3_REGION_MANAGEMENT.md,
    CHECKPOINT_PHASE_4_MVP.md, CHECKPOINT_TASK_6_THROUGH_17.md,
    FINAL_BUILD_STATUS.md, FINAL_IMPLEMENTATION_REPORT.md, FINAL_TESTING_CHECKLIST.md,
    HOOKS_COMPLETION_STATUS.md, IMPLEMENTATION_SUMMARY.md, INTEGRATION_PROGRESS_REPORT.md,
    MULTI_REGION_MAP_GUIDE.md, MULTI_REGION_RESEARCH_AND_PLAN.md, MULTI_REGION_SUMMARY.md,
    NEXT_STEPS_GUIDE.md, PROJECT_COMPLETION_REPORT.md, REGIONAL_CONTENT_INTEGRATION_GUIDE.md,
    SESSION_SUMMARY.md, SYSTEM_HOOKS_INTEGRATION_STATUS.md, TASK_COMPLETION_SUMMARY.md,
    TESTING_STATUS.md, BUILD_NOW.md, BUILD_IN_PROGRESS.md, PHASE_1_QUICKSTART.md,
    CHECKPOINT_TASK_6_THROUGH_17.md
  - Req 5
  - Depends On: QA-001 (don't delete until tests confirmed green)
  - Owner: Application Engineer

---

## PHASE: Phase 5 — Johto Region Port (Post-Consolidation)

- [~] DEV-009 — Clone and analyze CrystalDust repository
  - Depends On: QA-001
  - Owner: Application Engineer

- [~] DEV-010 — Adapt format converter for CrystalDust structure
  - Depends On: DEV-009
  - Owner: Application Engineer

- [~] DEV-011 — Convert Johto routes, cities, dungeons (40+ maps)
  - Depends On: DEV-010
  - Owner: Application Engineer

- [~] DEV-012 — Convert Johto tilesets, event scripts, trainer data, wild encounters
  - Depends On: DEV-010
  - Owner: Application Engineer

- [~] DEV-013 — Integrate Johto into build system, enable Johto region unlock
  - Depends On: DEV-011, DEV-012
  - Owner: Application Engineer

- [~] QA-002 — Full test pass for 3-region game (Kanto + Hoenn + Johto)
  - Depends On: DEV-013
  - Owner: QA Engineer

---

## PHASE: Rival Scaling, Ace Pokemon, and Region Travel (Reqs 22–25)

- [x] QA-003 — Write tests for rival role-slot detection (Req 22)
  - Property: rival in encounter slot uses adaptive formula; rival in Champion slot uses Champion formula
  - Property: no circular dependency when rival occupies Champion slot (Blue/FireRed case)
  - Property: first encounter at 0 badges forces rival to level 5 regardless of formula
  - Spec Ref: Req 22, AC 1–7
  - Depends On: —
  - Complexity: M
  - Owner: QA Engineer

- [x] QA-004 — Write tests for ace Pokemon level bonus (Req 23)
  - Property: last party slot is designated ace for all trainer categories
  - Property: ace level = trainer average + 4 for all trainer types
  - Property: single-Pokemon trainer has that Pokemon treated as ace
  - Property: ace level clamped to 100
  - Spec Ref: Req 23, AC 1–8
  - Depends On: —
  - Complexity: S
  - Owner: QA Engineer

- [x] QA-005 — Write tests for ticket+fly travel system (Req 24)
  - Property: ticket use warps to starting town; ticket is not consumed
  - Property: starting town auto-registered as fly destination on first arrival
  - Property: fly destination survives save/load round-trip
  - Property: locked region ticket use is rejected with message
  - Property: party and inventory unchanged after any region transition
  - Spec Ref: Req 24, AC 1–9
  - Depends On: —
  - Complexity: M
  - Owner: QA Engineer

- [x] DEV-016 — Implement rival role-slot detection and dual-formula dispatch (Req 22)
  - Level_Scaler must inspect trainer role slot (Champion slot vs. rival encounter slot) and call the correct formula
  - Champion formula: average = wild_max + 15; ace = average + 4
  - Rival adaptive formula: average = player party average at encounter init; ace = average + 4
  - First-encounter override: 0-badge first rival encounter sets single Pokemon to level 5
  - Spec Ref: Req 22, AC 1–7; ADR-007
  - Depends On: QA-003
  - Complexity: M
  - Owner: Application Engineer

- [x] DEV-017 — Implement ace Pokemon level bonus in Level_Scaler (Req 23)
  - Last party slot = ace; ace level = trainer average + 4; clamp to 100
  - Apply to all trainer categories (Normal, Gym Leader, Elite Four, Champion, Rival)
  - Single-Pokemon trainer treated as ace
  - Spec Ref: Req 23, AC 1–8
  - Depends On: QA-004
  - Complexity: S
  - Owner: Application Engineer

- [x] DEV-018 — Implement ticket item handler as reusable key item with fly registration (Req 24)
  - Ticket is NOT consumed on use; implement as key item with no consume flag
  - On first use: trigger cutscene, warp to starting town, register starting town as fly destination
  - On subsequent uses: warp without re-registering (fly destination already exists)
  - Locked region: display rejection message, no warp
  - Party and inventory preserved; Region_Manager current region updated
  - Spec Ref: Req 24, AC 1–9; ADR-008
  - Depends On: QA-005
  - Complexity: M
  - Owner: Application Engineer

- [x] DEV-019 — Identify and remove Crossroads-native region travel mechanisms (Req 25)
  - Audit all NPC scripts, warp data, menu handlers, and event scripts in pokemon-crossroads for cross-region travel triggers
  - Delete or disable every identified trigger
  - Update or stub any remaining code references to removed triggers so build is clean
  - Preserve intra-region warps that share an entry-map boundary but are not cross-region travel
  - Spec Ref: Req 25, AC 1–7; ADR-008
  - Depends On: DEV-003 (consolidation rename must be complete before audit)
  - Complexity: L
  - Owner: Application Engineer

- [ ] QA-006 — Validate rival formula, ace bonus, and travel system in live build (Reqs 22–25)
  - Confirm rival in encounter slot uses adaptive formula (test vs. Champion slot rival)
  - Confirm Champion slot trainer (Blue) uses Champion formula, not adaptive
  - Confirm ace Pokemon is last party slot and is at trainer average + 4
  - Confirm ticket is present in bag after first use
  - Confirm starting town appears in Fly menu after first ticket use
  - Confirm no Crossroads travel trigger is reachable by normal gameplay
  - Inspect all NPC scripts, warp data, and menu handlers to confirm Req 25 AC 4 is satisfied
  - Spec Ref: Req 22–25
  - Depends On: DEV-016, DEV-017, DEV-018, DEV-019
  - Complexity: M
  - Owner: QA Engineer

---

## PHASE: Phase 6 — Polish and Release

- [~] DEV-014 — Full 3-region playthrough, ROM size check, performance pass
  - Depends On: QA-002
  - Owner: Application Engineer

- [~] DEV-015 — Final ROM build and release documentation
  - Depends On: DEV-014
  - Owner: Application Engineer
