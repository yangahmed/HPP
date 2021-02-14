#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void print(const int N, int **arr)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

void update(const int N, int **p, int **np)
{
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int s = 0;

            /*count alive neighbours*/
            if (i > 0)
            {
                if (j > 0)
                {
                    if (p[i - 1][j - 1] == 1) s++;
                }
                if (p[i - 1][j] == 1) s++;
                if (j < N - 1)
                {
                    if (p[i - 1][j + 1] == 1) s++;
                }
            }

            if (j > 0)
            {
                if (p[i][j - 1] == 1) s++;
            }
            if (j < N - 1)
            {
                if (p[i][j + 1] == 1) s++;
            }

            if (i < N - 1)
            {
                if (j > 0)
                {
                    if (p[i + 1][j - 1] == 1) s++;
                }
                if (p[i + 1][j] == 1) s++;
                if (j < N - 1)
                {
                    if (p[i + 1][j + 1] == 1) s++;
                }
            }


            /*calculate the status on next generation*/
            if (p[i][j] == 1)
            {
                if (s < 2 || s > 3)
                    np[i][j] = 0;
                else
                    np[i][j] = 1;
            }
            else if (p[i][j] == 0)
            {
                if (s == 3)
                    np[i][j] = 1;
                else
                    np[i][j] = 0;
            }
        }
    }

    /*update p*/
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            p[i][j] = np[i][j];
        }
    }
}


int main(int argc, const char *argv[])
{
    if (argc != 5)
    {
        printf("Give 4 input args:\n");
        printf("N, nsteps, show, nthreads\n");
        return -1;
    }

    const int N = atoi(argv[1]);
    const int nsteps = atoi(argv[2]);
    const int show = atoi(argv[3]);
    const int nthreads = atoi(argv[4]);

    omp_set_num_threads(nthreads);

    /*a number to control the initial status*/
    const double m = 0.7;

    /*create a matrix contain current pattern*/
    int **p = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++)
    {
        p[i] = (int *)malloc(N * sizeof(int));
    }

    /*set initial pattern by random number*/
    srand(time(NULL));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (rand() / (double)RAND_MAX > m)
                p[i][j] = 1;
            else
                p[i][j] = 0;
        }
    }

    if (show == 1)
    {
        printf("initial:\n");
        print(N, p);
        printf("\n");
    }

    /*create another matrix contain the value of next generation*/
    int **np = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++)
    {
        np[i] = (int *)malloc(N * sizeof(int));
    }

    for (int t = 0; t < nsteps; t++)
    {
        update(N, p, np);
    }

    for (int i = 0; i < N; i++)
    {
        free(np[i]);
    }
    free(np);

    if (show == 1)
    {
        printf("result:\n");
        print(N, p);
        printf("\n");
    }

    for (int i = 0; i < N; i++)
    {
        free(p[i]);
    }
    free(p);
}