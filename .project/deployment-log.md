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
