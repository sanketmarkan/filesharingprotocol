#include <iostream>
#include <stdio.h>
#include <openssl/md5.h>
#define DELIM " \t\r\n"
unsigned char h[MD5_DIGEST_LENGTH];
time_t last;
void gethash(char* name){
	//printf("name: %s",name);
	MD5_CTX cntxt;
	int b;
	unsigned char data[1024];
	FILE *in = fopen(name,"r");
	if(!in){
		printf("file open error\n");
		return;
	}
	MD5_Init (&cntxt);
	while((b=fread(data,1,1024,in))!=0){
		MD5_Update(&cntxt,data,b);
	}
	MD5_Final(h,&cntxt);
	fclose(in);	
}
void printhash(unsigned char hash[]){
	for(int i=0;i< MD5_DIGEST_LENGTH;++i){
		printf("%02x",h[i]);
	}
	printf("\n");
}
void verify(char* fname){
	//printf("v : %s\n",fname );
	gethash(fname);
	last = findtime(fname);
}
int Hashhandler(char* cmd){
	char* tok = strtok(NULL,DELIM);	
	if(!strcmp(tok,"--verify")){
		tok = strtok(NULL,DELIM);
		strcpy(files[0].name,tok);
		gethash(parse2(files[0].name));
		for(int j=0;j<MD5_DIGEST_LENGTH;++j){
			files[0].hash[j]=h[j];
		}
		files[0].timestamp = findtime(files[0].name);
		return 1;
	}
	else if(!strcmp(tok,"--checkall")){
		int nu = IndexGet();
		for(int i=0;i<nu;++i){
			gethash(parse2(files[i].name));
			for(int j=0;j<MD5_DIGEST_LENGTH;++j){
				files[i].hash[j]=h[j];
			}
			files[i].timestamp = findtime(parse2(files[i].name));
		}
		return nu;
	}
	else{
		//printf("wrong input\n");
		return -1;
	}
}
