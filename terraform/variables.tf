###############################################################################
# Variables — Pokemon Tuxedo release infrastructure
###############################################################################

variable "aws_region" {
  description = "AWS region for all resources."
  type        = string
  default     = "us-east-1"

  validation {
    condition     = can(regex("^[a-z]{2}-[a-z]+-[0-9]+$", var.aws_region))
    error_message = "aws_region must be a valid AWS region identifier (e.g. us-east-1)."
  }
}

variable "project_name" {
  description = "Project name applied to all resource tags."
  type        = string
  default     = "pokemon-tuxedo"
}

variable "environment" {
  description = "Deployment environment name applied to all resource tags."
  type        = string
  default     = "prod"

  validation {
    condition     = contains(["dev", "staging", "prod"], var.environment)
    error_message = "environment must be one of: dev, staging, prod."
  }
}

variable "bucket_name" {
  description = "Name of the S3 bucket used to store the built ROM file."
  type        = string
  default     = "pokemon-tuxedo-releases"

  validation {
    condition     = can(regex("^[a-z0-9][a-z0-9\\-]{1,61}[a-z0-9]$", var.bucket_name))
    error_message = "bucket_name must be a valid S3 bucket name (lowercase, 3–63 chars, no underscores)."
  }
}

variable "release_object_key" {
  description = "S3 object key for the canonical latest ROM build. This key is overwritten on every master push."
  type        = string
  default     = "pokemon-tuxedo-latest.gba"
}

variable "ci_iam_user_name" {
  description = "IAM username for the GitHub Actions CI/CD service account."
  type        = string
  default     = "pokemon-tuxedo-ci"
}
