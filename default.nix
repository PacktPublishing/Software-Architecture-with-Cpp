{ pkgs ? import (
  builtins.fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/archive/a439e0ef6eb2ab395048b3cf68a46224874245ef.tar.gz";
    sha256 = "005z8j5bikrlr2rxh0i3znygkdp8y6b9cbshm986n2mrf47zpbri";
  }
) {} }:

with pkgs;

mkShell {
  buildInputs = [
    autoconf
    automake
    clang
    cmake
    docker
    docker-compose
    doxygen
    gcc
    libtool
    lsb-release
    pkg-config
    pre-commit
    python3Packages.setuptools
    python3Packages.pip
    python3Packages.virtualenv
  ];
  shellHook = ''
    pre-commit install -f --hook-type pre-commit
    virtualenv venv
    source venv/bin/activate
    pip install conan==1.34.1 sphinx sphinx-rtd-theme breathe m2r2
  '';
}
