#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum;

void *runner(void *param);


int main(int argc, char *argv[]){

	pthread_t tid;
	pthread_attr_t attr;
	if(argc != 2){
		fprintf(stderr, "usage: ./pthread <integer value>\n");
		return -1;
	}
	if (atoi(argv[1]) < 0)
	{
		fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
		return -1;
	}
	// give the default attributes...
	pthread_attr_init(&attr);
	// create the thread....
	pthread_create(&tid,&attr,runner,argv[1]);
	// wait for the thread exit...
	pthread_join(tid, NULL);

	printf("sum = %d\n", sum);

    return 0;
}

// the thread will begin control in this function..

void *runner(char *param){
	int i=0,upper=atoi(param);
	sum = 0;
	for (int i = 0; i < upper; ++i)
	{
		sum+=i;
	}
	pthread_exit(0);
}

