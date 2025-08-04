{pkgs ? import <nixpkgs> {}, ...}:
pkgs.stdenv.mkDerivation {
  pname = "glp-1024-test";
  version = "0.1";
  src = ../../.;

  buildInputs = [pkgs.openssl];

  CCFLAGS = "-O3 -Wall -Wextra -std=gnu99 -pedantic -Wfatal-errors -funroll-all-loops";

  buildPhase = ''
    runHook preBuild

    compile() {
      ${pkgs.stdenv.cc.targetPrefix}cc $CCFLAGS -DGLP_N=1024 -c "$1" -o "$2"
    }

    compile glp.c                       glp_1024.o
    compile glp_utils.c                 glp_utils_1024.o
    compile glp_rand.c                  glp_rand_1024.o
    compile glp_rand_openssl_aes.c      glp_rand_openssl_aes_1024.o
    compile test_vectors.c              glp_test_vectors_1024.o

    ${pkgs.stdenv.cc.targetPrefix}cc \
      $CCFLAGS -DGLP_N=1024 \
      -o test_1024 \
      glp_test_vectors_1024.o glp_1024.o glp_utils_1024.o \
      glp_rand_1024.o glp_rand_openssl_aes_1024.o \
      FFT/FFT_1024_59393.o \
      -lm -lcrypto

    runHook postBuild
  '';

  installPhase = ''
    runHook preInstall
    mkdir -p $out/bin
    cp test_1024 $out/bin/
    runHook postInstall
  '';

  meta = with pkgs.lib; {
    description = "GLP 1024 test executable (pure C, OpenSSL crypto RNG)";
    platforms = platforms.unix;
  };
}
