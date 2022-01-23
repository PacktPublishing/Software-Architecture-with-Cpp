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
      autoconf
      automake
      clang
      cmake
      conan
      docker
      docker-compose
      doxygen
      gcc11
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
