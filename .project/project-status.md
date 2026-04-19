# Project Status — Pokemon Tuxedo

**Last Updated:** 2026-04-19  
**Current Stage:** Active QA — user testing live ROM, relaying findings  
**Route-to-Live:** Simple (dev → release ROM)

---

## Summary

All consolidation and QoL feature work is complete and on master. The user is live-testing `pokemon-tuxedo-latest.gba` from S3 and reporting bugs as they encounter them. Two bugs were fixed during this session (rival level override, champion slot detection). The QoL gap batch (Reqs 9, 11, 12, 13, 14, 15) is fully implemented, code reviewed, and deployed.

---

## What Just Completed

| Task | Description |
|------|-------------|
| DEV-020 | Rival first-encounter hard override: level 5 at 0 badges regardless of party state |
| DEV-021 | Champion slot detection: Blue identified by trainer ID, not trainer class |
| DEV-022 | HM auto-clear on contact; Flash added to field moves; caves never dark |
| DEV-023 | Wild Pokemon max level reduced by 3; trainer levels use original wild max |
| DEV-024 | Lucky/Mystic/Magic Egg multipliers wired into battle exp; stacking fixed; real item IDs |
| DEV-025 | PC option and Settings option added to pause menu |
| DEV-026 | Trade evolution at level 37 for all trade-evo species |
| DEV-027 | Post-E4 starter distribution: flags set, chosen starter persisted to save block |
| QA-007–012 | 37 new property tests for Reqs 9, 11, 12, 13, 14, 15 — all passing (70 total) |

---

## Open Items

| Task | Description | Notes |
|------|-------------|-------|
| QA-006 | Live emulator validation — Reqs 22–25 | User is testing; relay findings |
| Req 8 | Full party exp — user reports it's functional | Accepted as working |
| Req 21 | Custom character sprites (Prof. Nana, Mister Mango) | Deferred to Phase 6 |
| DEV-009–015 | Phase 5 Johto + Phase 6 polish | Deferred |

---

## Active Bugs / User Reports

| Report | Status |
|--------|--------|
| Rival level 9 instead of 5 on first encounter | Fixed (DEV-020) |
| Flash not in field moves | Fixed (DEV-022) |
| Wild Pokemon too strong at low levels | Fixed (DEV-023, wild max -3) |
| Cut/Rock Smash trees block passage | Fixed (DEV-022, auto-clear on contact) |

---

## Risks

| Risk | Impact | Status |
|------|--------|--------|
| QA-006 live emulator validation not yet done — Reqs 22–25 unverified | High | Active — user testing |
| Req 21 custom sprites not implemented | Low | Deferred Phase 6 |
| Johto integration not started | Medium | Phase 5 — deferred |
| Post-E4 starter NPC scripts are stubs (flags set, no delivery UI) | Medium | Active — DEV-027 sets flags but starter gifting needs NPC scripting |
