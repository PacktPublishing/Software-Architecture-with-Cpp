{ pkgs ? import (
  builtins.fetchTarball {
    url = "https://github.com/nixos/nixpkgs/archive/438a0cd40b2e40c0b85f6ea2eafaca094bd16bae.tar.gz";
    sha256 = "1nbc9nnlq21h2z954bhsz7jxxlknl9bn4dnl4xsjvrg4065hmbcr";
  }
) {} }:

with pkgs;

mkShell {
  buildInputs = [
    cmake
    gcc
    clang
    python3
  ];
  shellHook =''
    pip3 install conan==1.23.0
  '';
}
