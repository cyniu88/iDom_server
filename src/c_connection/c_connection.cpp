#include "c_connection.h"
#include <iostream>
//using namespace std;




pthread_mutex_t C_connection::mutex_buf = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t C_connection::mutex_who = PTHREAD_MUTEX_INITIALIZER;
// konstruktor
C_connection::C_connection (thread_data  *my_data)
{
    c_max_msg = MAX_MSG_LEN*sizeof(int32_t);
    c_from= my_data->from;
    c_socket= my_data->s_client_sock;
    this -> pointer = &my_data->pointer;
    this -> my_data = my_data;
    std::cout << "kostruuuje obiekt do komunikacji  obiekt " <<  std::endl;

}

C_connection::C_connection (thread_data  *my_data, std::string master)
{
    c_max_msg = MAX_MSG_LEN*sizeof(int32_t);
   // c_from= my_data->from;
   // c_socket= my_data->s_gniazdo_klienta;
    this -> pointer = &my_data->pointer;
    this -> my_data = my_data;
    std::cout << "kostruuuje obiekt do komunikacji  obiekt " <<  std::endl;

}

// destruktor
C_connection::~C_connection()
{
    shutdown( c_socket, SHUT_RDWR );
    std::cout << "koniec komunikacji kasuje obiekt " <<  std::endl;
}

int C_connection::c_send(int para)
{
    ChangeEndianness(msg.c_bufor_tmp);
    if(( send( c_socket, msg.char_buf/*c_bufor_tmp*/, c_max_msg, para ) ) <= 0 )
    {
        //perror( "send() ERROR" );
        return -1;
    }
    std::cout << "wyslalem \n";
    return 0;
}

int C_connection::c_send(std::string command)
{

    if (command=="exit")
    {

        for (int i =0 ; i < MAX_MSG_LEN ; i=i+2 )
        {
            msg.c_bufor_tmp[i]= 1 ;
        }
        for (int i =1 ; i < MAX_MSG_LEN ; i=i+2 )
        {
            msg.c_bufor_tmp[i]= 0 ;

        }
    }
    ChangeEndianness(msg.c_bufor_tmp);
    if(( send( c_socket, msg.char_buf /*c_bufor_tmp*/, c_max_msg, 0 ) ) <= 0 )
    {
        //perror( "send() ERROR" );
        return -1;
    }
    return 0;
}

int C_connection::c_recv(int para)
{
    if(( recv( c_socket, msg.char_buf /*c_bufor_tmp*/, c_max_msg, para ) ) <= 0 )
    {
        //perror( "recv() ERROR" );
        return -1;
    }
    ChangeEndianness(msg.c_bufor_tmp);
    for (int i =0 ; i < MAX_MSG_LEN ; ++i)
    {
        std::cout << " " << msg.c_bufor_tmp[i] << " " ;
    }
    std::cout << "odebralem \n";
   // binary(msg.c_bufor_tmp[2]);
    return 0;
}

int32_t C_connection::c_return(int iterator)
{
    return msg.c_bufor_tmp[iterator];
}

void C_connection::c_get(int32_t buffor, int i)
{
    //    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    //    {
    msg.c_bufor_tmp[i]= buffor ;

    // }
}

void C_connection::c_set_buf(int what)

{
    switch ( what)
    {
    case ok:
        for (int i =0 ; i < MAX_MSG_LEN ; i=i+2 )
        {
            msg.c_bufor_tmp[i]= 1 ;
        }
        for (int i =1 ; i < MAX_MSG_LEN ; i=i+2 )
        {
            msg.c_bufor_tmp[i]= 0 ;

        }

    default:

        break;
    }
}

bool C_connection::c_analyse_exit()
{
    if (msg.c_bufor_tmp[3]==0 && msg.c_bufor_tmp[5]==0 && msg.c_bufor_tmp[4]==-1 && msg.c_bufor_tmp[6]==-1 )
    {
        std::cout <<" koniec tego gniazda klienta ";
        std::cout<< "\n bool watek "<< std::endl;                                    // zakoncz ten wątek
        return false;
    }
    return true;
}


