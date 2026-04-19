# GitHub Actions Secrets Setup

The IAM access key for `pokemon-tuxedo-ci` is managed by Terraform and stored
in encrypted Terraform state in S3. No key IDs, account IDs, or secret values
are stored in this file or in any committed file.

---

## How to Add Secrets

1. Open the repository on GitHub.
2. Go to Settings -> Secrets and variables -> Actions.
3. Click "New repository secret" for each entry below.

---

## Required Secrets (5 total)

### 1. AWS_ACCESS_KEY_ID

The access key ID for the `pokemon-tuxedo-ci` IAM user.

To retrieve, run from the `terraform/` directory:

    cd terraform/
    terraform output -raw ci_access_key_id

### 2. AWS_SECRET_ACCESS_KEY

The secret access key paired with `AWS_ACCESS_KEY_ID`. Only available via
Terraform state — it cannot be retrieved from the AWS console after creation.

To retrieve, run from the `terraform/` directory:

    cd terraform/
    terraform output -raw ci_access_key_secret

Store this value immediately after retrieval. Do not log it, paste it into
chat, or commit it anywhere.

### 3. AWS_REGION

The AWS region where the S3 bucket is provisioned.

Value: `us-east-1`

### 4. S3_BUCKET

The name of the S3 bucket that receives the built ROM file.

Value: `pokemon-tuxedo-releases`

### 5. S3_KEY

The object key within the bucket. Overwritten on every push to master.
Release history is tracked via git tags, not S3 object versions.

Value: `pokemon-tuxedo-latest.gba`

---

## Terraform State

State is stored remotely in S3:

- Bucket: `pokemon-tuxedo-tfstate`
- Key: `pokemon-tuxedo/prod/terraform.tfstate`
- Region: `us-east-1`
- Encryption: AES-256 (SSE-S3)
- Locking: DynamoDB table `pokemon-tuxedo-tfstate-lock`

Local `terraform.tfstate` is 0 bytes and gitignored. Do not commit it.

---

## Key Rotation

If the IAM key must be rotated:

1. Taint the access key resource:

       cd terraform/
       terraform taint aws_iam_access_key.ci

2. Run `terraform apply`. Terraform creates a new key and stores the
   new ID and secret in encrypted S3 state.

3. Retrieve the new values:

       terraform output -raw ci_access_key_id
       terraform output -raw ci_access_key_secret

4. Update the GitHub Secrets (`AWS_ACCESS_KEY_ID`, `AWS_SECRET_ACCESS_KEY`)
   with the new values immediately.

5. Confirm only one key is active:

       aws iam list-access-keys --user-name pokemon-tuxedo-ci
