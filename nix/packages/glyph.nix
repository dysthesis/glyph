{pkgs, ...}: let
  nixpkgs-src = pkgs.fetchFromGitHub {
    owner = "NixOS";
    repo = "nixpkgs";
    rev = "c5d56965afd6bee27e1205c65daae6c4386d0e96";
    hash = "sha256-dCFI666lrtxcV58E5uX3Q/z+2Q+jTyNcaTC8TKACLVE=";
  };

  pkgs' = import nixpkgs-src {inherit (pkgs) system;};
in
  pkgs'.stdenv.mkDerivation {
    name = "glyph";
    src = ../../.;

    buildInputs = with pkgs'; [openssl];

    CCFLAGS = "-O3 -Wall -Wextra -std=gnu99 -pedantic -Wfatal-errors -funroll-all-loops";

    buildPhase = ''
      runHook preBuild

      compile() {
        ${pkgs'.stdenv.cc.targetPrefix}cc $CCFLAGS -DGLP_N=1024 -c "$1" -o "$2"
      }

      mkdir -p FFT

      compile glp.c                      glp_1024.o
      compile glp_utils.c                glp_utils_1024.o
      compile glp_rand.c                 glp_rand_1024.o
      compile glp_rand_openssl_aes.c     glp_rand_openssl_aes_1024.o
      compile FFT/FFT_1024_59393.c       FFT/FFT_1024_59393.o
      compile bench.c                    bench.o

      ${pkgs'.stdenv.cc.targetPrefix}cc \
        $CCFLAGS -DGLP_N=1024 -o bench_1024 \
        glp_1024.o glp_utils_1024.o \
        glp_rand_1024.o glp_rand_openssl_aes_1024.o \
        FFT/FFT_1024_59393.o bench.o \
        -lm -lcrypto

      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out/bin
      cp bench_1024 $out/bin/bench
      runHook postInstall
    '';

    meta = with pkgs'.lib; {
      description = "GLP 1024 benchmark executable (pure C, OpenSSL crypto RNG)";
      mainProgram = "bench";
      platforms = platforms.unix;
    };
  }
