# GitHub Actions Secrets Setup

Terraform apply completed successfully on 2026-04-18. All five GitHub Actions
secrets below must be set before the first push to master will produce a
successful S3 upload. The build and test steps run regardless — only the upload
step requires these secrets.

---

## How to Add Secrets

1. Open the repository on GitHub.
2. Go to Settings -> Secrets and variables -> Actions.
3. Click "New repository secret" for each entry below.
4. Paste the value exactly as shown (no leading/trailing spaces).

---

## Required Secrets

### AWS_ACCESS_KEY_ID

The access key ID for the `pokemon-tuxedo-ci` IAM user created by Terraform.
This user has one permission: `s3:PutObject` on the single key
`arn:aws:s3:::pokemon-tuxedo-releases/pokemon-tuxedo-latest.gba`.

Value: run the following command locally to retrieve it from Terraform state:

    cd terraform/
    terraform output -raw ci_access_key_id

The key ID provisioned by this apply is: `REDACTED_ROTATE_KEY`

---

### AWS_SECRET_ACCESS_KEY

The secret access key paired with the above key ID. This value is only
available via Terraform state — it cannot be retrieved from the AWS console
after creation. If lost, rotate by destroying and recreating the
`aws_iam_access_key.ci` resource.

Value: run the following command locally to retrieve it from Terraform state:

    cd terraform/
    terraform output -raw ci_access_key_secret

Store this value immediately after retrieval. Do not log it, paste it into
chat, or commit it anywhere.

---

### AWS_REGION

The AWS region where the S3 bucket was provisioned.

Value: `us-east-1`

---

### S3_BUCKET

The name of the S3 bucket that receives the built ROM file.

Value: `pokemon-tuxedo-releases`

---

### S3_KEY

The object key within the bucket. This key is overwritten on every push to
master. Versioning is intentionally disabled — release history is tracked via
git tags, not S3 object versions.

Value: `pokemon-tuxedo-latest.gba`

---

## Terraform State — Local vs Remote

### Current state: local

Terraform state is currently stored locally at `terraform/terraform.tfstate`.
This file contains the IAM secret access key in plaintext. It must never be
committed to git. The `.gitignore` at the repo root has been updated to exclude:

    terraform/.terraform/
    terraform/terraform.tfstate
    terraform/terraform.tfstate.backup
    terraform/.terraform.lock.hcl

Verify the exclusions are in place before your next `git add .` or `git push`.

### Migrating to remote state (required for team use)

Local state is acceptable for a single operator. For team access or CI-driven
`terraform plan` runs, migrate to S3 backend as follows:

1. Create a dedicated state bucket and DynamoDB lock table (manually or via a
   separate bootstrap Terraform config):

       aws s3api create-bucket \
         --bucket pokemon-tuxedo-tfstate \
         --region us-east-1

       aws s3api put-bucket-versioning \
         --bucket pokemon-tuxedo-tfstate \
         --versioning-configuration Status=Enabled

       aws s3api put-bucket-server-side-encryption-configuration \
         --bucket pokemon-tuxedo-tfstate \
         --server-side-encryption-configuration \
           '{"Rules":[{"ApplyServerSideEncryptionByDefault":{"SSEAlgorithm":"AES256"}}]}'

       aws dynamodb create-table \
         --table-name pokemon-tuxedo-tfstate-lock \
         --attribute-definitions AttributeName=LockID,AttributeType=S \
         --key-schema AttributeName=LockID,KeyType=HASH \
         --billing-mode PAY_PER_REQUEST \
         --region us-east-1

2. Uncomment the `backend "s3"` block in `terraform/main.tf`.

3. Run `terraform init -migrate-state` from the `terraform/` directory.
   Terraform will copy local state to S3 and delete the local `.tfstate` file.

4. Restrict access to the state bucket — only operators and CI should have
   `s3:GetObject` / `s3:PutObject` on it. The `pokemon-tuxedo-ci` IAM user
   used by GitHub Actions does NOT need access to the state bucket; it only
   uploads the ROM.

---

## Key Rotation

If the `AWS_SECRET_ACCESS_KEY` is ever compromised:

1. In `terraform/main.tf`, taint the access key resource:

       terraform taint aws_iam_access_key.ci

2. Run `terraform apply`. Terraform will delete the old key and create a new
   one. The new ID and secret will appear in `terraform output`.

3. Update the GitHub Secrets with the new values immediately.

4. Confirm the old key is deactivated in the AWS IAM console.

---

## Resources Provisioned

| Resource | ID / ARN |
|----------|----------|
| S3 bucket | `pokemon-tuxedo-releases` (us-east-1) |
| S3 bucket ARN | `arn:aws:s3:::pokemon-tuxedo-releases` |
| IAM user | `pokemon-tuxedo-ci` |
| IAM user ARN | `arn:aws:iam::REDACTED_ACCOUNT_ID:user/pokemon-tuxedo-ci` |
| IAM policy | `pokemon-tuxedo-ci-upload` (inline, s3:PutObject on single key) |
| Access key ID | `REDACTED_ROTATE_KEY` |

All resources are tagged: `Project=pokemon-tuxedo`, `Environment=prod`,
`ManagedBy=terraform`.
