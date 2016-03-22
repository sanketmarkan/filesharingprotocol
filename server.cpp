#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "file.cpp"
#include "Index.cpp"
#include "hash.cpp"
#define DELIM " \t\r\n"
using namespace std;

int Server(int port,int updport){
	int sockfd,connfd;
	socklen_t cli_len;
	struct sockaddr_in server,client;
	char buffer[1000],writebuff[100000],readbuff[1000],result[100];;
	//create new socket
	//printf("Enter server port\n");
	//scanf("%d",&port);
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		printf("Error creating socket\n");
		return -1;
	}
	//bind port
	/*if(argc==0){
	  printf("No port provided\n");
	  return -1;
	  }*/
	//port = atoi(argv);
	//set up server_addr structure
	memset((char*)&server,0,sizeof(server));  //clear struct
	server.sin_family=AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port=htons(port);
	//now bind this socket
	if(bind(sockfd,(struct sockaddr*)&server,sizeof(server))<0){
		printf("Error binding port\n");
		return -1;
	}

	int udpfd;                                 //creaing udp socket
	socklen_t len;
	struct sockaddr_in serv_addr,cli_addr;
	udpfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(udpfd<0){
		printf("Socket create error server\n");
		exit(-1);
	}
	else printf("Socket UDP created server\n");
	bzero(&serv_addr,sizeof(serv_addr));
	//bzero(buffer,1000);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(updport);
	if(bind(udpfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
		printf("Error UDP socket bind server = %d:%s\n",errno,strerror(errno));
		exit(-1);
	}
	len = sizeof(cli_addr);


	//start listening
	printf("Listening......\n");
	listen(sockfd,3);
	cli_len=sizeof(client);
	connfd=accept(sockfd,(struct sockaddr*)&client,&cli_len);
	if(connfd<0){
		printf("Error on Accept\n");
		return -1;
	}
	printf("Clien connected %s:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
	while(1){
		bzero(readbuff,1000);
		if(read(connfd,readbuff,1000)<=0){
			//printf("Read error\n");
			break;
		}
		//printf("%s\n",readbuff);
		bzero(writebuff,100000);
		//printf("%d\n",(int)strlen(readbuff));
		char* Token = strtok(readbuff,DELIM);
		if(strcmp(Token,"IndexGet")==0){
			int nu = handler(readbuff);
			if(nu==-1){
				strcpy(writebuff,"Invalid flag\n");
				if(write(connfd,writebuff,strlen(writebuff))<0){
					printf("Error writing\n");
					exit(-1);
				}
				continue;
			}
			strcpy(writebuff,"Name       Size        Type       Timestamp\n");
			for(int i=0;i<nu;++i){
				sprintf(result,"%s      %d      %s       %s\n",files[i].name,(int)files[i].size,files[i].type,ctime(&files[i].timestamp));
				strcat(writebuff,result);
			}
			if(write(connfd,writebuff,strlen(writebuff))<0){
				printf("Error writing\n");
				exit(-1);
			}
		}
		else if(strcmp(Token,"FileHash")==0){
			int nu = Hashhandler(readbuff);
			if(nu==-1){
				strcpy(writebuff,"Invalid flag\n");
				if(write(connfd,writebuff,strlen(writebuff))<0){
					printf("Error writing\n");
					exit(-1);
				}
				continue;
			}
			strcpy(writebuff,"Name                    Hash                 LastModified\n");
			for(int i=0;i<nu;++i){
				sprintf(result,"%s                       ",files[i].name);
				strcat(writebuff,result);
				for(int c=0;c<MD5_DIGEST_LENGTH;++c){
					sprintf(result,"%x",h[c]);
					strcat(writebuff,result);	
				}
				sprintf(result,"\t %s\n",ctime(&last));
				strcat(writebuff,result);	
			}
			if(write(connfd,writebuff,strlen(writebuff))<0){
				printf("Error writing\n");
				exit(-1);
			}
		}
		else if(strcmp(Token,"FileDownload")==0){
			char fname[100],protocol[10];
			Token = strtok(NULL,DELIM);
			strcpy(protocol,Token);
			Token = strtok(NULL,DELIM);
			strcpy(fname,Token);
			//printf("Download requested : %s\n",fname );
			FILE *fp = fopen(Token,"rb");
			if(!fp){
				printf("Error openning file\n");
				exit(-1);
			}
			int size = filesize(Token);
			sprintf(result,"%d",size);
			if(write(connfd,result,strlen(result))<0){
				printf("Error writing\n");
				exit(-1);
			}
			bzero(readbuff,1000);
			if(read(connfd,readbuff,1000)<=0){
				printf("Read error\n");
				exit(-1);
			}
			//printf("%s\n",readbuff );
			if(strcmp(readbuff,"N")==0)continue;

			if(strcmp(protocol,"UDP")==0){                    
				 bzero(buffer,1000);
				recvfrom(udpfd,buffer,1000,0,(struct sockaddr*)&cli_addr,&len);
				//printf("Received\n");
				//printf("%s\n",buffer );
				bzero(buffer,1000);
				while(1){
					int bytes_read = fread(buffer,1,1000,fp);
					//printf("udp read:%d\n",bytes_read );
					if(bytes_read<=0)break;
					sendto(udpfd,buffer,bytes_read,0,(struct sockaddr*)&cli_addr,len);
				}
				close(udpfd);
				
			}
			else{
				int bytes_read;
				bzero(buffer,1000);
				while((bytes_read = fread(buffer,1,1000,fp))>0){
					char *p = buffer;
					while(bytes_read>0){
						int bytes_written = write(connfd,buffer,bytes_read);
						if(bytes_written<=0){
							printf("Error writing to socket\n");
							exit(-1);
						}
						//printf("written:%d\n",bytes_written );
						bytes_read-=bytes_written;
						p+=bytes_written;
					}
				}
			}
			fclose(fp);
			//printf("File sent\n");
			bzero(writebuff,100000);
			//printf("%s\n",fname );
			verify(parse2(fname));
			sprintf(writebuff,"Name           Size             Timestamp            MD5hash\n");
			sprintf(result,"%s           %d             %s             ",fname,size,ctime(&last));
			strcat(writebuff,result);
			for(int c=0;c<MD5_DIGEST_LENGTH;++c){
				sprintf(result,"%x",h[c]);
				strcat(writebuff,result);	
			}
			if(write(connfd,writebuff,strlen(writebuff))<0){
				printf("Error writing\n");
				exit(-1);
			}

		}
		else {
			strcpy(writebuff,"Invalid command");
			if(write(connfd,writebuff,strlen(writebuff))<=0){
				printf("Error writing to socket\n");
				exit(-1);
			}
		}
		fflush(stdout);
		bzero(writebuff,100000);
		bzero(readbuff,1000);
		//while(read(connfd,readbuff,sizeof(readbuff))<=0);

	}
	close(connfd);
	close(sockfd);
	return 0;
}
