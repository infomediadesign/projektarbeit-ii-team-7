
with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [
    (enableDebugging vulkan-headers)
    cmake
  ];

  buildInputs = [
    gdb
    clang
    gcc11
    (enableDebugging vulkan-loader)
    vulkan-tools
    (enableDebugging vulkan-validation-layers)
    spirv-tools
    shaderc
    wayland
    wayland-protocols
    libxkbcommon
    extra-cmake-modules
    libGL
  ];

  shellHook = ''
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(nix eval --raw nixos.libGL)/lib:$(nix eval --raw nixos.vulkan-loader)/lib:$(nix eval --raw nixos.libxkbcommon)/lib:$(nix eval --raw nixos.wayland)/lib
  '';
}
