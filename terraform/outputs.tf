###############################################################################
# Outputs — Pokemon Tuxedo release infrastructure
#
# SECURITY: access_key_secret is marked sensitive. It will NOT appear in
# `terraform plan` output or standard logs. To retrieve it for the initial
# GitHub Secrets setup, run:
#
#   terraform output -raw ci_access_key_id
#   terraform output -raw ci_access_key_secret
#
# Store these values in GitHub Secrets immediately:
#   Settings → Secrets and variables → Actions → New repository secret
#   - AWS_ACCESS_KEY_ID      ← ci_access_key_id
#   - AWS_SECRET_ACCESS_KEY  ← ci_access_key_secret
#
# After storing in GitHub Secrets, do NOT log these values again.
# If the keys are ever compromised, run `terraform apply` to rotate:
#   - Delete and recreate the aws_iam_access_key resource
###############################################################################

output "releases_bucket_name" {
  description = "Name of the S3 bucket holding ROM releases."
  value       = aws_s3_bucket.releases.id
}

output "releases_bucket_arn" {
  description = "ARN of the S3 releases bucket."
  value       = aws_s3_bucket.releases.arn
}

output "ci_iam_user_arn" {
  description = "ARN of the CI/CD IAM user. Use this to audit CloudTrail events for upload activity."
  value       = aws_iam_user.ci.arn
}

output "ci_access_key_id" {
  description = "AWS access key ID for the GitHub Actions CI/CD user. Store in GitHub Secret AWS_ACCESS_KEY_ID."
  value       = aws_iam_access_key.ci.id
  sensitive   = true
}

output "ci_access_key_secret" {
  description = "AWS secret access key for the GitHub Actions CI/CD user. Store in GitHub Secret AWS_SECRET_ACCESS_KEY. Only available at creation time."
  value       = aws_iam_access_key.ci.secret
  sensitive   = true
}
