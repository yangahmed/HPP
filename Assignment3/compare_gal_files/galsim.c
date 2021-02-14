#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
  return seconds;
}

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

void cal(int N,double *buf, double deltaT, int nsteps){
		double temp[6*N], P1_X, P1_Y,P1_M, P1_VX, P1_VY,P1_B,P2_X, P2_Y,P2_M; 
		double Xcomp,Ycomp,accX,accY,r,p,e,G;
		Xcomp=r=p=Ycomp=accX=G=accY=0;
		e=1e-3;G=100.0/N;
	for (int t = 0; t < nsteps; t++)
    {     
			
		for(int i=0;i<N;i++)
		{
			  P1_X= buf[i * 6 + 0];
			  P1_Y= buf[i * 6 + 1];
			  P1_M= buf[i * 6 + 2];
			  P1_VX= buf[i * 6 + 3];
			  P1_VY= buf[i * 6 + 4];
			  P1_B=buf[i*6+5];
				Xcomp = 0;
			Ycomp = 0;
			for(int j=0;j<N;j++){
				if(i!=j){
                           	P2_X=buf[j*6+0];
							P2_Y=buf[j*6+1];
							P2_M=buf[j*6+2];
							r=sqrt(pow((P1_X - P2_X),2)+pow((P1_Y - P2_Y),2));
							p=P2_M/pow((r+e),3);
							Xcomp+=p* (P1_X-P2_X); 
							Ycomp+=p*(P1_Y-P2_Y);					
				}
                accX = -G*Xcomp;
				accY = -G*Ycomp;
			    P1_VX+=deltaT*accX;
				P1_VY+=deltaT*accY;
				P1_X+=deltaT*P1_VX;
				P1_Y+=deltaT*P1_VY;
			}
			temp[i*6+0]=P1_X;
			temp[i*6+1]=P1_Y;
			temp[i*6+2]=P1_M;
			temp[i*6+3]=P1_VX;
			temp[i*6+4]=P1_VY;
			temp[i*6+5]=P1_B;
		}
		for(int i=0;i<(6*N);i++)
		{
			buf[i]=temp[i];
		}
	}
}

int main(int argc, const char* argv[]) {	
	  if(argc != 6) {
		printf("Give  input args:N filename steps delta_t graphics\n");
		return -1;
	  }
	  int N = atoi(argv[1]);
	  double p=0;
	  const char* fileName1 = argv[2];
	  int nsteps=atoi(argv[3]);
	  int delta_t=atoi(argv[4]);
	  int graphics=atoi(argv[5]);
	  FILE *f=fopen(fileName1,"rb");
	  double buf[6 * N];
	  //reading data from file
	  if (read_doubles_from_file(6 *N, buf, fileName1) != 0)
		  {
			printf("Error reading file '%s'\n", fileName1);
			return -1;
		  }
	  double t1 = get_wall_seconds();
	  cal(N,buf,delta_t,nsteps);
 	  double t2 = get_wall_seconds();
	  printf("Total time taken=%f\n",t2-t1);
	  FILE *output_file = fopen("result.gal", "wb");
	  if (!output_file)
		  {
			printf("Error: failed to open output file\n");
			return -1;
		  }
	  fwrite(buf, sizeof(char), 6 * N * sizeof(double), output_file);
	  fclose(output_file);	  
	  return 0;
}