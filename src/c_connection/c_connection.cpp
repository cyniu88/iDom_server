#include "c_connection.h"
#include <iostream>


pthread_mutex_t C_connection::mutex_buf = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t C_connection::mutex_who = PTHREAD_MUTEX_INITIALIZER;
// konstruktor
C_connection::C_connection (thread_data  *my_data)
{
    c_max_msg = MAX_MSG_LEN*sizeof(float);
    c_from= my_data->from;
    c_socket= my_data->s_client_sock;
    this -> pointer = &my_data->pointer;
    this -> my_data = my_data;
    std::cout << "kostruuuje obiekt do komunikacji  obiekt " <<  std::endl;

}

C_connection::C_connection (thread_data  *my_data, std::string master)
{
    c_max_msg = MAX_MSG_LEN*sizeof(float);
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
    if(( send( c_socket, c_bufor_tmp, c_max_msg, para ) ) <= 0 )
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
            c_bufor_tmp[i]= 1 ;
        }
        for (int i =1 ; i < MAX_MSG_LEN ; i=i+2 )
        {
            c_bufor_tmp[i]= 0 ;

        }
    }
    if(( send( c_socket, c_bufor_tmp, c_max_msg, 0 ) ) <= 0 )
    {
        //perror( "send() ERROR" );
        return -1;
    }
    return 0;
}

int C_connection::c_recv(int para)
{
    if(( recv( c_socket, c_bufor_tmp, c_max_msg, para ) ) <= 0 )
    {
        //perror( "recv() ERROR" );
        return -1;
    }
    return 0;
}

float C_connection::c_return(int iterator)
{
    return c_bufor_tmp[iterator];
}

void C_connection::c_get(float buffor, int i)
{
    //    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
    //    {
    c_bufor_tmp[i]= buffor ;

    // }
}

void C_connection::c_set_buf(int what)

{
    switch ( what)
    {
    case ok:
        for (int i =0 ; i < MAX_MSG_LEN ; i=i+2 )
        {
            c_bufor_tmp[i]= 1 ;
        }
        for (int i =1 ; i < MAX_MSG_LEN ; i=i+2 )
        {
            c_bufor_tmp[i]= 0 ;

        }

    default:

        break;
    }
}

bool C_connection::c_analyse_exit()
{
    if (c_bufor_tmp[3]==0 && c_bufor_tmp[5]==0 && c_bufor_tmp[4]==-1 && c_bufor_tmp[6]==-1 )
    {
        std::cout <<" koniec tego gniazda klienta ";
        std::cout<< "\n bool watek "<< std::endl;                                    // zakoncz ten wątek
        return false;
    }
    return true;
}


int C_connection::c_analyse()
{


      if ( c_bufor_tmp[1]== 13 && c_bufor_tmp[2]== 13 &&  c_bufor_tmp[3]== 31  )
    {

        my_data->server_settings->ID_server = c_bufor_tmp[0];
        std::cout << "moj nowy id to "<< my_data->server_settings->ID_server << std::endl;
         c_set_buf( ok);
    }
    //####################################################
    else if ( c_bufor_tmp[0]==22 && c_bufor_tmp[1]==21 &&  c_bufor_tmp[2]==202 &&  c_bufor_tmp[3]==201)
    {
        c_bufor_tmp[0]=c_bufor_tmp[1] = c_bufor_tmp[2]=c_bufor_tmp[3] =  33 ;
        c_bufor_tmp[9] = my_data->server_settings->v_delay ;  // jakie opoznienie w odczytywaniu kto jest na wifi
        std::cout << " opoznienie perwsze " << my_data->server_settings->v_delay << std::endl;
        std::cout << " opoznienie " << c_bufor_tmp[9] << std::endl;
        c_bufor_tmp[7] = my_data->server_settings->A_MAC.size();;   // ile mamy adresow

        for (u_int i =0 ; i < my_data->server_settings->A_MAC.size(); ++i)
        {
            if(( send( c_socket, c_bufor_tmp, c_max_msg, MSG_DONTWAIT ) ) <= 0 )
            {
                //  perror( "send() ERROR" );
                break;
            }


            if(( recv( c_socket, c_bufor_tmp, c_max_msg, 0 ) ) <= 0 )
            {
                // perror( "recv() ERROR" );
                break;
            }
            c_send_recv_RS232();
            c_bufor_tmp[0]=  c_bufor_tmp[1]=34;
            c_bufor_tmp[2] =0 ;
            c_bufor_tmp[3]=(float)i  ;

            c_bufor_tmp[4] = my_data->server_settings->A_MAC[i].MAC[0];
            c_bufor_tmp[5] = my_data->server_settings->A_MAC[i].MAC[1];

            c_bufor_tmp[6] = my_data->server_settings->A_MAC[i].MAC[3];
            c_bufor_tmp[7] = my_data->server_settings->A_MAC[i].MAC[4];

            c_bufor_tmp[8] = my_data->server_settings->A_MAC[i].MAC[6];
            c_bufor_tmp[9] = my_data->server_settings->A_MAC[i].MAC[7];

            c_bufor_tmp[10] = my_data->server_settings->A_MAC[i].MAC[9];
            c_bufor_tmp[11] = my_data->server_settings->A_MAC[i].MAC[10];

            c_bufor_tmp[12] = my_data->server_settings->A_MAC[i].MAC[12];
            c_bufor_tmp[13] = my_data->server_settings->A_MAC[i].MAC[13];

            c_bufor_tmp[14] = my_data->server_settings->A_MAC[i].MAC[15];
            c_bufor_tmp[15] = my_data->server_settings->A_MAC[i].MAC[16];

            std::cout << " podglad bufora " << (char)c_bufor_tmp [4] << " " << (char)c_bufor_tmp[5]<<std::endl;

        }
    }
    // ############################################3333

      else if (c_bufor_tmp[16]==1001)
    {
         c_send_recv_MASTER();
    }

      else if (c_bufor_tmp[16]==RS232)
    {
        //c_send_recv_master(RS232);
        c_send_recv_RS232();
    }
    ///// end /////////








    return true;
}

