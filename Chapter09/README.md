# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 9: Continuous Integration and Continuous Deployment

### Packer

To try the Packer and Terraform code, you first need an AWS account. You can get
a free trial at https://aws.amazon.com/ .

Get your credentials by following this document:
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
the current directory. Fill in your secret values in the `env.example` file and
rename it to `.env`.

Set up AWS CLI (following this manual: https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-files.html) and run `aws configure`

To build the image with Packer run

```
packer build packer_ami.json
```

When Packer finishes, take a note of the AMI it created. You need to use it with
Terraform. The AMI will look like the example one: ami-0f89d498d96a38d2f

**Important**: make sure to clean all the
cloud resources. Log in to the AWS Web console and remove
all the AMIs and snapshots:
https://eu-central-1.console.aws.amazon.com/ec2/v2/home?region=eu-central-1#Images:sort=name
. Otherwise you will be billed by AWS based on your resource usage.

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

Terraform will use a default key pair based on the keys in your home directory.
If you don't have an SSH key pair, you can create it running
`ssh-keygen -t rsa -m PEM`

You can also use an existing key pair by following https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ec2-key-pairs.html#how-to-generate-your-own-key-and-import-it-to-aws

To create the resources in your account, run:

```
terraform apply -var packer_ami=ami-0f89d498d96a38d2f -var region=eu-central-1
```

**Important**: make sure to run `terraform destroy` afterwards to clean all the
resources created by Terraform. Also: log in to the AWS Web console and remove
all the AMIs and snapshots:
https://eu-central-1.console.aws.amazon.com/ec2/v2/home?region=eu-central-1#Images:sort=name
. Otherwise you will be billed by AWS based on your resource usage.

```
terraform destroy -var packer_ami=ami-0f89d498d96a38d2f -var region=eu-central-1
```

### BDD

The code in the `bdd` subdirectory is only meant for illustration and should not
be built directly.
