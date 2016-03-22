#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <readline/readline.h>
#include <readline/history.h>
using namespace std;
int Client(int port,int udpport){
	int sockfd;
	struct sockaddr_in server_addr;
	struct hostent *server;
	char *buf;
	char buffer[1000],readbuff[100000],writebuff[1000];
	//printf("Enter client port\n");
	//scanf("%d",&port);
	sockfd=socket(AF_INET,SOCK_STREAM,0);  //create socket
	if(sockfd<0){
		printf("Error creating socket\n");
		return -1;
	}
	server=gethostbyname("127.0.0.1");
	//cout<<strlen((char*)server->h_addr)<<endl;
	if(server==NULL){
		printf("No such host\n");
		exit(-1);
	}
	memset((char*)&server,0,sizeof(server));  //reset server_addr struct
	server_addr.sin_family=AF_INET;
	//bcopy((char*)server->h_addr,(char*)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port=htons(port);
	//assert(server!=NULL);
	//server_addr.sin_addr=*((struct in_addr*)server->h_addr);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bzero(&(server_addr.sin_zero),8);

	int udpfd;                                        //creating udp socket
	struct sockaddr_in serv_addr,cli_addr;
	udpfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(udpfd<0){
		printf("Socket UDP not created client\n");
		exit(-1);
	}
	//else printf("Socket UDP created client\n");
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(udpport);

	//connect
	printf("Connecting.....\n");
	while(connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
		//printf("Connecting...\n");
		continue;
	}
	printf("Connected\n");
	while((buf = readline("\n >>"))!=NULL){
		int k=0;
		//while((writebuff[k++]=getchar())!='\n');
		//writebuff[k]='\0';
		strcpy(writebuff,buf);
		if(writebuff[0]!=0){
			add_history(writebuff);
		}
		if(write(sockfd,writebuff,strlen(writebuff))<0){
			printf("Write Error clientt\n");
			exit(-1);
		}
		char* tok = strtok(writebuff," \t\r\n");
		if(strcmp(tok,"FileDownload")==0){
			char fname[100],protocol[10];
			tok = strtok(NULL," \t\r\n");
			strcpy(protocol,tok);
			tok = strtok(NULL," \t\r\n");
			strcpy(fname,tok);
			strcat(fname,"_downloaded");
			bzero(readbuff,100000);
			if(read(sockfd,readbuff,100000)<0){
				printf("Read error\n");
				exit(-1);
			}
			printf("File size: %d Y/N\n",atoi(readbuff));
			char c;
			scanf("%c",&c);
			//c=getchar();
			bzero(writebuff,1000);
			sprintf(writebuff,"%c",c);
			if(write(sockfd,writebuff,strlen(writebuff))<0){
				printf("Write Error clientt\n");
				exit(-1);
			}
			if(c=='N')continue;
			FILE *f = fopen(fname,"w+");
			if(!f){
				printf("file open error\n");
				exit(-1);
			}

			if(strcmp(protocol,"UDP")==0){
				strcpy(buffer,"UDP connected");
				sendto(udpfd,buffer,strlen(buffer)+1,0,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr));
				//printf("Sent\n");
				int size = atoi(readbuff);
				while(size>0){
					bzero(buffer,1000);
					int r = recvfrom(udpfd,buffer,sizeof(buffer),0,NULL,NULL);
					//printf("udp_recvd : %d\n",r );
					if(r<=0)break;
					if(fwrite(buffer,1,r,f)<=0)
						break;
					size-=r;

				}
			}
			else{
				
				int r,size=atoi(readbuff);
				//printf("size:%d\n",size);
				bzero(buffer,1000);
				while(size>0){
					r = read(sockfd,buffer,1000);
					//printf("read : %d\n",r );
					if(r<=0)
						break;
					if(fwrite(buffer,1,r,f)<=0)
						break;
					size-=r;
					bzero(buffer,1000);
				}
			}
			fclose(f);
			printf("File received\n");
			bzero(readbuff,100000);
			if(read(sockfd,readbuff,100000)<0){
				printf("Read error\n");
				exit(-1);
			}
			printf("%s\n",readbuff );
			continue;
		}
		bzero(readbuff,100000);
		if(read(sockfd,readbuff,100000)<0){
			printf("Read error\n");
			exit(-1);
		}
		printf("%s\n",readbuff );
		fflush(stdout);
		fflush(stdin);
	}
	close(sockfd);
	return 0;
	}

