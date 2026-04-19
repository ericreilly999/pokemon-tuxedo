###############################################################################
# Pokemon Tuxedo — Release Infrastructure
#
# Provisions:
#   - S3 bucket for ROM distribution
#   - IAM user with least-privilege PutObject access for CI/CD
#
# NOTE: This is intentionally flat (no modules) because there is exactly one
# logical domain (storage) and no compute, networking, or database components.
# If this grows (e.g., CloudFront for distribution, Lambda for release hooks),
# extract a modules/storage/ module at that point.
#
# Run once to provision. CI/CD credentials are output below — store them
# immediately in GitHub Secrets and do not log them anywhere else.
###############################################################################

terraform {
  required_version = ">= 1.6.0"

  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
  }

  # Remote state: uncomment and configure after creating the state bucket.
  # Until the state bucket exists, run with local state for the initial apply,
  # then migrate with `terraform init -migrate-state`.
  #
  # backend "s3" {
  #   bucket         = "pokemon-tuxedo-tfstate"
  #   key            = "pokemon-tuxedo/prod/terraform.tfstate"
  #   region         = "us-east-1"
  #   dynamodb_table = "pokemon-tuxedo-tfstate-lock"
  #   encrypt        = true
  # }
}

provider "aws" {
  region = var.aws_region

  default_tags {
    tags = {
      Project     = var.project_name
      Environment = var.environment
      ManagedBy   = "terraform"
    }
  }
}

###############################################################################
# S3 Bucket — ROM distribution
###############################################################################

resource "aws_s3_bucket" "releases" {
  bucket = var.bucket_name

  # Prevent accidental deletion via `terraform destroy`.
  # To actually delete this bucket, first set lifecycle.prevent_destroy = false
  # and re-apply, then destroy.
  lifecycle {
    prevent_destroy = true
  }
}

# Versioning DISABLED — single-object overwrite model.
# Versioning is intentionally off: the bucket holds exactly one object
# (pokemon-tuxedo-latest.gba) which is overwritten on every master push.
# Release history is tracked via git tags, not S3 object versions.
# Enabling versioning would accumulate stale 16 MB objects and create
# cost drift over time with no operational benefit.
resource "aws_s3_bucket_versioning" "releases" {
  bucket = aws_s3_bucket.releases.id

  versioning_configuration {
    status = "Disabled"
  }
}

# Server-side encryption at rest — required by Security pillar.
# Objects are small (≤16 MB) so the AES-256 overhead is negligible.
resource "aws_s3_bucket_server_side_encryption_configuration" "releases" {
  bucket = aws_s3_bucket.releases.id

  rule {
    apply_server_side_encryption_by_default {
      sse_algorithm = "AES256"
    }
    bucket_key_enabled = true
  }
}

# Block all public access — this bucket is NOT for public distribution.
# Downloads are private; if a public download link is needed in future,
# add a CloudFront distribution with an Origin Access Control policy
# rather than making the bucket public.
resource "aws_s3_bucket_public_access_block" "releases" {
  bucket = aws_s3_bucket.releases.id

  block_public_acls       = true
  block_public_policy     = true
  ignore_public_acls      = true
  restrict_public_buckets = true
}

# Bucket ownership controls — enforces bucket owner for all objects.
# Prevents ACL confusion if a cross-account upload ever occurs.
resource "aws_s3_bucket_ownership_controls" "releases" {
  bucket = aws_s3_bucket.releases.id

  rule {
    object_ownership = "BucketOwnerEnforced"
  }
}

###############################################################################
# IAM — CI/CD user (least-privilege)
###############################################################################

# This IAM user is the identity used by GitHub Actions to upload the ROM.
# It has exactly one permission: PutObject on the single release key.
# It cannot ListBucket, GetObject, DeleteObject, or act on any other resource.
resource "aws_iam_user" "ci" {
  name = var.ci_iam_user_name

  tags = {
    Purpose = "GitHub Actions CI/CD ROM upload"
  }
}

# Inline policy — scoped to a single S3 object key, not the entire bucket.
# This is the minimum viable permission for the CI/CD upload step.
#
# Well-Architected note (Security pillar):
#   s3:PutObject is granted on the exact object key, not on /* or /*.
#   The CI user cannot read, list, or delete any object. If the key needs
#   to change, update this policy and rotate credentials.
resource "aws_iam_user_policy" "ci_upload" {
  name = "pokemon-tuxedo-ci-upload"
  user = aws_iam_user.ci.name

  policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Sid    = "AllowROMUpload"
        Effect = "Allow"
        Action = ["s3:PutObject"]
        Resource = [
          "arn:aws:s3:::${var.bucket_name}/${var.release_object_key}"
        ]
      }
    ]
  })
}

# Access key for the CI user.
# SECURITY: The secret access key is only available at creation time.
# It is exposed in Terraform state — ensure state is encrypted (S3 backend
# with SSE) and access to the state bucket is restricted. Do NOT log the
# output of `terraform output` in CI, and do NOT commit .tfstate files.
resource "aws_iam_access_key" "ci" {
  user = aws_iam_user.ci.name
}
