/*
 * Usage:
 *   bin_cmp <file1> <file2>
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_LEN 8192
int main(int argc, char *argv[]) {
  FILE* f1 = fopen(argv[1], "rb");
  FILE* f2 = fopen(argv[2], "rb");
  if (!f1 || !f2) {
    fclose(f1);
    fclose(f2);
    printf("file not exist.\n");
    return 1;
  }
  char b1[BUF_LEN], b2[BUF_LEN];
  do {
    fread(b1, sizeof(char), BUF_LEN, f1);
    fread(b2, sizeof(char), BUF_LEN, f2);
    if (memcmp(b1, b2, BUF_LEN)) {
      printf("bin different.\n");
      break;
    }
  } while (!feof(f1) || !feof(f2));
  fclose(f1);
  fclose(f2);
  return 0;
}

