# Hands-On-Software-Architecture-with-Cpp
Hands-On Software Architecture with C++ by Packt Publishing

## Chapter 14: Containers

**Warning**: Setting up . Before you run `direnv allow` in this directory, make
sure you have superuser access via `sudo` and backup the files that will be
modified (`/etc/containers/policy.json`, `/etc/containers/registries.conf`,
`$HOME/podman.conf`)

### Containers

The `containers` subdirectory has examples of different ways how to build
containers.

#### Docker

```
cd containers/docker
docker build -t basic -f basic.Dockerfile .
```

After you build the container image, you should be able to list it with `docker
images`, the new image should be listed there.

The `compile` and `multi-stage` examples require a copy of C++ sources present
in the current directory. The `scratch` example requires binary to be present in
the working directory.

#### Buildah

**Warning**: Buildah is experimental and may be unstable or require additional
setup.

Run the `./buildah.sh` script to build an image using the shell script. You can
see the built image by running `podman images`. It's named
`localhost/dominican-gcc`.

```
cd containers/buildah
./buildah.sh
```

#### Ansible Bender

**Warning**: Ansible Bender is experimental and may be unstable or require additional
setup.

To build an image with ansible-bender, run

```
cd containers/ansible-bender
ansible-bender build bender.yaml
```

Ansible Bender isn't in a stable version yet, so if you run into errors, eg.
`ERROR  Getting image source signatures`, stop the build and retry it.

You can see the built image by running `podman images`. It's named
`localhost/dominican-compiler`.

### Orchestration

The code in the `orchestration` subdirectory is only meant for illustration and
should not be used directly.
