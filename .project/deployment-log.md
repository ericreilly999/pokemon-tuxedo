# Deployment Log — Pokemon Tuxedo

This file is append-only. Every successful S3 upload from CI adds an entry
below. Do not edit or delete existing entries.

Entries are written automatically by `.github/workflows/build.yml` on every
push to master that successfully builds and uploads the ROM.

To trace any entry back to source: check out the commit SHA, rebuild, and
compare the ROM binary.

---

## Infrastructure Provisioned — 2026-04-18 (manual Terraform apply)

| Field | Value |
|-------|-------|
| Date | 2026-04-18 |
| Operator | Eric Reilly |
| Action | Initial `terraform apply` — infrastructure created from scratch |
| Resources created | 8 (S3 bucket, versioning, SSE, public access block, ownership controls, IAM user, IAM policy, IAM access key) |
| S3 bucket | `pokemon-tuxedo-releases` (us-east-1) |
| IAM user | `pokemon-tuxedo-ci` |
| Access key ID | [REDACTED — key was compromised and rotated; see security entry below] |
| State | Local (`terraform/terraform.tfstate`) — migration to S3 backend completed 2026-04-19 |
| Result | SUCCESS |

---

## SECURITY INCIDENT — 2026-04-19 (IAM key rotation and history purge)

| Field | Value |
|-------|-------|
| Date | 2026-04-19 |
| Operator | DevOps Engineer |
| Incident | IAM access key ID committed in plaintext in `.project/github-secrets-setup.md` |
| Compromised key | Deleted at 2026-04-19T00:08 UTC — confirmed inactive via `aws iam list-access-keys` |
| New key | `AKIAUHQEOQTP6ZLKGYOQ` — Terraform-managed, secret in encrypted S3 state only |
| Git history | feature/consolidation-and-new-mechanics rewritten via `git filter-branch`; force-pushed to origin; local object store pruned via `git gc --prune=now` |
| Remote state | Migrated from local `.tfstate` to `s3://pokemon-tuxedo-tfstate/pokemon-tuxedo/prod/terraform.tfstate` (AES-256 SSE, versioning enabled, public access blocked, DynamoDB lock table active) |
| Secrets doc | `.project/github-secrets-setup.md` rewritten — contains no key IDs, account IDs, or secret values |
| Result | REMEDIATED |

---

## CI GATE CHECK — 2026-04-19 (IAM key rotation wiring + PR #1 CI run)

| Field | Value |
|-------|-------|
| Date | 2026-04-19 |
| Operator | DevOps Engineer |
| Action | Wired rotated IAM credentials to GitHub Actions secrets; monitored CI run for PR #1 |
| Secrets set | AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY, AWS_REGION, S3_BUCKET, S3_KEY — all 5 confirmed via `gh secret list` |
| CI run | https://github.com/ericreilly999/pokemon-tuxedo/actions/runs/24617218838 |
| Branch | feature/consolidation-and-new-mechanics |
| PR | #1 — "Pokemon Tuxedo: codebase consolidation, new mechanics, CI/CD pipeline" |
| Workflow executing | "CI" (upstream pret/pokefirered workflow) — NOT the Pokemon Tuxedo custom "Build ROM" workflow |
| Result | FAILED — wrong workflow on feature branch |
| Failure step | "Compare FireRed" — `make -j${nproc} all syms` exits with code 2 |
| Root cause | `.github/workflows/build.yml` on feature/consolidation-and-new-mechanics is the upstream pokefirered CI (with COMPARE=1 and ROM checksum comparison). The Hoenn map additions cause a checksum mismatch against the vanilla FireRed ROM baseline. The correct Pokemon Tuxedo custom workflow (Build ROM, no COMPARE mode) lives on master but was not carried over to the feature branch. |
| S3 upload | Did NOT run — PR trigger; upload gate is master-push only |
| Routing | Application Engineer must replace .github/workflows/build.yml on feature branch with master's version. See report. |

---

## CI Investigation — 2026-04-19

| Field | Value |
|-------|-------|
| Date | 2026-04-19 |
| Operator | DevOps Engineer |
| Branch | feature/consolidation-and-new-mechanics |
| PR | #1 |

### Fixes Applied

| Commit | Fix |
|--------|-----|
| `b215a0d` | Replaced upstream pokefirered "CI" workflow (COMPARE=1) with Tuxedo workflow (MODERN=1, COMPARE=0) |
| `04e0136` | Stripped UTF-8 BOM from 104 Hoenn map.json files in data/maps/ |
| `53380e6` | Stripped UTF-8 BOM from data/maps/map_groups.json (primary cause of "expected value, got (-17)") |
| `7f8ab14` | Added libnewlib-arm-none-eabi to CI toolchain for MODERN=1 ARM standard library headers |

### Current CI Status

CI advances further with each fix. As of run 24617520842 (commit 7f8ab14):
- Toolchain: all tools build and pass
- mapjson groups/layouts/event_constants: PASS
- C compilation (all src/ files): PASS
- mapjson map per-map processing: FAIL

### Remaining Blocker — Application Engineer Required

```
tools/mapjson/mapjson map firered data/maps/CaveOfOrigin_1F/map.json ...
Failed to find matching layout for LAYOUT_CAVE_OF_ORIGIN_1F.
```

The Hoenn map.json files reference Hoenn-specific layouts (LAYOUT_CAVE_OF_ORIGIN_1F,
LAYOUT_LITTLEROOT_TOWN, etc.) that do not exist in data/layouts/layouts.json.
The layouts.json only contains vanilla FireRed layouts. All 104 Hoenn maps
need their layouts added to layouts.json, or their map.json files need to
reference existing FireRed placeholder layouts.

This is a data integrity issue requiring the Application Engineer — outside
DevOps scope. Escalating to Project Manager.

| Result | BLOCKED — Application Engineer action required |
