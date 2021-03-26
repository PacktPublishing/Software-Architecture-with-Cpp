{ pkgs ? import (
  builtins.fetchTarball {
    url = "https://github.com/nixos/nixpkgs/archive/f460e62d9b367a0bfd71148ef34f44421d0e82cb.tar.gz";
    sha256 = "1pazl2zy39zjih6s1dyvlxm849mwj3f6ijji2c3913n6kw0s3nh0";
  }
) {} }:

with pkgs;

mkShell {
  buildInputs = [
    ansible
    autoconf
    automake
    buildah
    conmon
    clang
    cmake
    docker
    ensureNewerSourcesForZipFilesHook
    gcc
    libtool
    lsb-release
    nomad
    pkg-config
    podman
    pre-commit
    runc
    python3Packages.setuptools
    python3Packages.pip
    python3Packages.virtualenv
    slirp4netns
  ];
  shellHook = ''
    pre-commit install -f --hook-type pre-commit
    # <setup for buildah toolchain>
    sudo mkdir -p /etc/containers
    cat <<EOF | sudo tee /etc/containers/policy.json
    {
        "default": [
            {
                "type": "insecureAcceptAnything"
            }
        ]
    }
    EOF
    cat <<EOF | sudo tee /etc/containers/registries.conf
    [registries.search]
    registries = [ 'docker.io' ]
    EOF

    # documentation for this is very disorganized at this point
    # see https://github.com/containers/libpod/blob/master/docs/libpod.conf.5.md
    cat <<EOF | tee $HOME/podman.conf
    conmon_path = [ "$(which conmon)" ]
    events_logger = "file"
    [runtimes]
    runc = [ "$(which runc)" ]
    EOF
    unset SOURCE_DATE_EPOCH
    virtualenv venv
    source venv/bin/activate
    pip install ansible-bender==0.9.0 docker-compose==1.25.5
  '';
}
