{ pkgs ? import (
  builtins.fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/archive/refs/tags/24.05-pre.tar.gz";
    sha256 = "1cfbkahcfj1hgh4v5nfqwivg69zks8d72n11m5513i0phkqwqcgh";
  }
) {} }:

let
  additionalInputs = if pkgs.system == "x86_64-linux" then pkgs.lsb-release else "";
in
with pkgs; {
  gcc13Env = stdenvNoCC.mkDerivation {
    name = "gcc13-environment";
    buildInputs = [
      autoconf
      automake
      clang
      cmake
      conan
      docker
      docker-compose
      doxygen
      gcc13
      libtool
      pkg-config
      pre-commit
      additionalInputs
    ];
    shellHook = ''
      pre-commit install -f --hook-type pre-commit
    '';
  };
}
