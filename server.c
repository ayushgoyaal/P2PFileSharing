//this is the server code..it can accept max of 10 connection..it is always running
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
int main(int argc, char *argv[])
{
    struct sockaddr_in their_addr;//default structure to automatically hold client address
    FILE *fp;//file pointer to the database
    char str[INET_ADDRSTRLEN];
    char str1[100];//holds port number selected to fetch file 
    char str2[100];//holds user requested file name
    char port_n[20];//holds published port number
    socklen_t addr_size;
    char file_search[1000];//holds name of the file user want to fetch
    char s[1000];//to read clients choice
    int listenfd = 0;//to allow number of clients
    int connfd = 0;//socket discriptor
    int clientnum=1;//to keep track of no. of clients
    int f=0,k=0;//k used if file not published
    int n=1;//to look for read write errors
    int i=0,u=0;//stores clients choice
    int lenc=0,o=0,g=0,p=0; //used for publish operation
    struct sockaddr_in serv_addr; 
    char pub_filename[20];//stores published file name
    char file_add[20];//file name and client's server port number
   
    listenfd = socket(AF_INET, SOCK_STREAM, 0);//welcome socket
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;//server family
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));//host to network short(Big endian)---port number of central server
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));//binding port with the socket
    listen(listenfd, 10);
    printf("welcome to server\nReady to accept connections\n");
    while(1)//infinite loop to accept multiple connections
    {
        addr_size = sizeof(their_addr); //since accept field take 3 arg as socklen_t type
        connfd = accept(listenfd,(struct sockaddr *)&their_addr,&addr_size); 
	if(connfd<0)
	{
		perror("accept error");
		exit(1);		
	}
        inet_ntop( AF_INET, &(their_addr.sin_addr), str, INET_ADDRSTRLEN ); //converts ip address of client to string
	fp=fopen("database","a");//database file
	fclose(fp);
        printf("connection accepted = %d\n",clientnum);
	if(fork()==0)
	{
		close(listenfd);
                n=write(connfd,"connection accepted\n",20);
  		if (n < 0) 
                   {
  	               perror("ERROR writing to socket");
                       exit(1);
                   }
                while(1)
                {
			memset(&s,'0', sizeof(s));
          	     	if((u=read(connfd,s,sizeof(s)))<=0)
			perror("\n");
                	i=(int)s[0];//to read client's choice
               		//printf("client %d choice is %d \n",clientnum,i);
			fflush(stdout);//display user choice
                	if(i==49)         //publish
                	{
				write(connfd,"client choose to publish",26);
				if(read(connfd,file_add,sizeof(file_add))<=0)
				{perror("\n");}//file_add clients server port number and file name
				lenc=strlen(file_add);
                	        for(p=0;p<lenc;p++)
				{	if(file_add[p]!=':')
					pub_filename[p]=file_add[p];
					else
					break;	
				}
				pub_filename[p]='\0';			
				o=0;
				for(g=p+1;g<lenc;g++)
				{	
					port_n[o]=file_add[g];	o++;
				}
                	      fp=fopen("database","a+");//To write in databasae
                	      fprintf(fp,"%s:%s\t%s\n",str,port_n,pub_filename);//keeps record of published file
                	      fclose(fp);
                	      write(connfd,"Sucessfully Published\n",21);
                       }
                       else if(i==50)//search
                       {
                	      if(read(connfd,file_search,sizeof(file_search)))
				{perror("\n");}
                              printf("client %d choose fetch of %s \n",clientnum,file_search);
  			      fp=fopen("database","r");
                              while(!(feof(fp)))//to look into database for filesearch
                              { 
				     fscanf(fp,"%s %s",str1,str2);//str1 stores port number and ip address of the client
	                             if(strcasecmp(file_search,str2)==0)
				     {
        	                      	   write(connfd,str1,sizeof(str1));//gives client the port number of clients server having file
					   fclose(fp);
					   close(connfd);
				     	   k=1;
                                     	   break;
 				      }
                              }
                              if(k==0)
                              {
                                      write(connfd,"-99",4);
				      fclose(fp);
				      close(connfd);
				      break;//to break connection from client to let him make another connection
                              }exit(0);
                   
                      }
                      else
                      {
                	      printf("connection closed=%d",clientnum);close(connfd);exit(0);
                      }
		u=read(connfd,s,sizeof(s));

              }//end of while loop
	}//end of connection
	close(connfd);
        clientnum++;
   }//end of while loop
}//end of main
