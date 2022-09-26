
with import <nixpkgs> {};
gcc11Stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [
    vulkan-headers
    cmake
    gcc11
    mold
  ];

  buildInputs = [
    gdb
    clang_14
    vulkan-loader
    vulkan-tools
    vulkan-validation-layers
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
    xorg.libXext.dev
  ];

  shellHook = ''
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${lib.strings.makeLibraryPath [ libGL vulkan-loader libxkbcommon wayland xorg.libX11 ]}
  '';
}
