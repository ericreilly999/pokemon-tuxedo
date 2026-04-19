# Project Status — Pokemon Tuxedo

**Last Updated:** 2026-04-18  
**Current Stage:** Stage 2 — Development (BLOCKED)  
**Route-to-Live:** Simple (dev → release ROM)

---

## Summary

Pokemon Tuxedo is a Pokemon FireRed ROM hack with multi-region progression (Kanto → Hoenn → Johto), dynamic level scaling, and QoL improvements. The project has significant real progress — custom game systems exist, 533 maps are present, and a format converter toolchain is in place — but the codebase accumulated 25+ contradictory status documents, a broken build, and a new consolidation spec that was written but never executed. The project needs a clean reset of its tracking structure and a focused sprint to fix the build, then execute the consolidation.

---

## What's Actually True (Ground Truth)

| Item | Claimed | Reality |
|------|---------|---------|
| Phases 1–4 complete | tasks.md says [x] | Phase 1–2 real. Phase 3 partially real. Phase 4 **false** — build is broken |
| 5426+ tests passing | README claims 100% | Python tests exist; C property tests exist; **none confirmed passing against a working build** |
| ROM successfully built | BUILD_SUCCESS.md | **False** — build.log shows compilation error in system_integration.c |
| Hoenn port complete | Tasks [x] | 533 maps in data/maps/, pokemon-crossroads/ has Hoenn data — **likely true** |
| Consolidation complete | Not claimed | Consolidation spec exists, **zero work done** |

---

## What Just Completed

- Phase 1: Format converter toolchain (map_converter tools, Python converters) ✅
- Phase 2: Hoenn map data conversion (104 maps in data/maps/ + pokemon-crossroads) ✅
- Phase 3: Custom Tuxedo source files written and placed in src/ ✅ (but build broken)
- Consolidation requirements spec written ✅

---

## What's In Progress / Blocked

- **BLOCKER**: `src/system_integration.c:123` — `too many arguments to function 'SetEliteFourDefeated'`
  - Prevents ROM from building
  - Must be fixed before any other progress is meaningful
  - Owner: Application Engineer (DEV-001, DEV-002)

---

## What's Coming Next

1. Fix build error (DEV-001, DEV-002)
2. Execute codebase consolidation (DEV-003 through DEV-008): rename pokemon-crossroads → pokemon-tuxedo, copy Tuxedo systems in, verify ROM builds from single directory
3. Confirm test suite passes (QA-001)
4. Clean up 25+ obsolete documentation files (DEV-008)
5. Phase 5: Johto integration from CrystalDust (DEV-009 onward)

---

## Risks

| Risk | Impact | Status |
|------|--------|--------|
| Build error may be deeper than one bad function call | High | Active — investigate after DEV-001 |
| "5426+ tests passing" claim unverified | High | Will be confirmed at QA-001 |
| Two specs in .kiro/specs/ with different scopes — developers may not know which is authoritative | Medium | Consolidation spec supersedes full-regions spec for current work; tracked in ADR-006 |
| pokemon-crossroads is pokeemerald-based; root is pokefirered-based — cross-engine merge is non-trivial | High | Core risk of consolidation; Application Engineer must resolve symbol conflicts |
| No custom character sprites yet (Prof. Nana, Mister Mango) | Low | Placeholder sprites needed; deferred to Phase 6 |
| Johto (CrystalDust) integration is untested | Medium | Phase 5 work — not yet started |

---

## Documentation Debt (to be cleaned up at DEV-008)

25 obsolete status/checkpoint documents at repo root that are contradictory. Will be deleted after QA-001 confirms green tests.
