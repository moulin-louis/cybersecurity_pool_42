#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void no(void) {
  puts("Nope.");
  exit(1);
}

void ok(void) {
  puts("Good job.");
}

int main(void) {
  char input [24];
  char result [9];
  char tmp[4];
  uint i = 2;
  int j = 1;

  printf("Please enter key: ");
  int ret_value = scanf("%23s",input);
  if (ret_value != 1 || input[0] != '0' || input[1] != '0')
    no();
  fflush(stdin);
  memset(result,0,9);
  result[0] = 'd';
  while( true ) {
    size_t len = strlen(result);
    bool stop_status = false;
    if (len < 8) {
      len = strlen(input);
      stop_status = i < len;
    }
    if (!stop_status) break;
    tmp[0] = input[i];
    tmp[1] = input[i + 1];
    tmp[2] = input[i + 2];
    tmp[3] = 0;
    int result_atoi = atoi(tmp);
    result[j] = (char)result_atoi;
    i = i + 3;
    j = j + 1;
  }
  result[j] = '\0';
  ret_value = strcmp(result,"delabere");
  if (ret_value == 0)
    ok();
  else
    no();
  return 0;
}