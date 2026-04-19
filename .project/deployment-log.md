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
| IAM user | `arn:aws:iam::REDACTED_ACCOUNT_ID:user/pokemon-tuxedo-ci` |
| Access key ID | `REDACTED_SEE_INCIDENT_LOG` |
| State | Local (`terraform/terraform.tfstate`) — migration to S3 backend pending |
| Result | SUCCESS |
