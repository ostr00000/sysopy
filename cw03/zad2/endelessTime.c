#include <stdio.h>
#include <unistd.h>

int main(){
	int tmp,c=1;
	while(1){
		for(int i=0;i<10000;i++)tmp=c*i*tmp+c*tmp+c;
		if(c++%10000==0)printf("I don't sleep\n");
	}
}
