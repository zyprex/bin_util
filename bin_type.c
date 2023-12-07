/*
 * Usage:
 *  bin_type <test_file_name1> <test_file_name2> ...
 *  can add more file type to 'types.txt'
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define READ_LEN 6
#define HEAD_LEN 12
#define CHECK_LEN 4096

void is_text(char *fname) {
  FILE *f = fopen(fname, "rb");
  char buf[CHECK_LEN];
  int rc = fread(buf, sizeof(char), CHECK_LEN, f);
  fclose(f);
  int black_list = 0, white_list = 0;
  for (int i = 0; i < rc; ++i) {
     if (buf[i] == 9 || buf[i] == 10 || buf[i] == 13 ||
         (buf[i] >=32 && buf[i] <= 255)) {
       white_list++;
     }
     if ((buf[i] >= 0 && buf[i] <= 6) ||
         (buf[i] >= 14 && buf[i] <= 31)) {
       black_list++;
     }
  }
  if (white_list > 0 && black_list == 0) {
    printf("type:text\n");
  } 
  else {
    printf("type:\n");
  }
}

char* str_match_after(char *src, char *s, int offset) {
  char* pos = strstr(src, s);
  if (pos) {
    printf("type:");
    char ch;
    do {
      ch = *(pos + offset + 1);
      putchar(ch);
      offset++;
    } while (isalnum(ch) || ch == ',');
    putchar(10);
  }
  return pos;
}
int main(int argc, char *argv[]) {

  FILE *dic = fopen("types.txt", "r");
  fseek(dic, 0L, SEEK_END);
  long dic_len = ftell(dic);
  fseek(dic, 0L, SEEK_SET);
  char *text = (char*)malloc(dic_len*sizeof(char));
  fread(text, sizeof(char), dic_len, dic);
  fclose(dic);


  if (argc > 0) {
    for (int i = 1; i < argc; ++i) {

      FILE *f = fopen(argv[i], "rb");
      unsigned char tmp[READ_LEN]; // <- don't use signed char
      fread(tmp, sizeof(char), READ_LEN, f);
      fclose(f);
      char head[HEAD_LEN] = {0}, hx[3] = {0};
      for (int i = 0; i < READ_LEN; ++i) {
        sprintf(hx, "%02x", tmp[i]);
        strcat(head, hx);
        if (strlen(head) == HEAD_LEN) break;
      }

      printf("file:%s\nhead:%s\n", argv[i], head);
      if(!str_match_after(text, head, strlen(head))) {
        is_text(argv[i]);
      }
      if (i != argc - 1)
        putchar(10);
    }

  }
  free(text);

  return 0;
}
