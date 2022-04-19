let
  pkgs = import <nixos> {};
in pkgs.mkShell {
  nativeBuildInputs = [
    pkgs.vulkan-headers
  ];

  buildInputs = [
    pkgs.gdb
    pkgs.clang
    pkgs.cmake
    pkgs.gcc11
    pkgs.vulkan-loader
    pkgs.vulkan-tools
    pkgs.vulkan-validation-layers
    pkgs.spirv-tools
    pkgs.shaderc
    pkgs.wayland
    pkgs.wayland-protocols
    pkgs.libxkbcommon
    pkgs.extra-cmake-modules
    pkgs.libGL
  ];

  shellHook = ''
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(nix eval --raw nixos.libGL)/lib:$(nix eval --raw nixos.vulkan-loader)/lib:$(nix eval --raw nixos.libxkbcommon)/lib:$(nix eval --raw nixos.wayland)/lib
  '';
}
