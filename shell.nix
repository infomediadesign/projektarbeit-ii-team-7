let
  pkgs = import <nixos> {};
in pkgs.mkShell {
  buildInputs = [
    pkgs.clang
    pkgs.cmake
    pkgs.gcc
    pkgs.raylib
  ];

  shellHook = ''
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(nix eval --raw nixos.raylib)/lib
  '';
}
