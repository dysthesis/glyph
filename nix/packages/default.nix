{
  pkgs,
  lib,
  inputs,
  ...
}: rec {
  default = glyph;
  glyph = pkgs.callPackage ./glyph.nix {inherit pkgs inputs lib;};
}
