/*
 * Usage:
 *  bin_split <block_list|block_size|block_num> <src_file>
 *
 * Example:
 *  split by a file, the file format adopted by
 *  bin_merge's output format, which is:
 *    FILE_PART_NAME_1\tFILE_SIZE_IN_BYTES
 *    FILE_PART_NAME_2\tFILE_SIZE_IN_BYTES
 *    ...
 *
 *  split by size, each file size 1KB
 *    > bin_split 1KB temp.bin
 *
 *  split one file to 10 files
 *    > bin_split 10 temp.bin
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FNAME_LEN 250 /* file name len */
#define BNAME_LEN 260 /* block file name len */
#define BUF_LEN 8192
#define UNIT_KB 1024
#define UNIT_MB 1048576
#define UNIT_GB 10737418240
#define LIN_LEN 80
int file_exist(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (f) {
    fclose(f);
    return 1;
  }
  fclose(f);
  return 0;
}
long long hdata_to_bytes(char *hdata) {
  long num;
  char unit[3];
  sscanf(hdata, "%li%s", &num, unit);
  printf("num:%ld, unit:%s\n", num, unit);
  if (!strcmp("B", unit))
    return num;
  else if (!strcmp("KB", unit))
    return num * UNIT_KB;
  else if (!strcmp("MB", unit))
    return num * UNIT_MB;
  else if (!strcmp("GB", unit))
    return num * UNIT_GB;
  if (!*unit)
    return -1; // no unit
  else 
    return -2; // invalid unit
}

int last_index_of(const char *str, char ch) {
  int slen = strlen(str);
  while (slen--)
    if (ch == *(str + slen))
      return slen;
  return slen;
}

void split_n(
    FILE *f,
    const char* fname,
    int bc,
    long long blen,
    long long flen
) {
  FILE *block;
  for (int i = 0; i < bc; ++i) {

    char filename[FNAME_LEN] = {0};
    char block_name[BNAME_LEN] = {0};
    int dot = last_index_of(fname, '.');
    strncpy(filename, fname, dot);
    sprintf(block_name, "%s-%d%s", filename, i,
        (dot >= strlen(fname)) ?  "" :  fname + dot);
    printf("%s\n", block_name);
    block = fopen(block_name, "wb");

    int offset = i * blen;
    fseek(f, offset, SEEK_SET);
    if (i == bc - 1)
      blen = flen - blen * (bc - 1);
    long long count = 0;
    int rc; // read count
    char buf[BUF_LEN];
    do {
       rc = fread(buf, sizeof(char), BUF_LEN, f);
       count += rc;
       if (count >= blen) {
         fwrite(buf, sizeof(char), rc - (count - blen), block);
         break;
       }
       fwrite(buf, sizeof(char), rc, block);
    } while(!feof(f));
  }
  fclose(block);
}

void split_b(FILE *f, const char *fname, long long bpb) {
  FILE *block;
  int fnum = 0;
  long long count = 0;
  int rc;
  char buf[BUF_LEN];

  char filename[FNAME_LEN] = {0};
  char block_name[BNAME_LEN] = {0};
  int dot = last_index_of(fname, '.');
  strncpy(filename, fname, dot);

  sprintf(block_name, "%s-%d%s", filename, fnum,
      (dot >= strlen(fname)) ?  "" : fname + dot);
  block = fopen(block_name, "wb");
  printf("%s\n", block_name);
  fnum++;

  do {
    rc = fread(buf, sizeof(char), BUF_LEN, f);
    count += rc;
    if (count >= bpb) {
      fwrite(buf, sizeof(char), rc - (count - bpb), block);
      fseek(f, ftell(f) - (count - bpb), SEEK_SET);
      count = 0;
      if (block) fclose(block);
      sprintf(block_name, "%s-%d%s", filename, fnum,
          (dot >= strlen(fname)) ?  "" : fname + dot);
      block = fopen(block_name, "wb");
      printf("%s\n", block_name);
      fnum++;
      continue;
    }
    fwrite(buf, sizeof(char), rc, block);
  } while(!feof(f));
  if (block) fclose(block);
}

void split_f(FILE *f, const char* list_file) {
  FILE* list = fopen(list_file, "r");
  char line[LIN_LEN];
  char file_name[LIN_LEN];
  long long file_size = 0;
  while (fgets(line, LIN_LEN, list)) {
    int tab = last_index_of(line, '\t');
    strncpy(file_name, line, tab);
    file_name[tab] = '\0';
    file_size = atoll(line + tab + 1);
    if (file_exist(file_name)) {
      printf("file exist: %s\n", file_name);
      break;
    }
    FILE *b = fopen(file_name, "wb");
    int rc;
    long long count = 0;
    char buf[BUF_LEN];
    do {
      rc = fread(buf, sizeof(char), BUF_LEN, f);
      count += rc;
      if (count >= file_size) {
        fwrite(buf, sizeof(char), rc - (count - file_size), b);
        fseek(f, ftell(f) - (count - file_size), SEEK_SET);
        printf("%s, %I64d...OK\n", file_name, file_size);
        break;
      }
      fwrite(buf, sizeof(char), rc, b);
    } while(!feof(f));
    fclose(b);
  }
  fclose(list);
}

int main(int argc, char *argv[]) {
  int bc = atoi(argv[1]); // block count
  long long bpb = hdata_to_bytes(argv[1]); // bytes per block

  FILE *f = fopen(argv[2], "rb");
  if (!f) {
    fclose(f);
    printf("file not exist: %s\n", argv[2]);
    return 1;
  }
  fseek(f, 0, SEEK_END);
  long long flen = ftell(f); // total size

  if (bpb > 0) {
    bc = 1;
    while (bpb * bc < flen) bc++;
  }

  printf("block count: %d, bytes per block: %I64i\n", bc, bpb);

  fseek(f, 0, SEEK_SET);

  if (file_exist(argv[1])) {
    split_f(f, argv[1]);
  } else if (bpb < 0) {
    long long blen = flen / bc;
    // split to N files
    split_n(f, argv[2], bc, blen, flen);
  } else {
    // split N bytes per file
    split_b(f, argv[2], bpb);
  }
  fclose(f);
  return 0;
}
