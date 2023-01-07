#include"common.h"


void *send_data(void* param);

int main()
{
	int server_sockfd, client_sockfd;
	socklen_t sin_size;
	struct sockaddr_in server_addr, client_addr;

	memset(&server_addr,0,sizeof(server_addr));
	//Create socket 
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket create error\n");
		exit(1);
	}

	//Set the socket's attributes
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(server_addr.sin_zero), 8);

	//Create a link
	if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1) {
		perror("bind error\n");
		exit(1);
	}

	//Listening requests from clients
	if (listen(server_sockfd, BACKLOG) == -1) {
		perror("listen error\n");
		exit(1);
	}

	//Accept requests from clients,loop and wait.
	cout << "-HTTP Server [xhttp] started..." << endl;
	while (1) {
		sin_size = sizeof(client_addr);
		if ((client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &sin_size)) == -1) {
			perror("accept error!\n");
			continue;
		}
		cout<<"Received a connection from "<<inet_ntoa(client_addr.sin_addr)<<endl;

		pthread_t id;
		pthread_create(&id,NULL,send_data,&client_sockfd);
		pthread_join(id,NULL);

		close(client_sockfd);
	}
	return 0;
}

void *send_data(void* param){
	int client_sockfd,len;
	char buf[BUF_SIZE],ext[256],temp[256],file[256];
	char output[BUF_SIZE],content_type[256];
	char *pa,*pb,*pc,*pd;
	struct stat st;
	FILE* fp;

	memset(buf,0,sizeof(buf));
	memset(ext,0,sizeof(ext));
	memset(temp,0,sizeof(temp));
	memset(file,0,sizeof(file));
	memset(output,0,sizeof(output));
	memset(content_type,0,sizeof(content_type));

	client_sockfd = *(int*)param;
	cout<<"Request Header:\n";

	while((len = recv(client_sockfd, buf, sizeof(buf)-1, 0)) > 0){
		cout<<buf<<endl;

		if((pa=strstr(buf,"Accept: ")) != NULL){
			pb = strchr(pa+1,' ');
			pc = strchr(pa+1,',');
			pd = strchr(pa+1,'\r');
			pc = pc<pd ? pc : pd;
			strncpy(ext,pb+1,pc-pb-1);
		}
		cout<<"DEBUG - file extention: "<<ext<<endl;
		
		if(strncmp(ext,"image/png",9) ==0){
			strcpy(content_type, "image/png");
		}else if(strncmp(ext,"text/html",9) ==0){
			strcpy(content_type,"text/html");
		}else if(strncmp(ext,"text/css",8) ==0){
			strcpy(content_type,"text/css");
		}
		cout<<"DEBUG - content type: "<<content_type<<endl;

		if(strncmp(buf,"GET",3) == 0){
			strcpy(file,server_dir);
			pa = strchr(buf,' ');
			pb = strchr(pa+1,' ');
			strncpy(temp,pa+1,pb-pa-1);
			strcat(file,temp);
			cout<<"DEBUG - file path and name: "<<file<<endl;

			pc = strchr(temp,'.');
			pd = temp+strlen(temp);
			strncpy(ext,pc+1,pd-pc-1);
			cout<<"DEBUG - file extention: "<<ext<<endl;
			
			stat(file,&st);
			if(strlen(content_type)==0) {
				if(strcmp(ext,"png")==0){
					strcpy(content_type, "image/png");
				}else {
					// strcpy(content_type, "*/*");
					strcpy(content_type,"text/html; charset=UTF-8");
				}
			}
			cout<<"Respond Header:\n";
			sprintf(output,"HTTP/1.1 200 OK\r\n"
					"Server: HttpServer\r\n"
					// "Accept-Ranges: bytes\r\n"
					
					// "Content-Encoding: gzip\r\n"
					"Content-Type: %s\r\n"
					"Content-Length: %d bytes\r\n"
					
					"Connection: keep-alive\r\n\r\n",content_type,(int)st.st_size);
			cout<<output;

			send(client_sockfd,output,sizeof(output),0);
			fp = fopen(file,"rb");
			while(fread(buf,1,sizeof(buf),fp) > 0){
				send(client_sockfd,buf,sizeof(buf),0);
			}
			close(client_sockfd);
			fclose(fp);
		}
		cout<<"**********************************************************"<<endl;
	}
	
	return NULL;
}
