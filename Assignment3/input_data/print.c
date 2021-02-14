#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int read_doubles_from_file(int n, double *p, const char *fileName)
{
  /* Open input file and determine its size. */
  FILE *input_file = fopen(fileName, "rb");
  if (!input_file)
  {
    printf("read_doubles_from_file error: failed to open input file '%s'.\n", fileName);
    return -1;
  }
  /* Get filesize using fseek() and ftell(). */
  fseek(input_file, 0L, SEEK_END);
  size_t fileSize = ftell(input_file);
  /* Now use fseek() again to set file position back to beginning of the file. */
  fseek(input_file, 0L, SEEK_SET);
  if (fileSize != n * sizeof(double))
  {
    printf("read_doubles_from_file error: size of input file '%s' does not match the given n.\n", fileName);
    printf("For n = %d the file size is expected to be (n * sizeof(double)) = %lu but the actual file size is %lu.\n",
           n, n * sizeof(double), fileSize);
    return -1;
  }
  /* Read contents of input_file into buffer. */
  size_t noOfItemsRead = fread(p, sizeof(char), fileSize, input_file);
  if (noOfItemsRead != fileSize)
  {
    printf("read_doubles_from_file error: failed to read file contents into buffer.\n");
    return -1;
  }
  /* OK, now we have the file contents in the buffer.
     Since we are finished with the input file, we can close it now. */
  if (fclose(input_file) != 0)
  {
    printf("read_doubles_from_file error: error closing input file.\n");
    return -1;
  }
  /* Return 0 to indicate success. */
  return 0;
}

int main(int argc, const char *argv[])
{
  const int N = atoi(argv[1]);
  const char *filename = argv[2];

  /*read the file*/
  double buf[6 * N];
  if (read_doubles_from_file(6 * N, buf, filename) != 0)
  {
    printf("Error reading file '%s'\n", filename);
    return -1;
  }

  for(int i = 0; i < N; i++) {
      printf("%f\t", buf[i*6+0]);
      printf("%f\n", buf[i*6+1]);
  }
}