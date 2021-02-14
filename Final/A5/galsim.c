#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "quadtree.h"

pthread_mutex_t mutexbuf;

struct thread_data
{
  double** buf;
  double theta_max;
  double G;
  double delta_t;
  int N;
  int i;
  int p;
  treenode* tree;
};


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


const double e = 1e-3;


void cal_force(double x, double y, const double theta_max, treenode *node, double* sum_x, double* sum_y){
  /*calculate the force to a partical by BH algorithm, with a given theta_max*/

  double x_j, y_j, m_j;
  x_j = node->m_x;
  y_j = node->m_y;
  m_j = node->m;
  //here m_x, m_y and m mean position and mass for a partical; while center of mass and total mass for a box
  
  double r = sqrt((x-x_j)*(x-x_j)+(y-y_j)*(y-y_j));
  if(node->isLeaf == 1) {
    //leaf node means a partical
    if(r != 0) {
      double p = m_j / ((r + e) * (r + e) * (r + e));
      *sum_x += p*(x-x_j);
      *sum_y += p*(y-y_j);
    }
  }

  else if(node->isLeaf == 0) {
    //not a leaf node, so it is a box
    //compute theta at first, determine whether it can be treated as one object
    double theta = node->width / r;
    if(theta <= theta_max) {
      //if so, calculate it as a "particle"
      double p = m_j / ((r + e) * (r + e) * (r + e));
      *sum_x += p*(x-x_j);
      *sum_y += p*(y-y_j);
    }
    else {
      //if not, go to lower level
      if(node->ne)
        cal_force(x, y, theta_max, node->ne, sum_x, sum_y);
      if(node->nw)
        cal_force(x, y, theta_max, node->nw, sum_x, sum_y);
      if(node->sw)
        cal_force(x, y, theta_max, node->sw, sum_x, sum_y);
      if(node->se)
        cal_force(x, y, theta_max, node->se, sum_x, sum_y);
    }
  }
}


void *calculation(void *arg){
  /*calculate and store the new position and new velocity of a partical*/
  struct thread_data *data;
  data = (struct thread_data *)arg;
  const int i = data->i;
  const int p = data->p;
  const int N = data->N;
  double* buf = *(data->buf);
  const double theta_max = data->theta_max;
  const double G = data->G;
  const double delta_t = data->delta_t;
  treenode* tree = data->tree;
  

  for(int j = i; j < i+p; j++){
    if(j >= N) break;

    double pos_dx, pos_dy, vel_dx, vel_dy;
    pos_dx = buf[j * 6 + 0];
    pos_dy = buf[j * 6 + 1];
    vel_dx = buf[j * 6 + 3];
    vel_dy = buf[j * 6 + 4];
    double *sum_x = (double *)malloc(sizeof(double));
    double *sum_y = (double *)malloc(sizeof(double));
    *sum_x = *sum_y = 0.0;

    cal_force(pos_dx, pos_dy, theta_max, tree, sum_x, sum_y);

    double a_x, a_y;
    a_x = -G * (*sum_x);
    a_y = -G * (*sum_y);
    vel_dx += delta_t * a_x;
    vel_dy += delta_t * a_y;
    pos_dx += delta_t * vel_dx;
    pos_dy += delta_t * vel_dy;

    pthread_mutex_lock(&mutexbuf);
    buf[j * 6 + 0] = pos_dx;
    buf[j * 6 + 1] = pos_dy;
    buf[j * 6 + 3] = vel_dx;
    buf[j * 6 + 4] = vel_dy;
    pthread_mutex_unlock(&mutexbuf);
    
    free(sum_x);
    free(sum_y);
  }
  free(data);
  pthread_exit(NULL);
}


int main(int argc, const char *argv[])
{
  if (argc != 8)
  {
    printf("Give 7 input args\n");
    return -1;
  }
  const int N = atoi(argv[1]);
  const char *filename = argv[2];
  const int nsteps = atoi(argv[3]);
  const double delta_t = atof(argv[4]);
  const double theta_max = atof(argv[5]);
  int graphics = atoi(argv[6]);
  const int n_threads = atoi(argv[7]);
  const double G = 100.0 / N;

  pthread_t thread[n_threads];
  pthread_attr_t attr;
  void *status;
  
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&mutexbuf, NULL);

  /*read the file*/
  double *buf = (double *)malloc(6*N*sizeof(double));
  if (read_doubles_from_file(6 * N, buf, filename) != 0)
  {
    printf("Error reading file '%s'\n", filename);
    return -1;
  }
  /*create an array to contain the results*/

  for (int t = 0; t < nsteps; t++)
  {
    treenode* tree = NULL;
    for(int i = 0; i < N; i++) {
      insert(&tree, buf[i*6+0], buf[i*6+1], buf[i*6+2]);
    }

    int i = 0;
    const int p = N/n_threads + 1;
   
    for (int thr = 0; thr < n_threads; thr++){
      struct thread_data *data = (struct thread_data *)malloc(sizeof(struct thread_data));
      data->buf = &buf;
      data->N = N;
      data->p = p;
      data->theta_max = theta_max;
      data->G = G;
      data->delta_t = delta_t;
      data->tree = tree;
      data->i = i;
      pthread_create(&thread[thr], &attr, calculation, (void *)data);

      i += p;

    }

    for (int thr = 0; thr < n_threads; thr++) {
      pthread_join(thread[thr], &status);
    }
    

    free_tree(tree);
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

  free(buf);
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mutexbuf);

  /*graphics part*/
  if (graphics == 1)
  {
    printf("graphic is not available\n");
  }

  return 0;
}
