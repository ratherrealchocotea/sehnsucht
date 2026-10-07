{
  description = "Programming as an art workshop for retarded children";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      crossPkgs = pkgs.pkgsCross.x86_64-embedded;
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        packages = [
          crossPkgs.buildPackages.gcc
          crossPkgs.buildPackages.binutils
            pkgs.limine-full
            pkgs.mtools
            pkgs.dosfstools
            pkgs.qemu
            pkgs.clang-tools
            pkgs.git
            pkgs.gnumake
			pkgs.doxygen
			pkgs.parted
        ];
    };
  };
}
