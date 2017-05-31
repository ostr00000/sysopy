#include <stdio.h>
#include <stdlib.h>

//#define _XOPEN_SOURCE

int main(int argc,const char* argv[]){
	if(argc<2){
		printf("error - no argument\n");
		return 1;
	}
	char* env=getenv(argv[1]);
	if(NULL==env){
		printf("no environment variable %s\n",argv[1]);
	}else{
		printf("%s=%s\n",argv[1],env);
	}
	return 0;
}