void C_connection::c_send_recv_MASTER()
{


    while (1)
    {
        usleep(500);
        std::cout << " sprawdzam czy hest FREE " <<pointer->ptr_who[0] <<" \n";
        pthread_mutex_lock(&mutex_who);
        if (  pointer->ptr_who[0] ==FREE )
        {

            pthread_mutex_lock(&mutex_buf);
            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                // bufor[i] =  c_return(i);
                pointer->ptr_buf[i]= c_return(i);

            }
            std::cout <<" po kopiowaniu \n";
            pthread_mutex_unlock(&mutex_buf);

            //who[0]= RS232;
            pointer->ptr_who[0] = my_data->server_settings->ID_server;
            //who[1]=  pthread_self ();
            pointer->ptr_who[1] =  pthread_self ();
            pthread_mutex_unlock(&mutex_who);
            break;
        }
        else
        {
            std::cout <<" szukma FREE "<< pthread_self ()  << std::endl;
            pthread_mutex_unlock(&mutex_who);

            continue;
        }
        pthread_mutex_unlock(&mutex_who);
    } // end wfile



    while (1)
    {
        usleep(500);
        // std::cout << " sprawdzam czy jest moj watek  " << pthread_self ()<< std::endl;
        pthread_mutex_lock(&mutex_who);
        if (pointer->ptr_who[0] ==pthread_self () )
        {

            pthread_mutex_lock(&mutex_buf);
            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                c_get(pointer->ptr_buf[i],i);

            }


            std::cout <<" po kopiowaniu \n";
            pthread_mutex_unlock(&mutex_buf);

            //who[0]= FREE;
            pointer->ptr_who[0]= FREE;
            pthread_mutex_unlock(&mutex_who);
            break;
        }
        else
        {
            std::cout <<" nie moje "<< pthread_self ()  << std::endl;
            pthread_mutex_unlock(&mutex_who);

            continue;
        }
        pthread_mutex_unlock(&mutex_who);
    } // end while

}



void C_connection::c_send_recv_RS232()
{


    while (1)
    {
        usleep(500);
        std::cout << " sprawdzam czy hest FREE " <<pointer->ptr_who[0] <<" \n";
        pthread_mutex_lock(&mutex_who);
        if (  pointer->ptr_who[0] ==FREE )
        {

            pthread_mutex_lock(&mutex_buf);
            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                // bufor[i] =  c_return(i);
                pointer->ptr_buf[i]= c_return(i);

            }
            std::cout <<" po kopiowaniu \n";
            pthread_mutex_unlock(&mutex_buf);

            //who[0]= RS232;
            pointer->ptr_who[0] = RS232;
            //who[1]=  pthread_self ();
            pointer->ptr_who[1] =  pthread_self ();
            pthread_mutex_unlock(&mutex_who);
            break;
        }
        else
        {
            std::cout <<" szukma FREE "<< pthread_self ()  << std::endl;
            pthread_mutex_unlock(&mutex_who);

            continue;
        }
        pthread_mutex_unlock(&mutex_who);
    } // end wfile



    while (1)
    {
        usleep(500);
        // std::cout << " sprawdzam czy jest moj watek  " << pthread_self ()<< std::endl;
        pthread_mutex_lock(&mutex_who);
        if (pointer->ptr_who[0] ==pthread_self () )
        {

            pthread_mutex_lock(&mutex_buf);
            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                c_get(pointer->ptr_buf[i],i);

            }


            std::cout <<" po kopiowaniu \n";
            pthread_mutex_unlock(&mutex_buf);

            //who[0]= FREE;
            pointer->ptr_who[0]= FREE;
            pthread_mutex_unlock(&mutex_who);
            break;
        }
        else
        {
            std::cout <<" nie moje "<< pthread_self ()  << std::endl;
            pthread_mutex_unlock(&mutex_who);

            continue;
        }
        pthread_mutex_unlock(&mutex_who);
    } // end while

}

void C_connection::start_master ()
{

    std::cout <<  " w pamieci jest " <<my_data->server_settings->AAS.size() <<" nodow\n";
    for (unsigned int i =0 ; i<my_data->server_settings->AAS.size() ; ++i )
    {
        std::cout << "  jest ustawien id " <<  my_data->server_settings->AAS[i].id << std::endl;
        std::cout << "  jest ustawien ip " <<  my_data->server_settings->AAS[i].SERVER_IP << std::endl;
    }
        while (1)
        {

             //c_recv_master_to();
             c_recv_send_master();
             std::cout << " odebralem w masterze \n";
             //c_send_master_to(c_bufor_tmp[16]);

        }


}


void C_connection::c_recv_send_master()
{


    while (1)
    {
        usleep(500);
       // std::cout << " sprawdzam czy to do mastera " << pointer->ptr_who[0] <<" \n";
        pthread_mutex_lock(&mutex_who);
        if (  pointer->ptr_who[0] == my_data->server_settings->ID_server)
        {

            pthread_mutex_lock(&mutex_buf);

            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                 pointer->ptr_buf[i]-=1;

            }

            pointer->ptr_who[0] =  pointer->ptr_who[1] ;//to;
            //who[1]=  pthread_self ();
            pointer->ptr_who[1] =  my_data->server_settings->ID_server;


            pthread_mutex_unlock(&mutex_buf);
        }
        pthread_mutex_unlock(&mutex_who);
    } // end while

}

