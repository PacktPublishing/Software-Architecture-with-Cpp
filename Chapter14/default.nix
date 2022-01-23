{ pkgs ? import (
  builtins.fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/archive/21.11-pre.tar.gz";
    sha256 = "0ww19c7n4fj9zn770aw8zaqld742bi9sa9s8hqb3vrgp3mpihil0";
  }
) {} }:

let
  additionalInputs = if pkgs.system == "x86_64-linux" then pkgs.lsb-release else "";
in
with pkgs; {
  gcc11Env = stdenvNoCC.mkDerivation {
    name = "gcc11-environment";
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
  };
}
