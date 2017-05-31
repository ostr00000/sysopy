#include <stdio.h>
#include <stdlib.h>

int main(){
	int const SIZE=1024*1024;
	int bytes=-1;
	while(1){
		if(++bytes%100==0)printf("alocated space: %dMB\n",bytes);
		char* ptr=malloc(SIZE);
		for(int i=0;i<SIZE;i++)ptr[i]='\0';
	}
}
