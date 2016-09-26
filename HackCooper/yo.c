#include <stdio.h>

int main() {
  int i;
  char word[100];
  for (i = 0; i < 15000; i++) {
    scanf("%s\n", word);
    printf("%s\n", word);
    //lol
  }
  return 0;
}
