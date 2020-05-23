{ pkgs ? import (
  builtins.fetchTarball {
    url = "https://github.com/nixos/nixpkgs/archive/f460e62d9b367a0bfd71148ef34f44421d0e82cb.tar.gz";
    sha256 = "1pazl2zy39zjih6s1dyvlxm849mwj3f6ijji2c3913n6kw0s3nh0";
  }
) {} }:

with pkgs;

mkShell {
  buildInputs = [
    autoconf
    automake
    clang
    cmake
    gcc
    openssl
    libtool
    pre-commit
    python3Packages.setuptools
  ];
  shellHook = ''
    pre-commit install
  '';
}
