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
  if (argc != 6)
  {
    printf("Give 4 input args\n");
    return -1;
  }
  const int N = atoi(argv[1]);
  const char *filename = argv[2];
  const int nsteps = atoi(argv[3]);
  const double delta_t = atof(argv[4]);
  int graphics = atoi(argv[5]);

  const double e = 1e-3;
  const double G = 100.0 / N;

  /*read the file*/
  double buf[6 * N];
  if (read_doubles_from_file(6 * N, buf, filename) != 0)
  {
    printf("Error reading file '%s'\n", filename);
    return -1;
  }
  /*create an array to contain the results*/
  double result[6 * N];

  for (int t = 0; t < nsteps; t++)
  {
    for (int i = 0; i < N; i++)
    {
      double pos_dx = buf[i * 6 + 0];
      double pos_dy = buf[i * 6 + 1];
      double m = buf[i * 6 + 2];
      double vel_dx = buf[i * 6 + 3];
      double vel_dy = buf[i * 6 + 4];
      double b = buf[i * 6 + 5];

      double sum_x = 0.0;
      double sum_y = 0.0;
      for (int j = 0; j < N; j++)
      {
        if (j == i)
        {
          continue;
        }

        double pos_dx_j = buf[j * 6 + 0];
        double pos_dy_j = buf[j * 6 + 1];
        double m_j = buf[j * 6 + 2];

        double r;
        r = sqrt((pos_dx - pos_dx_j) * (pos_dx - pos_dx_j) + (pos_dy - pos_dy_j) * (pos_dy - pos_dy_j));
        double p = m_j / ((r + e) * (r + e) * (r + e)); //compute and store some value in advance, in order to make it faster
        sum_x += p * (pos_dx - pos_dx_j);
        sum_y += p * (pos_dy - pos_dy_j);
      }

      double a_x = -G * sum_x;
      double a_y = -G * sum_y;
      vel_dx += delta_t * a_x;
      vel_dy += delta_t * a_y;
      pos_dx += delta_t * vel_dx;
      pos_dy += delta_t * vel_dy;

      result[i * 6 + 0] = pos_dx;
      result[i * 6 + 1] = pos_dy;
      result[i * 6 + 2] = m;
      result[i * 6 + 3] = vel_dx;
      result[i * 6 + 4] = vel_dy;
      result[i * 6 + 5] = b;
    }

    /*after each step, the result should be new "initial" value*/
    for (int i = 0; i < 6 * N; i++)
    {
      buf[i] = result[i];
    }
  }

  /*write file*/
  FILE *output_file = fopen("result.gal", "wb");
  if (!output_file)
  {
    printf("Error: failed to open output file\n");
    return -1;
  }
  fwrite(buf, sizeof(char), 6 * N * sizeof(double), output_file);
  fclose(output_file);

  /*graphics part*/
  if (graphics == 1)
  {
    printf("graphic is not available\n");
  }

  return 0;
}