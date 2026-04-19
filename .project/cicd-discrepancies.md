# CI/CD Discrepancies — ROM Hack vs. Standard Web Application

**Project:** Pokemon Tuxedo (Pokemon FireRed ROM hack)  
**Pipeline:** `.github/workflows/build.yml`  
**Date:** 2026-04-18  
**Author:** DevOps Engineer

This document records every place where the Pokemon Tuxedo CI/CD pipeline
deviates from standard DevOps practice for a web application, and explains
why each deviation is correct for a ROM hack project. This is the reference
document for any engineer who looks at this pipeline and wonders "why doesn't
it do X?"

---

## Deviation Table

| # | Standard Web App Practice | Pokemon Tuxedo Reality | Reason |
|---|--------------------------|----------------------|--------|
| 1 | Build artifact is a Docker image | Build artifact is `pokefirered.gba` — a 16 MB GBA ROM binary | There is no server, no container, no runtime environment. The ROM IS the product. |
| 2 | Build with npm / Gradle / Maven / pip | Build with GNU Make + custom Makefile | pokefirered uses the pret decomp build system, which is a hand-authored Makefile targeting GBA hardware. |
| 3 | Compile with gcc or clang | Compile with agbcc — a fork of gcc 2.95 targeting ARM7TDMI with C89 strict mode | The GBA CPU is an ARM7TDMI running at 16 MHz. Modern compilers produce code that is too large or uses instructions the CPU does not support. agbcc is the only supported compiler for this codebase. |
| 4 | Push image to ECR or Docker Hub | Upload `.gba` file to S3 | No container registry is needed. S3 is the simplest distribution store for a binary file. |
| 5 | Deploy to ECS / Lambda / EC2 | No deployment target | The ROM runs on GBA hardware or an emulator on the end user's machine. There is nothing to deploy to. |
| 6 | Health checks against a live endpoint | ROM size validation (≤ 16,777,216 bytes) | There is no running service to probe. The size check is the closest equivalent: a ROM that exceeds 16 MB will not fit on the target flash cartridge and is treated as a hard build failure. |
| 7 | Automated smoke tests against a live environment | Python property-based tests exercising game logic models | Emulator testing (mGBA) is manual. The Python tests validate the game system logic (level scaling, region management, exp curves) without requiring a running GBA. |
| 8 | Staging environment (ECS service, RDS, etc.) | No staging server — testing is via mGBA emulator locally | There is no infrastructure to stage against. QA runs the ROM in mGBA on their local machine. The CI build produces the artifact; the tester runs it. |
| 9 | Rollback by re-deploying previous image tag from ECR | Re-upload a previous `.gba` from a git tag | S3 versioning is disabled (single-object overwrite model). To roll back, check out the git tag for the previous release, rebuild, and upload. Git is the source of truth for versioning. |
| 10 | devkitPro Docker image (`devkitpro/devkitarm`) required for devkitARM builds | Ubuntu-latest runner + apt-installed arm-none-eabi + agbcc from source | The upstream pokefirered repo CI (pret/pokefirered) compiles agbcc from source on ubuntu-latest. This is proven to work. The devkitPro Docker image is an alternative if the build system is later migrated to use devkitARM exclusively (e.g., a MODERN=1 only build). |
| 11 | Database migration step | Not applicable | A GBA ROM has no database. All game state is stored in the cartridge save RAM (SRAM/EEPROM/Flash), which is managed by the ROM itself at runtime. |
| 12 | Secrets injected at runtime (env vars, Secrets Manager) | No runtime secrets — the ROM is a static binary | The ROM does not make network calls, has no API keys, and has no server-side secrets. AWS credentials exist only in GitHub Secrets for the S3 upload step in CI, not in the ROM itself. |
| 13 | Multi-environment promotion (dev → staging → prod) | Single-environment release (build → S3) | The route-to-live for this project is `dev → release`. There is no staging infrastructure because there is nothing to run. The QA gate is: build succeeds + tests pass + manual emulator validation. |
| 14 | Container image scanning (Trivy on image layers) | Trivy not applicable — no container image is built | If a Dockerfile is added in future (e.g., for a build environment container), add Trivy scanning at that point. |
| 15 | SAST on application code (Semgrep) | Semgrep not in current pipeline | The codebase is C89 targeting a bare-metal GBA — many standard Semgrep C rules produce false positives against agbcc-specific patterns. SAST is a recommended follow-up once Semgrep rulesets are evaluated against this codebase. |
| 16 | Checkov on Terraform (blocks on HIGH/CRITICAL) | Checkov applicable — should be added | The Terraform in `terraform/` is standard AWS Terraform. Checkov should be added to the PR validation pipeline once a `pr-validation.yml` workflow is introduced. Currently there is only a single `build.yml`. **This is an open gap — see Well-Architected notes below.** |
| 17 | Build on Windows (developer machine) | CI targets Linux (ubuntu-latest) | Developers use Windows with PowerShell convenience scripts (`build.ps1`, scripts in `tools/`). CI runs on Linux to minimise runner cost (Linux runners are cheaper than Windows runners on GitHub Actions) and because the agbcc build toolchain is Linux-native. The PowerShell scripts in `tools/` are developer tools only and are not invoked by CI. |

