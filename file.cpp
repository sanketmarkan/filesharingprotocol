#include <sys/stat.h>

char* parse(char* s){
	int l = strlen(s);
	char* ret = (char*)malloc((l+3)*sizeof(char));
	bzero(ret,sizeof(ret));
	if(s[l-1]=='\n'){
		s[l-1]='\0';
	}
	strcat(ret,"\"");
	strcat(ret,s);
	strcat(ret,"\"");
	return ret;
}
char* parse2(char* s){
	char* ret = new char[100];
	strcpy(ret,s);
	if(ret[strlen(ret)-1]=='\n'){
		ret[strlen(ret)-1]='\0';
	}

	return ret;
}

char* filetype(char* name){
	char tmp[100];
	char* line=NULL;
	char* tok;
	ssize_t r;
	size_t len=0;
	char* ret = parse(name);                      //parse to handle spaces
	sprintf(tmp,"file -b %s > tmp2",ret);        
	if(system(tmp)<0){
		printf("Error running command\n");
		exit(-1);
	}
	 FILE *fp = fopen("tmp2","r");
	 if(fp==NULL){
	 	printf("Error opening file\n");
	 	exit(-1);
	 }
	 if((r = getline(&line,&len,fp))==-1){
	 	printf("Error getline\n");
	 	exit(-1);
	 }
	 tok=strtok(line,",");
	 fclose(fp);
	 system("rm tmp2");
	 if(tok[strlen(tok)-1]=='\n'){
	 	tok[strlen(tok)-1]='\0';
	 }
	 return tok;
}
off_t filesize(char* name){
	struct stat filestat;
	if(stat(name,&filestat)<0){
		printf("size stat error = %d:%s\n",errno,strerror(errno));
		exit(-1);
	}
	return filestat.st_size;
}
time_t findtime(char* name){
	struct stat filestat;
	if(stat(parse2(name),&filestat)<0){
		printf("time stat error = %d:%s\n",errno,strerror(errno));
		exit(-1);
	}
	return filestat.st_mtime;
}