# Workflow State — Pokemon Tuxedo

## Route-to-Live
Simple (no deployed service; output is a .gba ROM file)

## Current Stage
**Stage 6: Complete — Consolidation Phase shipped. Pending: QA-006 live emulator validation.**

## Active Route
`dev → release`

## Gate Status

| Gate | Status | Notes |
|------|--------|-------|
| Spec complete | ✅ | Two specs: full-regions (Phases 1–6) + consolidation (Req 1–21) |
| TODO.md written | ✅ | See .project/TODO.md |
| decisions.md initialised | ✅ | ADR-001 through ADR-008 all accepted |
| DEV tasks complete | ✅ | DEV-001–008, DEV-016–019 all [x] |
| PR merged | ✅ | PR #1 merged — SHA b62ccb8e3 |
| ROM built | ✅ | CI builds clean on master; 33/33 tests pass |
| S3 artifact uploaded | ✅ | `s3://pokemon-tuxedo-releases/pokemon-tuxedo-latest.gba` live |
| QA-001 sign-off | ✅ | 33 tests, all 14 Req 17 properties, Hoenn completeness — APPROVED |
| Root directory cleanup | ✅ | 25 obsolete docs removed (DEV-008) |
| QA-006 live validation | ❌ | Rival formula, ace bonus, travel — requires emulator testing |

## Next Action
Human decision: proceed to QA-006 (emulator validation of Reqs 22–25 in live ROM), or advance to Phase 5 (Johto integration from CrystalDust).

QA-006 requires loading `pokemon-tuxedo-latest.gba` in a GBA emulator and validating:
- Rival encounter levels match player's top-3 average
- Champion (Blue) uses badge-count formula, not adaptive
- Ace Pokemon is last party slot at trainer average + 4
- Hoenn Ticket in bag after first use, Littleroot Town in Fly menu
- No Crossroads travel trigger reachable in normal gameplay

## Last Updated
2026-04-19 — Consolidation phase complete
