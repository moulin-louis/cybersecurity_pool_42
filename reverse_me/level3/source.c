#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void no(void) {
  puts("Nope.");
  exit(1);
}

void yes(void) {
  puts("Good Job.");
}

int main(void) {
  char tmp[4];
  char input[31];
  char result[9];
  size_t j = 2;
  int i = 1;

  printf("Please enter key: ");
  int ret_value = scanf("%23s", input);
  if (ret_value != 1 || input[0] != '4' || input[1] != '2')
    no();
  fflush(stdin);
  memset(result, 0, 9);
  result[0] = '*';
  while (true) {
    size_t len = strlen(result);
    bool stop_status = false;
    if (len < 8) {
      len = strlen(input);
      stop_status = j < len;
    }
    if (!stop_status) break;
    tmp[0] = input[j];
    tmp[1] = input[j + 1];
    tmp[2] = input[j + 2];
    tmp[3] = 0;
    int ret_atoi = atoi(tmp);
    result[i] = (char) ret_atoi;
    j = j + 3;
    i = i + 1;
  }
  result[i] = '\0';
  int ret_result = strcmp(result, "********");
  if (ret_result == 0)
    yes();
  else
    no();
  return 0;
}