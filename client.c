//client code have a server part and client part
//client can do multiple publish
//client can do fetch only once and takes exit
//client can send any file format(jpg,mp3,pdf)
//client server side is always running to send files published by it
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{

    char line[1024]; //clients server side buffer to send file
    char s[1000]; //clients buffer to recieve file
    size_t len = 0; //used to send file by clients server
    FILE *fp;//file pointer at clients server side
    FILE *fp1;//file pointer at clients side
    struct sockaddr_in serv_addr,contact;//stores information about client side socket
    struct sockaddr_in client_ser_addr;//stores information about clients server side socket
    int contactfd=0;//is client side socket discriptor(used for fetch)
    int listenfd = 0,listenfd_c=0;//is client's server socket discriptor
    int sockfd = 0;//client side socket descriptor(after making contact from central server)
    int n = 0,o,p,r,lenk;
    int option=0;//stores user's choice to fetch or publish
    int i;//used for fetch operation
    int client_ser_port;//client's server port number
    int g=0;//g used to close connection from central server
    int lenc=0,a=0;//used for publish operation
    char filename[100];//stores name of the file to be published
    char client_fetch[100];//client's server port number fetched from central server
    char pub_succ[100];//to store info about publish success
    char str[100];//Client's client part server port number(used to convert server port from int to string)
    char recmsg[100];//clients server side buffer to store name of the file to be fetched
    char k[10]="-99";//to look it file is published by some peer
    char ip_fetch_server[100];//stores ip of the peer to fetch file from
    char port_n[100];//stores port number of the peer to fetch file from
    socklen_t addrlen;//stores client's server struct size
    listenfd = socket(AF_INET, SOCK_STREAM, 0);//client's server side socket open
    memset(&client_ser_addr, '0', sizeof(client_ser_addr));
    client_ser_addr.sin_family = AF_INET;//client's server side Address family
    client_ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);//client's server side IP
    client_ser_addr.sin_port =0;  //will assign client with any random port 
    bind(listenfd, (struct sockaddr*)&client_ser_addr, sizeof(client_ser_addr));//to bind client's side server with a port address
    addrlen = sizeof(client_ser_addr);
    char dd[100];//used with publish
    //char file_store[100]="/home/ayush/subj1/";//path to store fetched files
    if(getsockname(listenfd,(struct sockaddr *)&client_ser_addr,&addrlen)==-1)//to store client's side port number
    {
          perror("getsockname");
    }
    else
    client_ser_port=ntohs(client_ser_addr.sin_port);
    if(fork()!=0)//client's server
    {
          printf("clients server\n");
 	  listen(listenfd, 10);//10 connection can be made with the client's server at a time
          while(1)
	  {
		listenfd_c= accept(listenfd, (struct sockaddr*)NULL, NULL);//to make connection with other peer
		read(listenfd_c,recmsg,sizeof(recmsg));//to read file name sent by other peers 
		fp=fopen(recmsg,"r");
		if(fp==NULL)
  		return 1;
		while (!feof(fp))//sending file to other peers
		 {
			n=fread(line,1,1024,fp);
			send(listenfd_c,line,n,0);
		 }
	 	fclose(fp);
		close(listenfd_c);
		printf("file fetched to the requesting peer...");
	  }  
   } 
   else   //client code starts from here
   {
   	printf("I am Client's client part my server port id is %d\n",client_ser_port);
   	sprintf(str, "%d",client_ser_port);//to convert port number of client's server to string
   	fflush(stdout);
   	close(listenfd);
   	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)//open socket at client's client side
    	{
      		printf("\n Error : Could not create socket \n");
       		return 1;
   	} 
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(atoi(argv[2]));//central server port number
	printf("Server address used is: %s\n", argv[1]);//central servel ip address
        if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
  	{
        	printf("\n inet_pton error occured\n");
        	return 1;
    	} 
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)//connection made to central server
    	{
		perror("\n Error : Connect Failed \n");
		return 1;
    	} 
    	read(sockfd, s,sizeof(s));
    	printf("I just read this string: %s\n", s);
	fflush(stdout);
    	while(1)
   	{
   		printf("1-to publish\n2-to fetch\n3-Exit\n");
    		scanf("%d",&option);//stores user option
        	switch(option)
    	 	{ 
       			case 1: if(write(sockfd,"1",5)) //to send server the user's choice
				perror("\n");
				read(sockfd,dd,sizeof(dd));
 		       		printf("%s\n",dd);
        	       		printf("specifiy the file name\n");
				scanf("%s",filename);
				lenc=strlen(filename);
				filename[lenc]=':';
				for(a=1;a<=strlen(str);a++)
				{
					filename[a+lenc]=str[a-1];
				}
				filename[a+lenc]='\0';
				if(write(sockfd,filename,sizeof(filename)))//file to be published
				{perror("\n");}
				else
				{perror("failed write\n");}
				fflush(stdout);
 		        	read(sockfd,pub_succ,sizeof(pub_succ));//client's server port number send to central server
        	       		printf("server says = %s\n",pub_succ);
                       		fflush(stdout);
				fflush(stdin);
                       		break;
     	  		case 2: write(sockfd,"2", 5); //to send server the user's choice
        	       		printf("you choose fetch operation\n");
        	       		printf("specifiy the file name\n");
        	       		scanf("%s",filename);
        	       		write(sockfd,filename, 100);//coice of file to be fetch to central server
        	       		read(sockfd,client_fetch,sizeof(client_fetch)); //peer server portnum whom to fetch from
                       		if(strcasecmp(client_fetch,k)==0)
                           	{
					printf("file not found\n\n");
			   	}
		       		else
				{
					close(sockfd);//close connection from central server
					lenk=strlen(client_fetch);
                	        	for(p=0;p<lenk;p++)
					{	if(client_fetch[p]!=':')
						ip_fetch_server[p]=client_fetch[p];//stores the ip of the peer to fetch file
						else
						break;	
					}
					ip_fetch_server[p]='\0';			
					o=0;
					for(r=p+1;r<lenk;r++)
					{	
						port_n[o]=client_fetch[r];//pot_n stores port number of the file to fetch file
						o++;
					}
			      		if((contactfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)//new connection to another client's server to fetch file
     					{
     				   		printf("\n Error : Could not create socket \n");
        			   		return 1;
    					}
		       	     		memset(&contact, '0', sizeof(contact));
		       	      		contact.sin_family = AF_INET;
   		              		contact.sin_port = htons(atoi(port_n)); //to convert to integer
			      		if(inet_pton(AF_INET,ip_fetch_server, &contact.sin_addr)<=0)//ip of client server
   			       		{
     				   		printf("\n inet_pton error occured\n");
    				   		return 1;
  			  		} 
			      		if( connect(contactfd, (struct sockaddr *)&contact, sizeof(contact)) < 0)//connection with others client sever
 			       		{
			       	   		perror("\n Error : Connect Failed \n");exit(1);
			       		}
			      		write(contactfd,filename,sizeof(filename));//to tell another clients server which file to fetch
			      		fflush(stdout);
					//strcat(file_store,filename);
					//strcat(file_store,"/");
					//printf("%s",file_store);
			      		fp1=fopen(filename,"a+");//path of the folder
					memset(s,'\0',sizeof s);
			      		while((i=recv(contactfd,s,1024,0))>0)//to recieve file from other peer
			       		{
				  		fwrite(s,1,i,fp1);
			       		}
			      		fclose(fp1);
					g=1;//to make exit
					printf("file successfully fetched\nbyee\nConnection ends of the client part");
			 	 }
        	         	 break;
			case 3 : g=1;
				 break;
     	  	 	default:  printf("wrong choice\nclient exits\nstart new connection");g=1;	
    	  	}//end of switch-case
		if(g==1)
		break;
       }//end of while
   }//end of client side 
}//end of main

