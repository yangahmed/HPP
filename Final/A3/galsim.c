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
    printf("Give 4 input args: \nN filename nsteps delta_t graphics\n");
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

  for (int t = 0; t < nsteps; t++)
  {
    double* sum_x = (double *)calloc(N, sizeof(double));
    double* sum_y = (double *)calloc(N, sizeof(double));

    for (int i = 0; i < N; i++)
    {
      double pos_dx, pos_dy, m, vel_dx, vel_dy;
      pos_dx = buf[i * 6 + 0];
      pos_dy = buf[i * 6 + 1];
      m = buf[i * 6 + 2];
      vel_dx = buf[i * 6 + 3];
      vel_dy = buf[i * 6 + 4];

      for (int j = i + 1; j < N; j++)
      {
        double pos_dx_j, pos_dy_j, m_j, r;
        pos_dx_j = buf[j * 6 + 0];
        pos_dy_j = buf[j * 6 + 1];
        m_j = buf[j * 6 + 2];

        r = sqrt((pos_dx - pos_dx_j) * (pos_dx - pos_dx_j) + (pos_dy - pos_dy_j) * (pos_dy - pos_dy_j));
        
        double p, force_x, force_y;
        p = (m * m_j) / ((r + e) * (r + e) * (r + e)); //compute and store some value in advance, in order to make it faster
        force_x = p * (pos_dx - pos_dx_j);
        force_y = p * (pos_dy - pos_dy_j);

        //use symmetry due to the 3rd Newton's law
        sum_x[i] += force_x;
        sum_y[i] += force_y;
        sum_x[j] += -force_x;
        sum_y[j] += -force_y;
      }

      double a_x, a_y;
      a_x = (-G * sum_x[i])/m;
      a_y = (-G * sum_y[i])/m;
      vel_dx += delta_t * a_x;
      vel_dy += delta_t * a_y;
      pos_dx += delta_t * vel_dx;
      pos_dy += delta_t * vel_dy;

      buf[i * 6 + 0] = pos_dx;
      buf[i * 6 + 1] = pos_dy;
      buf[i * 6 + 3] = vel_dx;
      buf[i * 6 + 4] = vel_dy;
    }

    free(sum_x);
    free(sum_y);
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