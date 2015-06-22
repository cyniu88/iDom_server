#ifndef C_CONNECTION_H
#define C_CONNECTION_H
#include "../wiadomosc/wiadomosc.h"
#include "../functions/functions.h"
#include "../logger/logger.hpp"
#include <unistd.h>
//#include "global.h"
 extern Logger log_file_mutex;
 extern char * _logfile;
class C_connection
{

public:

    //C_connection(int _socket, sockaddr_in from, s_pointer _pointer); ;
     C_connection(thread_data  *my_data);
     C_connection(thread_data  *my_data, std::string master);
    ~C_connection();

      thread_data   *my_data;
    int c_socket;
    //int32_t c_bufor_tmp[ MAX_MSG_LEN ];
    sockaddr_in c_from;

    union conv{
        int32_t c_bufor_tmp[ MAX_MSG_LEN ];
        char char_buf[4*MAX_MSG_LEN];
    } msg;

    int c_max_msg  ;

    static pthread_mutex_t mutex_buf ;// = PTHREAD_MUTEX_INITIALIZER;
    static pthread_mutex_t mutex_who ;//= PTHREAD_MUTEX_INITIALIZER;
    struct s_pointer *pointer;


     int c_send(int para);
     int c_send(std::string command);
     int c_recv(int para);
 int32_t c_return(int iterator);
    void c_get(int32_t buffor, int i);
    void c_send_recv_RS232 ();
    void c_read_buf (int c_pthread_self);
    void c_write_buf(int *to);
    void c_send_recv_MASTER ();
     int c_analyse();
    bool c_analyse_exit();
    void c_set_buf (int what);


    // metody dla noda
     void c_start_master();
    void c_recv_send_master();

     // wysylanie loga
    void l_send_log();

};


#endif // C_CONNECTION_H