---

## Well-Architected Review

### Security

**IAM least-privilege — SATISFIED**

The CI/CD IAM user (`pokemon-tuxedo-ci`) has a single inline policy granting
only `s3:PutObject` on the exact object key
`arn:aws:s3:::pokemon-tuxedo-releases/pokemon-tuxedo-latest.gba`. It cannot
list, read, or delete any object. It has no IAM, EC2, or other permissions.

This is the minimum viable permission surface for the upload task.

**S3 bucket public access — SATISFIED**

The bucket has all four public-access-block settings enabled. The ROM is not
publicly downloadable via S3. If public distribution is needed, the correct
pattern is a CloudFront distribution with an Origin Access Control (OAC)
policy — do not relax the bucket policy to allow public reads.

**Secrets management — SATISFIED**

AWS credentials live in GitHub Secrets only. They are not logged, not in code,
and not in Terraform state (the access key secret is output as `sensitive =
true`). Terraform state must be stored on an encrypted S3 backend (configured
but commented out until the state bucket is created — see `terraform/main.tf`).

**Gap — no Checkov in pipeline**

Checkov is not currently scanning the Terraform files in CI. Add a
`pr-validation.yml` workflow with a Checkov step blocking on HIGH/CRITICAL
findings. This is a follow-up item, not a production blocker (the Terraform is
simple and has been manually reviewed).

### Cost

**S3 storage — negligible**

One 16 MB object in S3 Standard in us-east-1:

- Storage: 0.016 GB × $0.023/GB-month = **~$0.00037/month**
- PUT requests: ~$0.000005 per push
- Total: effectively $0

No Reserved Instances, no compute, no data transfer costs (bucket is private,
no egress).

### Reliability

**Single-object overwrite — DOCUMENTED RISK**

The overwrite model means there is no S3 history of previous ROMs. If the
master branch build produces a bad ROM and is uploaded before the issue is
caught, recovery requires:

1. Identify the last good git commit tag
2. Check out that tag locally (or trigger a CI run from it)
3. Build the ROM
4. Manually upload to S3

**Mitigation:** Use git tags for every intentional release (e.g., `v1.0.0`,
`v1.1.0`). The `deployment-log.md` records the commit SHA of every S3 upload,
providing a traceable reference to the source commit. With the commit SHA, any
previous build can be reproduced.

S3 versioning is intentionally disabled to avoid accumulating 16 MB objects
with no automatic cleanup. If this risk is unacceptable, enable versioning and
add a lifecycle rule to expire non-current versions after N days.

**No multi-AZ concern**

S3 Standard is inherently multi-AZ within a region. No additional reliability
configuration is needed for a static file store.

### Performance Efficiency

Not applicable. The ROM is a static file. S3 PUT/GET latency is irrelevant
for an infrequently-updated binary. If download speed becomes a concern for
end users, add a CloudFront distribution.

### Operational Excellence

**Deployment logging — IMPLEMENTED**

Every successful master-branch upload appends an entry to
`.project/deployment-log.md` via a git commit from the CI runner. This
provides an immutable, auditable record of every ROM published to S3.

**Open gap — no CloudWatch alarms**

There is no compute to monitor, so the standard "monitor for 10 minutes after
deploy" pattern does not apply. If a Lambda or API is added in future, add
CloudWatch alarms at that point.

---

## Initial Setup Checklist

Run these steps once before the first CI push to master:

1. `cd terraform && terraform init`
2. `terraform plan` — review, confirm no unexpected changes
3. `terraform apply` — provisions bucket and IAM user
4. `terraform output -raw ci_access_key_id` → copy value
5. `terraform output -raw ci_access_key_secret` → copy value
6. In the GitHub repository: Settings → Secrets and variables → Actions
   - Create secret `AWS_ACCESS_KEY_ID` with the value from step 4
   - Create secret `AWS_SECRET_ACCESS_KEY` with the value from step 5
7. Do NOT log or store the secret access key anywhere else

After setup, every push to master will build the ROM, run tests, and upload
`pokefirered.gba` to `s3://pokemon-tuxedo-releases/pokemon-tuxedo-latest.gba`.
