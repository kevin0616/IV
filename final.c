#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<sys/wait.h>
#define NUM_PROCS 4
#define NUM_TRIALS 1000000
#define READ_END 0
#define WRITE_END 1

int simulation(int n){
	int res=0;
	srand((unsigned int)time(NULL)+getpid());
	for(int i=0; i<NUM_TRIALS; i++){
		int tmp[n];
		int hist[365]={0};
		for(int j=0; j<n; j++){
			tmp[j]=rand()%365;
			hist[tmp[j]]++;
			if(hist[tmp[j]]>1){
				res+=1;
				break;
			}
		}
	}
	return res;
}

int main(int argc, char *argv[]){
	int n=atoi(argv[1]);
	int pip1[2], pip2[2], pip3[2], pip4[2];
	pipe(pip1);
	pipe(pip2);
	pipe(pip3);
	pipe(pip4);
	int c1=fork();
	if(c1>0){
		int c2=fork();
		if(c2>0){
			int c3=fork();
			if(c3>0){
				int c4=fork();
				if(c4>0){
					close(pip4[WRITE_END]);
					char buf[20];
					read(pip4[READ_END], buf, 20);	
					int receive = atoi(buf);
					printf("result is %f%%\n", (float)receive/NUM_PROCS/NUM_TRIALS*100);				
				}else{
					close(pip3[WRITE_END]);
					close(pip4[READ_END]);
					int nhits=simulation(n);
					char buf[20];
					read(pip3[READ_END], buf, 20);
					int receive = atoi(buf);
					char buff[20];
					sprintf(buff,"%d", receive + nhits);
					write(pip4[WRITE_END], buff, strlen(buff));
				}
			}else{
				close(pip2[WRITE_END]);
				close(pip3[READ_END]);
				int nhits=simulation(n);
				char buf[20];
				read(pip2[READ_END], buf, 20);
				int receive = atoi(buf);
				char buff[20];
				sprintf(buff,"%d", receive + nhits);
				write(pip3[WRITE_END], buff, strlen(buff));
			}
		}else{
			close(pip1[WRITE_END]);
			close(pip2[READ_END]);
			int nhits=simulation(n);
			char buf[20];
			read(pip1[READ_END], buf, 20);
			int receive = atoi(buf);
			char buff[20];
			sprintf(buff,"%d", receive + nhits);
			write(pip2[WRITE_END], buff, strlen(buff));
		}
		
	}else{
		close(pip1[READ_END]);
		
		int nhits=simulation(n);
		char buf[20];
		sprintf(buf,"%d",nhits);
		write(pip1[WRITE_END], buf, strlen(buf));
	}
}
