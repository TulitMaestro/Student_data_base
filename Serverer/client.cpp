#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
// Определимся с портом, адресом сервера и другими константами.
// В данном случае берем произвольный порт и адрес обратной связи
// (тестируем на одной машине).
//#define SERVER_PORT 5555
//#define SERVER_NAME "127.0.0.1"
#define BUFLEN 512
// Две вспомогательные функции для чтения/записи (см. ниже)
void writeToServer (int fd);
void readFromServer (int fd);
int main (int argc,char *argv[])
{
    int i;
    int SERVER_PORT=5555;
    int err;
    int sock;
    char *SERVER_NAME =nullptr;
    struct sockaddr_in server_addr;
    struct hostent *hostinfo;
    // Получаем информацию о сервере по его DNS имени
    // или точечной нотации IP адреса.
    if(!(argc==3 && sscanf(argv[2],"%d",&SERVER_PORT)==1))
    {
        printf("ERROR Client INPUT\n");
        exit (EXIT_FAILURE);

    }
    SERVER_NAME=argv[1];
    hostinfo = gethostbyname (SERVER_NAME);
    if (hostinfo == NULL)
    {
        fprintf (stderr, "Unknown host %s.\n", SERVER_NAME);
        exit (EXIT_FAILURE);
    }
    // Заполняем адресную структуру для последующего
    // использования при установлении соединения
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons (SERVER_PORT);
    server_addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;

    for (i=0;i<100000;i++)
    {
        // Создаем TCP сокет.
        sock = socket (PF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror ("Client: socket was not created");
            exit (EXIT_FAILURE);
        }
        // Устанавливаем соединение с сервером
        err =connect (sock, (struct sockaddr *) &server_addr, sizeof (server_addr));
        if (err < 0)
        {
            perror ("Client: connect failure");
            exit (EXIT_FAILURE);
        }
        fprintf (stdout, "Connection is ready\n");
        // Обмениваемся данными
      
        writeToServer (sock);
        readFromServer (sock);
        // Закрываем socket

        close (sock);
    }
    exit (EXIT_SUCCESS);
}
void writeToServer (int fd)
{
    int nbytes, len, i;
    char buf[BUFLEN];
    fprintf (stdout, "Send to server > ");
    if(!(fgets (buf, BUFLEN, stdin)))
    {
         perror ("get error");
         exit (EXIT_FAILURE);
    }
    // Удаляем завершающий символ ’\n’ (если есть)
    for (len = 0; buf[len]; len++)
    if (buf[len] == '\n')
    {
        buf[len] = 0;
        break;
    }
    // Длина сообщения
    len++;
    // Пересылаем длину сообщения
    if (write (fd, &len, sizeof (int)) != (int) sizeof (int))
    {
        perror ("write length");
        exit (EXIT_FAILURE);
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
}
void readFromServer (int fd)
{
    int nbytes, len, i;
    char buf[BUFLEN];
    // Получаем длину сообщения
    if (read (fd, &len, sizeof (int)) != (int) sizeof (int))
    {
        // ошибка чтения
        perror ("read length");
        exit (EXIT_FAILURE);
    }
    // Получаем len байт
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
    // Длина находится в i
    if (i == 0)
    {
        // нет данных
        fprintf (stderr, "Client: no message\n");
    }
    else
    {
        // ответ успешно прочитан
        fprintf (stdout, "Server’s replay: %s\n", buf);
    }
}