#include <stdio.h>
#include <unistd.h>
int main(int argc,char* argv[]){
	fprintf(stderr,"progam name: %s\n",argv[0]);    
	for(int i=1;i<argc;i++){
		printf("arg %d : %s\n",i,argv[i]);
	}
	printf("pid = %d\n", getpid());
	fflush(stdout);
	return 0;
}
