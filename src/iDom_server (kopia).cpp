#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <unistd.h>
// MOJE BIBLIOTEKI
#include "serialib/serialib.h"
#include "parser/parser.hpp"
#include "wiadomosc/wiadomosc.h"

#define MAX_MSG_LEN 16


#define MAX_CONNECTION 10


int main()
{

     konfiguracja ustawienia_servera;     // strukruta z informacjami z pliku konfigu


    ustawienia_servera  =  parsowanie ( "/etc/config/iDom_server"    );

    std::cout << ustawienia_servera.portRS232 << std::endl;
    std::cout << ustawienia_servera.BaudRate << std::endl;
    std::cout << ustawienia_servera.PORT << std::endl;
    std::cout << ustawienia_servera.SERWER_IP << std::endl;
    for (u_int i=0;i<ustawienia_servera.A_MAC.size();++i){
        std::cout << ustawienia_servera.A_MAC[i].name_MAC<<" "<< ustawienia_servera.A_MAC[i].MAC<<" " << ustawienia_servera.A_MAC[i].opcja1<<
                     " " << ustawienia_servera.A_MAC[i].opcja2<<
                     " " << ustawienia_servera.A_MAC[i].opcja3<<
                     " " << ustawienia_servera.A_MAC[i].opcja4<<
                     " " << ustawienia_servera.A_MAC[i].opcja5<<
                     " " << ustawienia_servera.A_MAC[i].opcja6<<std::endl;
    }

    std::cout << " \n";

    int go = atoi(  ustawienia_servera.BaudRate.c_str());


    std::cout << "-----------------------------------------------\n";

    int SERWER_PORT = atoi(ustawienia_servera.PORT.c_str());
    const char *SERWER_IP = ustawienia_servera.SERWER_IP.c_str();

    struct sockaddr_in serwer;
    int gniazdo;
    float bufor[ MAX_MSG_LEN ];
    int max_msg = MAX_MSG_LEN*sizeof(float);


    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    {
        bufor[i]=0;
    }

    bzero( & serwer, sizeof( serwer ) );
    //bzero( bufor, MAX_MSG_LEN );

    serwer.sin_family = AF_INET;
    serwer.sin_port = htons( SERWER_PORT );
    if( inet_pton( AF_INET, SERWER_IP, & serwer.sin_addr ) <= 0 )
    {
        perror( "inet_pton() ERROR" );
        exit( - 1 );
    }

    if(( gniazdo = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "socket() ERROR" );
        exit( - 1 );
    }

    if( fcntl( gniazdo, F_SETFL, O_NONBLOCK ) < 0 ) // fcntl()
    {
        perror( "fcntl() ERROR" );
        exit( - 1 );
    }

    socklen_t len = sizeof( serwer );
    if( bind( gniazdo,( struct sockaddr * ) & serwer, len ) < 0 )
    {
        perror( "bind() ERROR" );
        exit( - 1 );
    }

    if( listen( gniazdo, MAX_CONNECTION ) < 0 )
    {
        perror( "listen() ERROR" );
        exit( - 1 );
    }
    std::cout <<" przed while \n";
    int licz =0;
 ///////////////////////////////////////////////////// WHILE ////////////////////////////////////////////////////
    while( 1 )
    {   ++licz;
        struct sockaddr_in from;
        int gniazdo_clienta = 0;
        bzero( & from, sizeof( from ) );
        printf("Waiting for connection... %i\n",licz);
        usleep(100000);


        if(( gniazdo_clienta = accept( gniazdo,( struct sockaddr * ) & from, & len ) ) < 0 )
        {
            //perror("accept() ERROR");
            continue;
        }
        if(fork() == 0) {

        printf("Jesteś w procesie potomnym, jego ID = %d\n", getpid());
            while (1)
            {
            if(( recv( gniazdo_clienta, bufor, max_msg, 0 ) ) <= 0 )
            {
                perror( "recv() ERROR" );
                exit( - 1 );
            }
            printf( "|Wiadomosc od klienta|:  \n" );
            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                std::cout << bufor[i] << " ";
            }
            char bufor_ip[ 128 ];
            bzero( bufor_ip, sizeof( bufor_ip ) );
            printf( "|Client ip: %s port: %d|\n", inet_ntop( AF_INET, & from.sin_addr, bufor_ip, sizeof( bufor_ip ) ), ntohs( from.sin_port ) );
            printf( "  New connection from: %s:%d\n", inet_ntoa( from.sin_addr ), ntohs( from.sin_port ) );

            //bzero( bufor, MAX_MSG_LEN );

            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                bufor[i]=i-7;
            }

            if(( send( gniazdo_clienta, bufor, max_msg, MSG_DONTWAIT ) ) <= 0 )
            {
                perror( "send() ERROR" );
                exit( - 1 );
            }
           } //while
            shutdown( gniazdo_clienta, SHUT_RDWR );
        }

        /* Otherwise, we must be the parent and our work for this client is
        finished. */

        else {
             printf("Jesteś w procesie macierzystym, jego ID = %d\n", getpid());
        continue;
        }


    }

    shutdown( gniazdo, SHUT_RDWR );


    return 0;
}




