# Configure the AWS provider
provider "aws" {
  region  = var.region
  version = "~> 2.7"
}

# Input variable pointing to an SSH key we ant to associate with the newly
# created machine
variable "public_key_path" {
  description = <<DESCRIPTION
Path to the SSH public key to be used for authentication.
Ensure this keypair is added to your local SSH agent so provisioners can
connect.
Example: ~/.ssh/terraform.pub
DESCRIPTION

  default = "~/.ssh/id_rsa.pub"
}

# Input variable with a name to attach to the SSH key
variable "aws_key_name" {
  description = "Desired name of AWS key pair"
  default     = "terraformer"
}

# An ID from our previous Packer run that points to the custom base image
variable "packer_ami" {
}

variable "env" {
  default = "development"
}

variable "region" {
}

# Create a new AWS key pair cotaining the public key set as the input
# variable
resource "aws_key_pair" "deployer" {
  key_name   = var.aws_key_name
  public_key = file(var.public_key_path)
}

# Create a VM instance from the custom base image that uses the previously
# created key
# The VM size is t2.xlarge, it uses a persistent storage volume of 60GiB,
# and is tagged for easier filtering
resource "aws_instance" "project" {
  ami           = var.packer_ami
  instance_type = "t2.xlarge"
  key_name      = aws_key_pair.deployer.key_name
  root_block_device {
    volume_type = "gp2"
    volume_size = 60
  }
  tags = {
    Provider = "terraform"
    Env      = var.env
    Name     = "main-instance"
  }
}
