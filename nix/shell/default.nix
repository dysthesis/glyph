{pkgs, ...}:
pkgs.mkShell {
  name = "glyph";

  packages = with pkgs; [
    nixd
    alejandra
    statix
    deadnix
    clang
    clang-tools
  ];
}
