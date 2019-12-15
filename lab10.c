#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

float globalSum=0.0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* threadCode(void *argument)
{
  float threadSum=0;
  float *array=(float *)argument;
  int arraySize = sizeof(array)/sizeof(float);
  pthread_t currentThreadID = pthread_self();

  printf("Thread #%ld arraySize=%d\n",currentThreadID,arraySize);

  for(int i=0;i<arraySize;i++)
    threadSum+=array[i];
  //printf("In threadCode:\targument:%p\n",argument);
  //printf("In threadCode:\tarray:%p\n",array);
  printf("Thread #%ld threadSum=%f\n",currentThreadID,threadSum);

  pthread_mutex_lock(&mutex);
  globalSum+=threadSum;
  pthread_mutex_unlock(&mutex);

  return 0;
}

int main(int argc, char *argv[]){
  if(argc>3){
    fprintf(stderr, "Too many arguments!\n");
    return 1;
  }
  if(argc==1)
  {
    fprintf(stderr,"Too few arguments!\n");
    return 1;
  }
  /*if( arg1-(int)arg1 != 0 ||  arg2-(int)arg2 != 0 )
  {
    fprintf(stderr,"Arguments have to be integers!\n");
    return 1;
  }*/
  int noOfData = atoi(argv[1]); //double arg1 = atof(argv[1]); //n
  int threads = atoi(argv[2]); //double arg2 = atof(argv[2]); //w
  //printf("%0.1f %0.1f\n",arg1,arg2);

  if(threads>=noOfData)
  {
    fprintf(stderr, "Number of threads have to be less than number of data!\n");
    return 1;
  }

  float *data = malloc(sizeof(float)*noOfData);

  if(!data)
  {
    fprintf(stderr,"Memory allocation failed.");
    return 1;
  }

  srand(0);
  for(int i=0;i<noOfData;i++)
    data[i]=1000.*rand()/RAND_MAX;

  printf("After drawing the numbers:\n");
  for(int i=0;i<noOfData;i++)
    printf("%f\n",data[i]);

  printf("\n\n");

  //distributing data to other arrays
  /*if(noOfData%threads==0)
  {
    printf("Remainder is 0\n");
    int divide = noOfData/threads;
    float **dividedArray=malloc(sizeof(float*)*threads);

    for(int i=0;i<threads;i++)
      dividedArray[i]=malloc(sizeof(float)*divide);

    for(int i=0,k=0;i<noOfData;k++)
      for(int j=0;j<divide;j++,i++)
        dividedArray[k][j]=data[i];

    printf("After distributing the numbers:\n");
    for(int i=0;i<threads;i++)
      for(int j=0;j<divide;j++)
        printf("%f\n",dividedArray[i][j]);
  }
  else
  {*/
    int divide = noOfData/threads;
    int remainder = noOfData%threads;
    printf("Remainder is %d\n",remainder);
    float **dividedArray=malloc(sizeof(float*)*threads);

    for(int i=0;i<threads-1;i++)
      dividedArray[i]=malloc(sizeof(float)*divide);
    dividedArray[threads-1]=malloc(sizeof(float)*(divide+remainder));

    for(int i=0,k=0;i<noOfData-divide-remainder;k++)
      for(int j=0;j<divide;j++,i++)
        dividedArray[k][j]=data[i];
    for(int i=noOfData-divide-remainder,j=0;i<noOfData;j++,i++)
      dividedArray[threads-1][j]=data[i];

    printf("After distributing the numbers:\n");
    for(int i=0;i<threads-1;i++)
      for(int j=0;j<divide;j++)
        printf("%f\n",dividedArray[i][j]);
    for(int j=0;j<divide+remainder;j++)
      printf("%f\n",dividedArray[threads-1][j]);
  //}

  pthread_t threadID;
  pthread_create(&threadID,NULL,threadCode,dividedArray[0]);

pthread_join(threadID,NULL);
//freeing allocated memory
  for(int i=0;i<threads;i++)
    free(dividedArray[i]);
  free(dividedArray);
  return 0;
}
