#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cassert>
#include "record.h"
#include "command.h"
#include "list.h"
#include "rb_tree.h"
#include "database.h"
//#define PORT 5555
#define BUFLEN 1234
#define LEN 1234
int readFromClient (int fd, char *buf);
int writeToClient (int fd, char *buf);
static int read_config(int &k1,int &k2,int &m,char* namer)
{
	FILE *fp;
	int i=0;
	char buf[LEN]={'\0'};
	char *pos=nullptr;
	strcpy(buf,namer);
	pos=strstr(buf,"server");
	if(pos==nullptr)
		return 1;
	*pos='\0';
	strcat(buf,"config.txt");
	fp=fopen(buf,"r");
	if(fp==nullptr)
	{
		printf("ERROR_OPEN config\n");
		return 1;
	}
	while(fgets(buf,LEN,fp))
	{
		if(*buf=='#')
			continue;
		pos=buf+strspn(buf," \t\n");
		if(i==0)
		{
			if(sscanf(pos,"%d",&k1)==1)
			{
				i++;
			}
		}
		else if(i==1) 
		{
			if(sscanf(pos,"%d",&k2)==1)
			{
				i++;
			}
		}
		else
		{
			if(sscanf(pos,"%d",&m)==1)
			{
				fclose(fp);
				return 0;
			}
		}

	}
	printf("ERROR_READ config\n");
	fclose(fp);
	return 2;
}

int main(int argc,char *argv[])
{
	FILE *fp;
	int ret=0;
	int m=1;
	int PORT;
	int k1=0,k2=0;
	//double t;
	if(!(argc==3 && sscanf(argv[2],"%d",&PORT)==1))
	{
    	printf("ERROR SERVER INPUT a.txt PORT\n");
	    exit (EXIT_FAILURE);

	}
	ret=read_config(k1,k2,m,argv[0]);
	if(ret!=0)
	{
		printf("EROR config read\n");
		exit (EXIT_FAILURE);
	}
	else
	{
		int i, err, opt = 1; 
	    int sock, new_sock;
	    fd_set active_set, read_set;
	    struct sockaddr_in addr;
	    struct sockaddr_in client;
	
	    socklen_t size;
	    database mybase(k1,k2,m);
		if(!(fp=fopen(argv[1],"r")))
		{
			printf("ERROR_OPEN\n");
			exit (EXIT_FAILURE);
		}
	   	ret=mybase.read(fp);
		if(ret!=0)
		{
			printf("Server:ERROR_READ %d\n", ret);
			fclose(fp);
			exit (EXIT_FAILURE);
		}
		fclose(fp);
		printf("Server:Read file complete\n");
	    // Создаем TCP сокет для приема запросов на соединение
	    sock = socket (PF_INET, SOCK_STREAM, 0);

	    if (sock < 0)
	    {
	        perror ("Server: cannot create socket");
	        exit (EXIT_FAILURE);
	    }
	    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));
	    // Заполняем адресную структуру и
	    // связываем сокет с любым адресом
	    addr.sin_family = AF_INET;
	    addr.sin_port = htons (PORT);
	    addr.sin_addr.s_addr = htonl (INADDR_ANY);
	    err = bind (sock, (struct sockaddr *) &addr, sizeof (addr));
	    if (err < 0)
	    {
	        perror ("Server: cannot bind socket");
	        exit (EXIT_FAILURE);
	    }
	    // Создаем очередь на 3 входящих запроса соединения
	    err = listen (sock, 20);
	    if (err < 0)
	    {
	        perror ("Server: listen queue failure");
	        exit (EXIT_FAILURE);
	    }
	    FD_ZERO (&active_set);
	    FD_SET (sock, &active_set);
		while (1)
		{
			char buf[BUFLEN];
			// Проверим, не появились ли данные в каком-либо сокете.
			// В нашем варианте ждем до фактического появления данных.
			read_set = active_set;
			if (select (FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0)
			{
				fprintf (stdout,"SERVER ERROR: select failure\n");
				return -1;
			}
			// Данные появились. Проверим в каком сокете.
			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (FD_ISSET (i, &read_set))
				{
					if (i == sock)
					{
					// пришел запрос на новое соединение
						size = sizeof (client);
						new_sock = accept (sock, (struct sockaddr *) &client, &size);
						if (new_sock < 0)
						{
							fprintf (stdout,"SERVER ERROR: accept socket connection\n");
							close(sock);
							return -1;
						}
						fprintf (stdout, "connect from host %s, port %d.\n",
						inet_ntoa (client.sin_addr),
						(unsigned int) ntohs (client.sin_port));
						FD_SET (new_sock, &active_set);
					}
					else
					{
						// пришли данные в уже существующем соединени
						err = readFromClient (i, buf);
						if (err==1)
						{
							close (i);
							FD_CLR (i, &active_set);
							fprintf (stdout,"stop-command in base by socket %d\n", i);
							return 0;
						}
						else
						if (err < 0)
						{
							close (i);
							FD_CLR (i, &active_set);
							fprintf (stdout,"SERVER ERROR: read error with socket %d\n", i);
						}
						else
						{

							ret = mybase.fun(buf,i);
							if (ret==-1)
							{
								close (i);
								FD_CLR (i, &active_set);
								fprintf (stdout,"quit-command in base by socket %d\n", i);
							}
							if(ret==-2)
							{
								close (i);
								FD_CLR (i, &active_set);
								fprintf (stdout,"stop-command in base by socket %d\n", i);
								return 0;
							}
						}
					}
				}
			}
		}

	}
	return 0;
}
int readFromClient (int fd, char *buf)
{
    int nbytes, i;
    char ch;
    unsigned len;

    // Получаем длину 
    len = 0;
    for (long unsigned int byte = 0; byte < sizeof(unsigned); byte++)
    {
        if ( read (fd, &ch, sizeof (char)) != (int) sizeof (char))
        {
            perror ("read length");
            return -1;
        }

        len |= ((unsigned)ch << (8 * byte));
    }
    assert (len <=  BUFLEN);

    // Получаем len байт
    for (i = 0; len > 0; i += nbytes, len -= nbytes)
    {
        nbytes = read (fd, buf + i, len);
        if (nbytes < 0)
        {
            perror ("Server: ERROR_READ");
            return -1;
        }
        else if (nbytes == 0)
        {
            perror ("Server:READ_TRUNC");
            return -1;
        }
    }

    if (i == 0)
    {
        fprintf (stderr, "Server: no message\n");
        return -1;
    }
    else
    {
        fprintf (stdout, "Server got %d bytes of message: %s\n", i, buf);
        return 0;
    }
}
