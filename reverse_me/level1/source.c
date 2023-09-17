#include <stdio.h>
#include <string.h>

int main(void) {
  char str[] = "__stack_check";
  char input[100];
  printf("Please enter key: ");
  scanf("%s", input);
  int return_value = strcmp(str, input);
  if (return_value == 0){
    printf("Good job.\n");
  } else {
    printf("Nope.\n");
  }
  return (0);
}