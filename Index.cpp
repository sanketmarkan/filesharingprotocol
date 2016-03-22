#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <regex>
#include <openssl/md5.h>
#define DELIM " \t\r\n"
typedef struct file{
	char name[100];
	time_t timestamp;
	char type[100];
	off_t size;
	unsigned char hash[MD5_DIGEST_LENGTH];
}file;
file files[100];

int shortlist(time_t st_time,time_t end_time){
	char *line =NULL;
	char temp[100];
	size_t len=0;
	ssize_t r;
	int num=0;
	if(system("ls> tmp")<0){
		printf("Error running system\n");
		exit(-1);
	}
	FILE *fp;
	fp=fopen("tmp","r");
	if(fp==NULL){
		printf("Error opening file\n");
		exit(-1);
	}
	while((r = getline(&line,&len,fp))!=-1){
		time_t ti = findtime(parse2(line));
		if(difftime(ti,st_time) > 0 && difftime(end_time,ti) > 0){
			//printf("done\n");
			strcpy(files[num].name,line);
			strcpy(files[num].type,filetype(line));
			files[num].size = filesize(line);
			files[num].timestamp=findtime(line);
			num++;
		}
	}
	fclose(fp);
	if(line)
		free(line);
	system("rm tmp");
	return num;
}
int Regex(char* reg){
	std::regex re(reg);
	char *line =NULL;
	char temp[100];
	size_t len=0;
	ssize_t r;
	if(system("ls> tmp")<0){
		printf("Error running system\n");
		exit(-1);
	}
	FILE *fp;
	fp=fopen("tmp","r");
	if(fp==NULL){
		printf("Error opening file\n");
		exit(-1);
	}
	int num=0;
	while((r = getline(&line,&len,fp))!=-1){
		//printf("%s\n",line );
		if(std::regex_match(parse2(line),re)){
			//printf("match\n");
			strcpy(files[num].name,line);
			strcpy(files[num].type,filetype(line));
			files[num].size = filesize(line);
			files[num].timestamp=findtime(line);
			time_t ti = findtime(line);
			num++;
		}
	}
	fclose(fp);
	if(line)
		free(line);
	system("rm tmp");
	return num;
}
int IndexGet(){
	char *line =NULL;
	char temp[100];
	size_t len=0;
	ssize_t r;
	if(system("ls> tmp")<0){
		printf("Error running system\n");
		exit(-1);
	}
	FILE *fp;
	fp=fopen("tmp","r");
	if(fp==NULL){
		printf("Error opening file\n");
		exit(-1);
	}
	int num=0;
	while((r = getline(&line,&len,fp))!=-1){
		//time_t ti = findtime(line);
		if(strcmp(parse2(line),"tmp")==0)
			continue;
		strcpy(files[num].name,line);
		strcpy(files[num].type,filetype(line));
		files[num].size = filesize(line);
		files[num].timestamp=findtime(line);
		time_t ti = findtime(line);
		num++;
	}
	fclose(fp);
	if(line)
		free(line);
	system("rm tmp");
	return num;
}
int handler(char* cmd){
	char* tok;
	tok=strtok(NULL,DELIM);
	//printf("tok:%s\n",tok );
	if(!tok){
		printf("Invalid/No flags\n");
		return 0;
	}
	if(!strcmp(tok,"--longlist")){
		return IndexGet();
	}
	else if(!strcmp(tok,"--shortlist")){
		time_t st,en;
		tok=strtok(NULL,DELIM);
		st = (time_t)atoi(tok);
		tok=strtok(NULL,DELIM);
		en = (time_t)atoi(tok);
		return shortlist(st,en);
	}
	else if(!strcmp(tok,"--regex")){
		tok=strtok(NULL,DELIM);
		return Regex(tok);
	}
	else{
		//printf("wrong input\n");
		return -1;
		//wrong input
	}
}
