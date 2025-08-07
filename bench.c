#include "glp.h"
#include "glp_rand.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define BENCH_TRIALS 1000

int main() {
  glp_signing_key_t sk;
  glp_public_key_t pk;
  glp_signature_t sig;
  char *message = "benchmark";
  size_t message_len = strlen(message);
  clock_t start, end;
  double cpu_time_used;
  int i;

  printf("Starting benchmarks (%d trials each)...\n", BENCH_TRIALS);

  // Benchmark keygen
  start = clock();
  for (i = 0; i < BENCH_TRIALS; i++) {
    glp_gen_sk(&sk);
    glp_gen_pk(&pk, sk);
  }
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Key Generation: %f seconds for %d trials (%f ms per keygen)\n",
         cpu_time_used, BENCH_TRIALS, (cpu_time_used * 1000) / BENCH_TRIALS);

  // Generate a single key pair for signing and verification benchmarks
  glp_gen_sk(&sk);
  glp_gen_pk(&pk, sk);

  // Benchmark signing
  start = clock();
  for (i = 0; i < BENCH_TRIALS; i++) {
    glp_sign(&sig, sk, (unsigned char *)message, message_len);
  }
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Signing:        %f seconds for %d trials (%f ms per signature)\n",
         cpu_time_used, BENCH_TRIALS, (cpu_time_used * 1000) / BENCH_TRIALS);

  // Generate a single signature for verification
  glp_sign(&sig, sk, (unsigned char *)message, message_len);

  // Benchmark verification
  start = clock();
  for (i = 0; i < BENCH_TRIALS; i++) {
    glp_verify(sig, pk, (unsigned char *)message, message_len);
  }
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Verification:   %f seconds for %d trials (%f ms per verification)\n",
         cpu_time_used, BENCH_TRIALS, (cpu_time_used * 1000) / BENCH_TRIALS);

  return 0;
}
