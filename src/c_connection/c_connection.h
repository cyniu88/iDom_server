#ifndef C_CONNECTION_H
#define C_CONNECTION_H
#include "../wiadomosc/wiadomosc.h"
#include <unistd.h>
//#include "global.h"


class C_connection
{

public:

    //C_connection(int _socket, sockaddr_in from, s_pointer _pointer); ;
     C_connection(thread_data  *my_data);
     C_connection(thread_data  *my_data, std::string master);
    ~C_connection();

      thread_data   *my_data;
    int c_socket;
    float c_bufor_tmp[ MAX_MSG_LEN ];
    sockaddr_in c_from;

    int c_max_msg  ;
    static pthread_mutex_t mutex_buf ;// = PTHREAD_MUTEX_INITIALIZER;
    static pthread_mutex_t mutex_who ;//= PTHREAD_MUTEX_INITIALIZER;

    int c_send(int para);
    int c_send(std::string command);
    int c_recv(int para);
    float c_return(int iterator);
    void c_get(float buffor, int i);
    void c_send_recv_RS232 ();
    void c_send_recv_MASTER ();
    int c_analyse();
    bool c_analyse_exit();
    void c_set_buf (int what);
    struct s_pointer *pointer;
     void start_master();
    void c_recv_send_master();

};


#endif // C_CONNECTION_H
