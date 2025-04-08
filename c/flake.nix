{
  description = "(m)ini (sc)he(m)e build flake";

  inputs = {
    nixpkgs = {
      url = "github:nixos/nixpkgs/nixos-unstable";
    };
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
  };
  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        packages = rec {
          mscm = pkgs.stdenv.mkDerivation {
            pname = "mscm";
            version = "0.1.0";
            src = ./.;

            nativeBuildInputs = [
              pkgs.gcc
            ];

            buildInputs = [
              pkgs.readline
            ];

            buildPhase = ''
              make
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp build/mscm $out/bin
            '';

            meta = with pkgs.lib; {
              description = "a (m)ini (sc)he(m)e interpreter";
              license = licenses.mit;
              platforms = platforms.all;
            };
          };
          default = mscm;
        };
        # devShell = import ./shell.nix { inherit pkgs; };
      }
    );
}
