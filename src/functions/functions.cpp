
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <netdb.h>
// przerobka  adresu na ip . //////////////////////////////////
std::string conv_dns (std::string temp){

int i;
struct hostent * he;
struct in_addr ** addr_list;
std::string s_ip;
//const char *buf =temp.c_str();
//scanf( "%99s", buf );

if(( he = gethostbyname( temp.c_str() ) ) == NULL )
{
    herror( "gethostbyname" );
    return "- 1";
}

// print information about this host:
printf( "Official name is: %s\n", he->h_name );

printf( "IP addresses: " );
addr_list =( struct in_addr ** ) he->h_addr_list;
for( i = 0; addr_list[ i ] != NULL; i++ )
{
    printf( "%s ", inet_ntoa( * addr_list[ i ] ) );
    s_ip += inet_ntoa( * addr_list[ i ] );

}
printf( "\n" );

return s_ip;
}
