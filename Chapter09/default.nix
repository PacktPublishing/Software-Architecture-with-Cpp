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
      awscli
      clang
      conan
      cmake
      docker
      docker-compose
      doxygen
      gcc
      libtool
      lsb-release
      packer
      pkg-config
      pre-commit
      python3Packages.setuptools
      python3Packages.pip
      python3Packages.virtualenv
      terraform
    ];
    shellHook = ''
      virtualenv venv
      source venv/bin/activate
      pip install sphinx sphinx-rtd-theme breathe m2r2
    '';
  };
}
