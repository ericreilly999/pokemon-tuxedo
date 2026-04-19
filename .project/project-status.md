# Project Status — Pokemon Tuxedo

**Last Updated:** 2026-04-19  
**Current Stage:** Stage 5 — QA Validation  
**Route-to-Live:** Simple (dev → release ROM)

---

## Summary

Pokemon Tuxedo is a Pokemon FireRed ROM hack with multi-region progression (Kanto → Hoenn → Johto), dynamic level scaling, and QoL improvements. The implementation sprint (PR #1) is now merged to master. The build is clean, all 31 property-based tests pass in CI, and the ROM artifact is being uploaded to S3 on every master push. Open items are QA sign-off passes and the eventual Johto integration.

---

## What Just Completed

- **PR #1 merged** (SHA b62ccb8e3) — full implementation sprint including:
  - Build fix: system_integration.c compilation error resolved
  - Hoenn map integration: 104 maps with complete stub headers (all CI errors resolved)
  - Custom Tuxedo systems: all 9 source files wired into build
  - Rival scaling (Req 22): Champion-slot vs encounter-slot dual formula, GetPlayerTop3Average()
  - Ace Pokemon bonus (Req 23): last party slot at avg+4, clamped to 100
  - Region travel (Req 24): reusable Hoenn/Johto tickets + Fly registration
  - Crossroads travel mechanism removed (Req 25)
  - CI/CD: GitHub Actions builds ROM, runs 31 Python tests, uploads to S3 on master push
  - Terraform: S3 bucket + IAM CI user provisioned
  - Code review: all BLOCKING findings resolved, approved and merged

---

## What's In Progress

- **Master CI run 24618281043** — building ROM and uploading `pokemon-tuxedo-latest.gba` to S3 bucket `pokemon-tuxedo-releases`
- **QA-001** — formal sign-off on all 14 named properties + Hoenn map completeness (31 pass in CI; formal task sign-off pending)

---

## What's Coming Next

1. Confirm master CI passes and S3 artifact is live
2. QA-001: QA Engineer formal sign-off on full property suite in consolidated codebase
3. QA-006: Live emulator validation — rival formula, ace bonus, travel system in running ROM
4. DEV-008: Root directory cleanup — delete 25 obsolete checkpoint/status docs (gated on QA-001)
5. Phase 5: Johto integration from CrystalDust (DEV-009 onward — deferred)

---

## Risks

| Risk | Impact | Status |
|------|--------|--------|
| QA-006 emulator validation not yet done | High | Active — rival formula, ace, travel untested in live ROM |
| full_party_exp.c is a stub (Req 10 undelivered) | Medium | Tracked in TODO.md; deferred — not blocking current milestone |
| Johto (CrystalDust) integration untested | Medium | Phase 5 work — not started |
| No custom character sprites (Prof. Nana, Mister Mango) | Low | Deferred to Phase 6 |
| CI Log deployment step uses git push — fails if branch protection enabled | Low | No branch protection currently; DevOps advisory |
| Johto ticket DoWarp to MAP_UNDEFINED if Johto obtained pre-Johto maps | Low | Tickets can't be obtained in-game until Johto maps exist |

---

## Documentation Debt

25 obsolete checkpoint/status docs at repo root scheduled for deletion at DEV-008 (after QA-001 green).
