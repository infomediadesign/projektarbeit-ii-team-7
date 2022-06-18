
with import <nixpkgs> {};
gcc11Stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [
    clang_14
  ];

  buildInputs = [
    gdb
    rustc
    cargo
  ];
}
