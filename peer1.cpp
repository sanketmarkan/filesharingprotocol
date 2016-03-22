#include "server.cpp"
#include "client.cpp"
#include <unistd.h>
int main(int argc , char *argv[]){
	pid_t pid;
	pid=fork();
	if(pid==0)
		int c = Client(8001,5001);    //8001
	else if(pid)
		int s = Server(8000,5000);
	return 0;
}