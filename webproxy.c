#include "webproxy.h"

char header[] = 
"HTTP/1.1 200 Ok\r\n"
"Content-Type: text/html; charset=utf-8\r\n\r\n";

char forbidden[4][FORBMAX];

int main(int argc, char *argv[])
{
	
	if(argc != 3){
		printf("Usage error.\n");
		exit(-1);
	}
	
	ssize_t nbytes;
	struct timeval t;
	t.tv_sec = atoi(argv[2]);
	
	int numForb = fileParser();
	char buff[MAXHEADER];
	
	struct hostent* host;
	struct sockaddr_in server_addr, client_addr;
	int fd_server, fd_client;
	int on = 1;
	pid_t pid;
	
	
	bzero(&server_addr, sizeof(server_addr));           //zero the struct
	socklen_t client_len = sizeof(client_addr);
	
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	server_addr.sin_family = AF_INET;   
	server_addr.sin_port = htons(atoi(argv[1]));       
	server_addr.sin_addr.s_addr = INADDR_ANY;          
	
	if ((fd_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("unable to create socket");
	}

	/******************
	  Once we've created a socket, we must bind that socket to the 
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("unable to bind socket\n");
		exit(-1);
	}
	
	
	if(listen(fd_server, 5) < 0 ){
		perror("unable to listen");
		close(fd_server);
		exit(0);
	}
	printf("Listen called\n");
	
	
	
	while(1)
	{
		
		fd_client = accept(fd_server, (struct sockaddr*)&client_addr, &client_len);
		printf("Connection accepted\n");
		pid = fork();
		if (pid < 0){
			printf("Fork error, now exiting.\n");
			exit(-1);
		}
		
		
		else if (pid == 0)
		{
			struct sockaddr_in host_addr;
			close(fd_server);
			
			char method[10], url[300], version[20]; //path[100];
			bzero(buff, sizeof(buff));
			read(fd_client, buff, sizeof(buff));
			int flag = 0, port, i, newsockfd, newsockfd2, nbytes = 0;
			char *tmp = NULL;
			char *path = NULL;
			
		

					
					sscanf(buff, "%s %s %s", method, url, version);
				
					

					
					if(!strncmp("GET", method, 3))
					{
						
						printf("Valid method. proceeding...\n");
						
						send(fd_client, header, sizeof(header)-1, 0);

					
								
											for(i=7;i<strlen(url);i++)
											{
												if(url[i]==':')
												{
													flag=1;
													break;
												}
											}
											
											tmp = strtok(url, "//");
											if (flag == 0){
												port = 80;
												tmp = strtok(NULL, "/");
												path = strtok(NULL, "\0");
												
											
											}
											else
											{
												tmp = strtok(NULL, ":");
											}
										
											
											sprintf(url, "%s", tmp);
											int p,t;
																			
											for(p = 0; p < numForb; p++)
											{
												if (strstr(forbidden[p], url) != NULL)
												{
													printf("bad url\n");
													char msg[] = "ERROR 403 FORBIDDEN";
													write(fd_client, msg, sizeof(msg));
													goto EXITING;
												}
											}
										

											
											host = gethostbyname(url);
											
											
											
											if (flag == 1){
												tmp = strtok(NULL, "/");
												port = atoi(tmp);
												path = strtok(NULL, "\0");
												
											}
																						
											

											bzero((char*)&host_addr, sizeof(host_addr));
											host_addr.sin_port=htons(port);
											host_addr.sin_family=AF_INET;
											bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
										
											
											for(t = 0; t< numForb; t++)
											{
												printf("IP - %s\n", inet_ntoa(host_addr.sin_addr));
												if (strstr(inet_ntoa(host_addr.sin_addr), forbidden[t]) != NULL)
												{
													printf("bad IP\n");
													char msg[] = "ERROR 403 FORBIDDEN";
													write(fd_client, msg, sizeof(msg));
													goto EXITING;
												}
											}
										
											newsockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); //creating socket leading to server
											if (newsockfd < 0){
												printf("Error creating new socket.\n");
												exit(-1);
											}
											
											newsockfd2=connect(newsockfd,(struct sockaddr*)&host_addr,sizeof(struct sockaddr)); //connecting to server as "client"
											
											if (newsockfd2 < 0){
												printf("Error connecting to host.\n");
												exit(-1);
											}
											sprintf(buff,"\nConnected to %s  IP - %s\n", url, inet_ntoa(host_addr.sin_addr));
											printf("%s", buff);
											bzero(buff, sizeof(buff));
											
											if(path != NULL)
												sprintf(buff,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",path,version,url);
											else
												sprintf(buff,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n", version, url);
												
											printf("buff to send = \n%s\n", buff);
											
											nbytes = send(newsockfd, buff, strlen(buff), 0);
											if (nbytes < 0)
												printf("Error writing to socket\n");
											else
											{
												do{
													bzero((char*)buff, 500);
													nbytes = recv(newsockfd, buff, 500, 0);
													cacher(buff);
													if(nbytes>0) send(fd_client, buff, nbytes, 0);
												}while(nbytes>0);
											}
						
					
					
					}
					else{
						char msg[] = "HTTP/1.1 400 Bad Request";
						printf("Invalid method.\n");
						write(fd_client, msg, sizeof(msg));
						break;
						
					}
					
									
			close(newsockfd);
			close(newsockfd2);
			
			EXITING:
			
			printf("Now exiting child process successfully.\n");	
			_exit(0);
			
		}
		
		else if (pid > 0)
		{		
			close(fd_client);
			printf("Now in parent process.\n");
			waitpid(0, NULL, WNOHANG);
		}
		
	}

	return 0;
}
 
 
int fileParser()
{	
	FILE *fp;
	char fileName[] = "/forbiddensites.txt";
	char *currentpath, *ptr, *currentline;
	char fbuf[PATH_MAX+1], filepath[PATH_MAX+1], line[50];
	int i = 0;

	char fileExt[] = "/";
	
	currentpath = getcwd(fbuf, PATH_MAX+1);
	strcpy(filepath, currentpath);
	strcat(filepath, fileName);
	
	fp = fopen(filepath, "r");
	
	if(fp == NULL){
		printf("Couldn't open forbidden sites file\n");
		exit(-1);
	}
	
	
	
	while (fgets(line, 50, fp) != NULL)
	{
		currentline = strtok_r(line, "\n", &ptr);
		strcpy(forbidden[i], currentline);
		i++;
	}

	fclose(fp);
	return i;
	
}

void cacher(char *buffer){
	
	FILE *fp;
	char *currentpath, fbuf[PATH_MAX], filepath[PATH_MAX];
	char fileName[] = "/cachefile.txt";
	currentpath = getcwd(fbuf, PATH_MAX);
	strcpy(filepath, currentpath);
	strcat(filepath, fileName);
	
	printf("filepath = %s\n", filepath);
	
	
	fp = fopen(filepath, "a");
	if (fp == NULL){
		printf("Couldn't create new file\n");
	}
	printf("Successfully created new file\n");
	
	
	
	fwrite(buffer, 1, strlen(buffer)-1, fp);
	
	fclose(fp);
	
}
