#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF 1024
int main (int argc, char **argv)
{
    int i,j;
    int create_socket;   /*Eine Variable der Wert von Funktion Socket übergeben wird*/
    char *buffer = malloc (BUF);
    struct sockaddr_in address;
    int size, spieler=0;
    if(buffer==NULL) return -1;
    if( argc < 2 )
    {
        printf("Usage: %s ServerAdresse\n", *argv);
        exit(EXIT_FAILURE);
    }

    if ((create_socket=socket (AF_INET, SOCK_STREAM, 0)) > 0) /*socket wird kreiert - bei -1 = Fehler*/
        printf ("Socket wurde angelegt\n");
    address.sin_family = AF_INET; /*struct sockaddr_in steht in einer headerdatei - hier wird mit AF_INET die protokoll familie festgelegt*/
    address.sin_port = htons (15000); /*hier wir port festgelegt und verwandelt von hostbyte order zu networkbyt order*/
    inet_aton (argv[1], &address.sin_addr); /*die funktion verwandelt die IPv4 adresse ins binär form(in network byte order)*/
    if (connect ( create_socket,
                  (struct sockaddr *) &address,
                  sizeof (address)) == 0) /*connenct funktion gibt 0 zurück wenn erfolgreich*/
        printf ("Verbindung mit dem Server (%s) hergestellt\n",
                inet_ntoa (address.sin_addr)); /*umgekehrt von der Funtion inet_aton - sie gibt uns string zurück*/

    do
    {
        size = recv(create_socket, buffer, BUF-1, 0); /*recv gibt anzahl der bytes die es gelesen hat*/
        if( size > 0)
            buffer[size] = '\0';
        if (spieler == 0)
        {
            spieler = buffer[0];
            continue;
        }
        /*Spielfeld ausgeben*/
        printf ("Spielfeld: %s\n", buffer);
        for ( i=0; i < 7; i++)
        {
            printf("1 2 3 4 5 6 7\n");
            printf("-------------\n");
            for ( j=0; j < 6; j++)
            {
                printf("%c ", buffer[5+i+j]);
            }
            printf("\n");
        }
        /*Server abhängig*/ if(buffer[3] ==1 || buffer[3] ==2)  /* gewinner steht fest*/
        {
            /*Server schickt zbs. Z1G0S00000000... Z1, Z2 (Spieler am Zug),G0 G1 G2(gewinner) S00000...(Spielfeld)*/
            printf("spieler %c hat gewonnen\n",buffer[3]); /*somit ist spiel fertig*/
            strcpy(buffer, "quit\n");
        }
        else if (spieler == buffer[1])
        {
            do
            {
                printf ("Du bist am Zug (1-7): ");
                fgets (buffer, BUF, stdin);
            }
            while(!(buffer[0]>'0'&&buffer[0]<='7'));
            buffer[1]='\0';


        }
        else
        {
            printf ("spieler %c ist am Zug\n", buffer[1]);
        }



        send(create_socket, buffer, strlen (buffer), 0); /*0 am ende ist flag*/

    }
    while (strcmp (buffer, "quit\n") != 0);
    /*while (0 != 0); // quit*/
    close (create_socket);
    return EXIT_SUCCESS;
}
