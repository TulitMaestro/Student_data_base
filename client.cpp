#include <ctime>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cassert>
#include "command.h"

//#define SERVER_PORT 5555
//#define SERVER_NAME "127.0.0.1"
#define BUFLEN 1234


void workWithServer (int fd);
bool readFromServer (int fd);

static int result = 0;

int main (int argc, char *argv[])
{
    int err;
    int sock;
    struct sockaddr_in server_addr;
    struct hostent *hostinfo;
    int port;
    char *host;

    if (argc != 3 || sscanf (argv[2], "%d", &port) != 1)
    {
        printf("Usage: %s host port\n", argv[0]);
        return 1;
    }

    host = argv[1];

    // Получаем информацию о сервере по его DNS имени
    // или точечной нотации IP адреса.
    hostinfo = gethostbyname (host);
    if (hostinfo == NULL)
    {
        fprintf (stderr, "Unknown host %s.\n", host);
        exit (EXIT_FAILURE);
    }

    // Заполняем адресную структуру для последующего
    // использования при установлении соединения
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons (port);
    server_addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;

    //for (i=0;i<100000;i++) {
    // Создаем TCP сокет.
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror ("Client: socket was not created");
        exit (EXIT_FAILURE);
    }

    // Устанавливаем соединение с сервером
    err = connect (sock, (struct sockaddr *) &server_addr, sizeof (server_addr));
    if (err < 0)
    {
        perror ("Client: connect failure");
        exit (EXIT_FAILURE);
    }
    fprintf (stdout, "Connection is ready\n");

    // Обмениваемся данными
    double time = clock ();
    workWithServer (sock);
    time = (clock() - time) / CLOCKS_PER_SEC;

    fprintf (stderr, "%s : Result = %d Elapsed = %.2f\n", argv[0], result, time);

    close (sock);
    //}
    exit (EXIT_SUCCESS);
}


void workWithServer (int fd)
{
    int nbytes, len, i;
    char buf[BUFLEN];

    while (1)
    {
        if(!(fgets (buf, BUFLEN, stdin)))
        {
            perror ("get error");
            exit (EXIT_FAILURE);
        }
        for (len = 0; buf[len]; len++);

        if(len==0)
        {
            printf("len=0\n");
            return;
        }
        len++;
        // Пересылаем длину сообщения побайтово, от последнего байта к первому
        const int mask = (1 << 8) - 1;
        char buf_ch;
        for (long unsigned byte = 0; byte < sizeof(int); byte++)
        {
            buf_ch = (char)( (len >> (8*byte)) & mask);
            if (write (fd, &buf_ch, sizeof (char)) != (int) sizeof (char))
            {
                perror ("write length");
                exit (EXIT_FAILURE);
            }
        }

        // Пересылаем len байт
        for (i = 0; len > 0; i += nbytes, len -= nbytes)
        {
            nbytes = write (fd, buf + i, len);
            if (nbytes < 0)
            {
                perror ("write");
                exit (EXIT_FAILURE);
            }
            else if (nbytes == 0)
            {
                perror ("write truncated");
                exit (EXIT_FAILURE);
            }
        }

        bool res = readFromServer (fd);
        if (res == true)
            return;
    }
}


bool readFromServer (int fd)    
{
    int nbytes, i;
    unsigned len;
    char buf[BUFLEN];
    codes_client_server errcode;

    while (1)
    {
        
        // получаем однобайтовый код окончания
        if (read (fd, &errcode, sizeof (codes_client_server)) != (int) sizeof (codes_client_server))
        {
            // ошибка чтения
            perror ("read length");
            exit (EXIT_FAILURE);
        }
        switch (errcode)
        {
            case codes_client_server::SUCCESS:
                break;
            case codes_client_server::QUIT_OR_STOP:
                printf("QUIT or STOp\n");
                return true;
            case codes_client_server::TROUBLE:
                perror ("trouble on server");
                exit (EXIT_FAILURE);
                return false;
            case codes_client_server::OUTPUT_END:
                return false;
            
        }
        result++;
        len = 0;
        char ch;
        for (long unsigned int byte = 0; byte < sizeof(unsigned); byte++)
        {
            if ( read (fd, &ch, sizeof (char)) != (int) sizeof (char))
            {
                perror ("read length");
                exit (EXIT_FAILURE);
            }

            len |= ((unsigned)ch << (8 * byte));
        }
        for (i = 0; len > 0; i += nbytes, len -= nbytes)
        {
            nbytes = read (fd, buf + i, len);
            if (nbytes < 0)
            {
                perror ("read");
                exit (EXIT_FAILURE);
            }
            else if (nbytes == 0)
            {
                perror ("read truncated");
                exit (EXIT_FAILURE);
            }

        }
        if (i != 0)
        {
            fprintf (stdout, "%s\n", buf);
        }

    }
    return true;
}