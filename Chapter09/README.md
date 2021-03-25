# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 9: Continuous Integration and Continuous Deployment

### Packer

To try the Packer and Terraform code, you first need an AWS account. You can get
a free trial at https://aws.amazon.com/ .

Get your credentialsby following this document:
https://docs.aws.amazon.com/sdk-for-javascript/v2/developer-guide/getting-your-credentials.html
.

If you are creating a new user just for testing, you can attach the
`AdministratorAccess` policy to the created user. For production use, apply
fine-grained permissions.

Set the environment variables to the values you got from IAM using:

```
export AWS_ACCESS_KEY_ID="anaccesskey"
export AWS_SECRET_ACCESS_KEY="asecretkey"
```

If you are using Direnv, you can also put these variables to your `.env` file in
the current directory.

To build the image with Packer run

```
packer build packer_ami.json
```

When Packer finishes, take a note of the AMI it created. You need to use it with
Terraform.

### Creating Terraform resources

First, you have to initialize the Terraform state:

```
terraform init
```

To see which resources are created, use `terraform plan`. Set the variables to
point to the AMI you created with Packer and to the desired region (the region
of Terraform resources has to be the same as the AMI region used in Packer).
Here's an example:

```
terraform plan -var packer_ami=ami-0f89d498d96a38d2f -var region=eu-central-1
```

To create the resources in your account, run:

```
terraform apply -var packer_ami=ami-0f89d498d96a38d2f -var region=eu-central-1
```
