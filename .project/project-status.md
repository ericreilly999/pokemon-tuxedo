# Project Status — Pokemon Tuxedo

**Last Updated:** 2026-04-19  
**Current Stage:** Consolidation Phase Complete — QA-006 pending  
**Route-to-Live:** Simple (dev → release ROM)

---

## Summary

The consolidation and implementation sprint is fully shipped. The ROM builds clean, 33 property-based tests pass in CI, the `pokemon-tuxedo-latest.gba` artifact is live on S3, and the root directory is cleaned of 25 obsolete docs. The next decision point is whether to proceed to QA-006 (live emulator validation) or advance directly to Phase 5 Johto integration.

---

## What Just Completed

| Task | Description | Status |
|------|-------------|--------|
| DEV-001–002 | Build fix + ROM build verified | ✅ |
| DEV-003–007 | Codebase consolidation (rename, copy, wire, verify maps) | ✅ |
| DEV-016–019 | Rival scaling, ace bonus, ticket travel, Crossroads removal | ✅ |
| QA-003–005 | Property tests written pre-implementation (31 properties) | ✅ |
| QA-001 | Full property suite + Hoenn completeness (33/33 pass) | ✅ |
| DEV-008 | Root cleanup: 25 obsolete docs removed | ✅ |
| CI/CD | GitHub Actions builds ROM, runs tests, uploads to S3 on master push | ✅ |
| Terraform | S3 bucket + IAM CI user provisioned with remote state | ✅ |
| PR #1 | Code review approved, merged to master | ✅ |

---

## Open Items

| Task | Description | Blocker |
|------|-------------|---------|
| QA-006 | Live emulator validation — rival formula, ace bonus, travel system | Requires GBA emulator + test play |
| DEV-009–013, QA-002 | Phase 5: Johto integration from CrystalDust | Human go-ahead |
| DEV-014–015, Phase 6 | Polish, 3-region playthrough, release | Depends on QA-002 |

---

## Risks

| Risk | Impact | Status |
|------|--------|--------|
| QA-006 emulator validation not yet done — Reqs 22–25 unverified in live ROM | High | Active |
| `full_party_exp.c` is a stub (Req 10 undelivered) | Medium | Deferred — tracked in TODO.md |
| Johto (CrystalDust) integration untested | Medium | Phase 5 — not started |
| Johto ticket calls DoWarp to MAP_UNDEFINED pre-Johto | Low | Can't be triggered until Johto maps exist |
| No custom character sprites (Prof. Nana, Mister Mango) | Low | Deferred to Phase 6 |