int C_connection::c_analyse()
{


      if ( msg.c_bufor_tmp[1]== 13 && msg.c_bufor_tmp[2]== 13 &&  msg.c_bufor_tmp[3]== 31  )
    {
        pthread_mutex_lock(&mutex_who);
        my_data->server_settings->ID_server = msg.c_bufor_tmp[0];
        pthread_mutex_unlock(&mutex_who);
        std::cout << "moj nowy id to "<< my_data->server_settings->ID_server << std::endl;
         c_set_buf( ok);
    }
    //####################################################
    else if ( msg.c_bufor_tmp[0]==22 && msg.c_bufor_tmp[1]==21 &&  msg.c_bufor_tmp[2]==202 &&  msg.c_bufor_tmp[3]==201)
    {
        msg.c_bufor_tmp[0]=msg.c_bufor_tmp[1] = msg.c_bufor_tmp[2]=msg.c_bufor_tmp[3] =  33 ;
        msg.c_bufor_tmp[9] = my_data->server_settings->v_delay ;  // jakie opoznienie w odczytywaniu kto jest na wifi
        std::cout << " opoznienie perwsze " << my_data->server_settings->v_delay << std::endl;
        std::cout << " opoznienie " << msg.c_bufor_tmp[9] << std::endl;
        msg.c_bufor_tmp[7] = my_data->server_settings->A_MAC.size();;   // ile mamy adresow

        for (u_int i =0 ; i < my_data->server_settings->A_MAC.size(); ++i)
        {
            if(( send( c_socket, msg.char_buf /*c_bufor_tmp*/, c_max_msg, MSG_DONTWAIT ) ) <= 0 )
            {
                //  perror( "send() ERROR" );
                break;
            }


            if(( recv( c_socket, msg.char_buf/*c_bufor_tmp*/, c_max_msg, 0 ) ) <= 0 )
            {
                // perror( "recv() ERROR" );
                break;
            }
            c_send_recv_RS232();
            msg.c_bufor_tmp[0]=  msg.c_bufor_tmp[1]=34;
            msg.c_bufor_tmp[2] =0 ;
            msg.c_bufor_tmp[3]=(int32_t)i  ;

            msg.c_bufor_tmp[4] = my_data->server_settings->A_MAC[i].MAC[0];
            msg.c_bufor_tmp[5] = my_data->server_settings->A_MAC[i].MAC[1];

            msg.c_bufor_tmp[6] = my_data->server_settings->A_MAC[i].MAC[3];
            msg.c_bufor_tmp[7] = my_data->server_settings->A_MAC[i].MAC[4];

            msg.c_bufor_tmp[8] = my_data->server_settings->A_MAC[i].MAC[6];
            msg.c_bufor_tmp[9] = my_data->server_settings->A_MAC[i].MAC[7];

            msg.c_bufor_tmp[10] = my_data->server_settings->A_MAC[i].MAC[9];
            msg.c_bufor_tmp[11] = my_data->server_settings->A_MAC[i].MAC[10];

            msg.c_bufor_tmp[12] = my_data->server_settings->A_MAC[i].MAC[12];
            msg.c_bufor_tmp[13] = my_data->server_settings->A_MAC[i].MAC[13];

            msg.c_bufor_tmp[14] = my_data->server_settings->A_MAC[i].MAC[15];
            msg.c_bufor_tmp[15] = my_data->server_settings->A_MAC[i].MAC[16];

            std::cout << " podglad bufora " << (char)msg.c_bufor_tmp [4] << " " << (char)msg.c_bufor_tmp[5]<<std::endl;

        }
    }
    // ############################################3333

      else if (msg.c_bufor_tmp[16]==my_data->server_settings->ID_server || msg.c_bufor_tmp[16]== -1001 )
    {
         c_send_recv_MASTER();
    }

      else if (msg.c_bufor_tmp[16]==RS232)
    {
        //c_send_recv_master(RS232);
        c_send_recv_RS232();
    }

    ///// end /////////








    return true;
}

void C_connection::c_send_recv_MASTER()
{

    c_write_buf(&my_data->server_settings->ID_server);
    c_read_buf(pthread_self());

}



void C_connection::c_send_recv_RS232()
{   int test = RS232;
    c_write_buf(&test);
    c_read_buf(pthread_self());
}


