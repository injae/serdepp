# in flake.nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          overlays = [];
          pkgs = import nixpkgs {
            inherit system overlays;
          };
        in
        with pkgs;
        {
          devShells.default = mkShell {
            nativeBuildInputs = [
              #llvmPackages_17.stdenv
              gcc13Stdenv
              ccache
              cmake
              ninja
              lcov
            ];
            shellHook = ''
              export CC=gcc
              export CXX=g++
            '';
          };
        }
    );
}
