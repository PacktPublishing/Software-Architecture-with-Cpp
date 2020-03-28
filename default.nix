{ pkgs ? import (
  builtins.fetchTarball {
    url = "https://github.com/nixos/nixpkgs/archive/e1d58266d55a91477d57b0715e3328968419663e.tar.gz";
    sha256 = "11jjmnpbxgq749sxggbrk6718fn8k0rqhhh8rllzphgwrsf1h6cs";
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
