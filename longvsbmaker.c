#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  unsigned int i;
  FILE *end;
  
  if (argc != 2) {
    printf("usage: ./lvm <file name>\n");
    return (-1);
  }
  end = fopen(argv[1], "w");
  for (i = 0; i < 65536; i++)
    if (i < 65535)
      fprintf(end, "%04x adr1 %04x %04x\n", i, i, i+1);
    else
      fprintf(end, "%04x out1 %04x %04x\n", i, i, 0);
  fclose(end);
}
