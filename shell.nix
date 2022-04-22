
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
    libffi
    renderdoc
    xorg.libX11
    xorg.libX11.dev
    xorg.libXrandr.dev
    xorg.libXinerama.dev
    xorg.libXcursor.dev
    xorg.libXi.dev
    xorg.libXi.dev
    xlibs.libXext.dev
  ];

  shellHook = ''
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(nix eval --raw nixos.libGL)/lib:$(nix eval --raw nixos.vulkan-loader)/lib:$(nix eval --raw nixos.libxkbcommon)/lib:$(nix eval --raw nixos.wayland)/lib:$(nix eval --raw nixos.xorg.libX11)/lib
  '';
}
