#include "glp.h"
#include "glp_rand.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BENCH_TRIALS 1000

static int compare_doubles(const void *a, const void *b) {
  double da = *(const double *)a;
  double db = *(const double *)b;
  if (da > db)
    return 1;
  if (da < db)
    return -1;
  return 0;
}

static void print_stats(const char *name, double *timings, int n) {
  if (n == 0) {
    printf("No data for %s\n", name);
    return;
  }

  double sum = 0.0;
  for (int i = 0; i < n; i++) {
    sum += timings[i];
  }
  double mean = sum / n;

  qsort(timings, n, sizeof(double), compare_doubles);

  double median;
  if (n % 2 == 0) {
    median = (timings[n / 2 - 1] + timings[n / 2]) / 2.0;
  } else {
    median = timings[n / 2];
  }

  double min = timings[0];
  double max = timings[n - 1];

  double sum_sq_diff = 0.0;
  for (int i = 0; i < n; i++) {
    sum_sq_diff += (timings[i] - mean) * (timings[i] - mean);
  }
  double stddev = 0.0;
  if (n > 1) {
    stddev = sqrt(sum_sq_diff / (n - 1));
  }

  // Mode calculation after sorting is more efficient
  double mode = timings[0];
  int max_count = 1;
  if (n > 0) {
    double current_val = timings[0];
    int current_count = 1;
    for (int i = 1; i < n; i++) {
      if (timings[i] == current_val) {
        current_count++;
      } else {
        if (current_count > max_count) {
          max_count = current_count;
          mode = current_val;
        }
        current_val = timings[i];
        current_count = 1;
      }
    }
    if (current_count > max_count) {
      mode = current_val;
    }
  }

  printf("Stats for %s (%d trials):\n", name, n);
  printf("  Mean:   %f ms\n", mean);
  printf("  Median: %f ms\n", median);
  printf("  Mode:   %f ms\n", mode);
  printf("  StdDev: %f ms\n", stddev);
  printf("  Min:    %f ms\n", min);
  printf("  Max:    %f ms\n", max);
  printf("\n");
}

int main() {
  glp_signing_key_t sk;
  glp_public_key_t pk;
  glp_signature_t sig;
  char *message = "benchmark";
  size_t message_len = strlen(message);
  clock_t start, end;
  int i;

  double timings[BENCH_TRIALS];

  printf("Starting benchmarks (%d trials each)...\n\n", BENCH_TRIALS);

  // Benchmark keygen
  for (i = 0; i < BENCH_TRIALS; i++) {
    start = clock();
    glp_gen_sk(&sk);
    glp_gen_pk(&pk, sk);
    end = clock();
    timings[i] = ((double)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
  }
  print_stats("Key Generation", timings, BENCH_TRIALS);

  // Generate a single key pair for signing and verification benchmarks
  glp_gen_sk(&sk);
  glp_gen_pk(&pk, sk);

  // Benchmark signing
  for (i = 0; i < BENCH_TRIALS; i++) {
    start = clock();
    glp_sign(&sig, sk, (unsigned char *)message, message_len);
    end = clock();
    timings[i] = ((double)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
  }
  print_stats("Signing", timings, BENCH_TRIALS);

  // Generate a single signature for verification
  glp_sign(&sig, sk, (unsigned char *)message, message_len);

  // Benchmark verification
  for (i = 0; i < BENCH_TRIALS; i++) {
    start = clock();
    glp_verify(sig, pk, (unsigned char *)message, message_len);
    end = clock();
    timings[i] = ((double)(end - start)) * 1000.0 / CLOCKS_PER_SEC;
  }
  print_stats("Verification", timings, BENCH_TRIALS);

  return 0;
}
