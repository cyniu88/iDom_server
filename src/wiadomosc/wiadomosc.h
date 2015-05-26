#ifndef WIADOMOSC_H
#define WIADOMOSC_H
#include <string>


#include <netinet/in.h>



#include "../parser/parser.hpp"


#define MAX_MSG_LEN 18
#define MAX_CONNECTION 10
#define FREE 1
#define RS232 11
#define ok     0



struct s_pointer{
    unsigned int *ptr_who;
    float *ptr_buf;
};


//struct s_arg{
//    serialib port_arduino;
//    struct struktura_wiadomosci s_msg;
//    union unia_wiadomosci s_unia_msg;
//    struct struktura_wiadomosci r_msg;
//    union unia_wiadomosci r_unia_msg;
//};


struct thread_data{
    int s_client_sock;
    struct sockaddr_in from;
    struct config *server_settings;
    struct s_pointer pointer;


};

struct thread_data_rs232{

    std::string portRS232;
    std::string BaudRate;
    struct s_pointer pointer;

};

int parser_bufor ( float bufor_tmp[]);





#endif // WIADOMOSC_H
