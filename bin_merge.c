/*
 * Usage:
 *  bin_merge <file_list> <output_file_name>
 * Description:
 *  file_list:
 *   a list of file to merge
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LIN_LEN 80
#define BUF_LEN 8192
int file_exist(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (f) {
    fclose(f);
    return 1;
  }
  fclose(f);
  return 0;
}
int main(int argc, char *argv[]) {
  if(file_exist(argv[2])) {
    printf("file exist!\n");
    return 1;
  }
  FILE *outfile = fopen(argv[2], "wb");
  FILE *flist = fopen(argv[1], "r");
  FILE *slice;
  char line[LIN_LEN] = {0}, buf[BUF_LEN];
  while (fgets(line, LIN_LEN, flist)) {
    if (line[strlen(line) - 1] == '\n')
      line[strlen(line) - 1] = '\0';
    //printf("open: %s\n", line);
    slice = fopen(line, "rb");
    if (!slice) {
      fclose(slice);
      printf("file '%s' not exist.\n", line);
      return 1;
    }
    int rc;
    long long count = 0;
    do {
      rc = fread(buf, sizeof(char), BUF_LEN, slice);
      count += rc;
      fwrite(buf, sizeof(char), rc, outfile);
    } while (!feof(slice));
    if (slice) fclose(slice);
    printf("%s\t%I64d\n", line, count);
  }
  if (outfile) fclose(outfile);
  if (flist) fclose(flist);
  return 0;
}
