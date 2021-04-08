# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 12: Service Oriented Architecture

### Prerequisites

Install the following software:
- CMake 3.15
- Conan 1.34.1
- GCC 10

Assuming you're on Linux or using WSL, configure a hosacpp Conan profile and remotes by running:

```bash
conan profile new hosacpp || true
conan profile update settings.compiler=gcc hosacpp
conan profile update settings.compiler.libcxx=libstdc++11 hosacpp
conan profile update settings.compiler.version=10 hosacpp
conan profile update settings.arch=x86_64 hosacpp
conan profile update settings.os=Linux hosacpp
```

If GCC 10 is not your default compiler, you also need to add:

```bash
conan profile update env.CXX=`which g++-10` hosacpp
conan profile update env.CC=`which gcc-10` hosacpp
```

Add a Conan remote with AWS C++ SDK:

```
conan remote add kmaragon https://api.bintray.com/conan/kmaragon/conan
```

### Building

**Warning**: building AWS SDK requires lots of time and resources. If you have
less than 16GB RAM available for compilation, the compiler may run out of
memory. To mitigate this, you can reduce the number of concurrent jobs by
setting CONAN_CPU_COUNT to 1:

```
export CONAN_CPU_COUNT=1
```

To build the example app, run:

```bash
mkdir build
cd build
conan install ../s3 --build=missing -s build_type=Release -pr=hosacpp
cmake ../s3 -DCMAKE_BUILD_TYPE=Release # build type must match Conan's
cmake --build .
```

### Running the example application

You need valid AWS credentials for the program to run successfully.

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

The application creates an S3 bucket in AWS.

**Important**: Make sure to delete it after
testing or you may be billed by the AWS. You can check the S3 bucket at
https://s3.console.aws.amazon.com/s3/home?region=eu-central-1# (switch the
region accordingly with the one in the code).

### Packer

You can create an API token at
https://cloud.digitalocean.com/account/api/tokens . You need a token with Write
permissions. Copy it and either set the environment variable yourself or put it
into the `.env` file if you're using Direnv.

```
export DIGITALOCEAN_ACCESS_TOKEN=alongkeyhere
```

Building the package image requires having a customer-1.0.3.deb package in your
working directory. You may either build it using the instructions from the other
chapters or you may substitute it with another valid deb package, eg.:

```
wget http://mirrors.kernel.org/ubuntu/pool/main/t/time/time_1.7-25.1build1_amd64.deb -O packer/customer-1.0.3.deb
```

To build the image with Packer run

```
cd packer
packer build packer-digitalocean.json
```

**Important**: Make sure to delete the cloud resources after testing. You can
find the created image at https://cloud.digitalocean.com/images/snapshots/droplets

### Example manifests

The `kubernetes`, `rest`, and `rpc` directories contain example manifests and
documents presented in the book. They are only meant for illustration purposes.
