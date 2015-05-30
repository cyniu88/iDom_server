#include <iostream>
#include <fstream>
#include <strings.h>
#include <string>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
// MOJE BIBLIOTEKI

#include "serialib/serialib.h"
#include "parser/parser.hpp"
#include "wiadomosc/wiadomosc.h"
#include "c_connection/c_connection.h"



#include "functions/functions.h"
//#include "global.h"

//#define MAX_MSG_LEN 16
//#define MAX_CONNECTION 10
//#define FREE 1
//#define RS232 11

//float bufor[ MAX_MSG_LEN ];
//unsigned int who [2]={FREE,FREE};
int max_msg = MAX_MSG_LEN*sizeof(float);

bool go_while = true;
//pthread_mutex_t mutex_buf  = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutex_who  = PTHREAD_MUTEX_INITIALIZER;



//////////// watek wysylajacy/obdbierajacy dane z portu RS232 ////////
void *Send_Recieve_rs232_thread (void *przekaz){
    thread_data_rs232 *data_rs232;

    data_rs232 = (thread_data_rs232*)przekaz;
    serialib port_arduino;   // obiekt port rs232
    std::cout << "otwarcie portu " << (int)port_arduino.Open(data_rs232->portRS232.c_str(), atoi( data_rs232->BaudRate.c_str()))<<std::endl;
    std::cout << "w buforze jest bajtow " << port_arduino.Peek() << std::endl;
    std::cout << " who ma " << data_rs232->pointer.ptr_who[0]<< std::endl;


    while (1)
    {
        usleep(500);
        pthread_mutex_lock(&C_connection::mutex_who);
        if (data_rs232->pointer.ptr_who[0] == RS232)
        {
            pthread_mutex_lock(&C_connection::mutex_buf);

            std::cout<<"  mutex rs232 start \n";
//            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
//            {

//                //bufor[i]+=1;
//                data_rs232->pointer.ptr_buf[i]+=1;
//                //   port_arduino.WriteChar(bufor[i]);
//            }

            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {

              //  std::cout <<" " <<
                data_rs232->pointer.ptr_buf[i]+=1 ;

                //   port_arduino.WriteChar(bufor[i]);
            }
            std::cout << "\n";
            //sleep(15);
            //             port_arduino.Write(bufor, MAX_MSG_LEN);

            //            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            //            {

            //                bufor[i]+=0;

            //            }
            //            //port_arduino.Write(bufor,MAX_MSG_LEN);
            //            //port_arduino.WriteChar('test');

            //            port_arduino.Read(bufor, MAX_MSG_LEN,500);
            std::cout<<"  mutex rs232 koniec \n";
            data_rs232->pointer.ptr_who[0] = data_rs232->pointer.ptr_who[1];
            data_rs232->pointer.ptr_who[1]= RS232;
            // who[1]=RS232;
            std::cout << " idze do watku " << data_rs232->pointer.ptr_who[0]  << std::endl;
            pthread_mutex_unlock(&C_connection::mutex_buf);
        }
        pthread_mutex_unlock(&C_connection::mutex_who);

    }


    pthread_exit(NULL);
}
//////////// watek do obslugi polaczeni miiedzy nodami  //////////////

void *f_serv_con_node (void *data){
    thread_data  *my_data;
        my_data = (thread_data*)data;
    std::cout<<"start watek master \n";
    pthread_detach( pthread_self () );

    C_connection master(my_data, "master");

    master.start_master();



     std::cout<<"koniec  watek master \n";

pthread_exit(NULL);
} //  koniec f_serv_con_node
///////////  watek wymiany polaczenia /////////////////////

void *Server_connectivity_thread(void *przekaz){
    thread_data  *my_data;
    my_data = (thread_data*)przekaz;

    pthread_detach( pthread_self () );

   // std::cout << " przed while  soket " <<my_data->s_v_socket_klienta << std::endl;

    C_connection *client = new C_connection( my_data);

    while (1)
    {
        if( client->c_recv(0) == -1 )
        {
            perror( "recv() ERROR" );
            break;
        }

        // ###########################3  analia wiadomoscu ####################################33//
        if ( client->c_analyse_exit() == false )
        {
            client->c_send("exit");

            go_while = false;
            break;
        }

        client->c_analyse();

        // ###############################33  koniec analizy   wysylanie wyniku do RS232 lub  TCP ########################



        if( client->c_send(0 ) == -1 )
        {
            perror( "send() ERROR" );
            break;
        }


    }

    delete client;
    pthread_exit(NULL);

}
void *main_thread( void * unused)
{

    config server_settings;     // strukruta z informacjami z pliku konfigu
    struct sockaddr_in server;
    int v_socket;
    int max_msg = MAX_MSG_LEN*sizeof(float);
    thread_data m_data;
    thread_data node_data; // przekazywanie do watku
    pthread_t thread_array[MAX_CONNECTION]={0,0,0,0,0,0,0,0,0,0};
    pthread_t rs232_thread_id;
  //  int socket_arry[MAX_CONNECTION];   //cyniu
    unsigned int who[2]={FREE, FREE};
    float bufor[ MAX_MSG_LEN ];
    std::cout << "bede parsowac \n";
    server_settings  =  read_config ( "/etc/config/iDom_server"    );

    std::cout << " ID servera "<< server_settings.ID_server << std::endl;
    std::cout << server_settings.portRS232 << std::endl;
    std::cout << server_settings.BaudRate << std::endl;
    std::cout << server_settings.PORT << std::endl;
    std::cout << server_settings.SERVER_IP << "serwer ip " <<std::endl;

    for (u_int i=0;i<server_settings.A_MAC.size();++i){
        std::cout << server_settings.A_MAC[i].name_MAC<<" "<< server_settings.A_MAC[i].MAC<<" " << server_settings.A_MAC[i].option1 <<
                     " " << server_settings.A_MAC[i].option2<<
                     " " << server_settings.A_MAC[i].option3<<
                     " " << server_settings.A_MAC[i].option4<<
                     " " << server_settings.A_MAC[i].option5<<
                     " " << server_settings.A_MAC[i].option6<<std::endl;
    }
    for (u_int i=0;i<server_settings.AAS.size();++i){
        std::cout << server_settings.AAS[i].id<<" "<< server_settings.AAS[i].SERVER_IP <<std::endl;
    }

    std::cout << " \n";

    // int go = atoi(  server_settings.BaudRate.c_str());

    thread_data_rs232 data_rs232;
    data_rs232.BaudRate=server_settings.BaudRate;
    data_rs232.portRS232=server_settings.portRS232;
    data_rs232.pointer.ptr_buf=bufor;
    data_rs232.pointer.ptr_who=who;
    pthread_create(&rs232_thread_id,NULL,&Send_Recieve_rs232_thread,&data_rs232 );
    std::cout << "-----------------------------------------------\n";

    int SERVER_PORT = atoi(server_settings.PORT.c_str());
    server_settings.SERVER_IP = conv_dns(server_settings.SERVER_IP);
    const char *SERVER_IP = server_settings.SERVER_IP.c_str();

//const char *server_IP = ;

    if (server_settings.ID_server == 1001){
        std::cout << "startuje noda do polaczen z innym \n";

        node_data.server_settings=&server_settings;
        node_data.pointer.ptr_buf=bufor;
        node_data.pointer.ptr_who=who;

        pthread_create (&thread_array[3], NULL,&f_serv_con_node ,&node_data);
        std::cout << " watek wystartowal dla NODA MASTERA "<< thread_array[3] << std::endl;

        pthread_detach( thread_array[3] );


    }

    else
    {
         std::cout << "NIEEE startuje noda do polaczen z innym \n";
    }




    bzero( & server, sizeof( server ) );


    server.sin_family = AF_INET;
    server.sin_port = htons( SERVER_PORT );
    if( inet_pton( AF_INET, SERVER_IP, & server.sin_addr ) <= 0 )
    {
        perror( "inet_pton() ERROR" );
        exit( - 1 );
    }

    if(( v_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "socket() ERROR" );
        exit( - 1 );
    }

    if( fcntl( v_socket, F_SETFL, O_NONBLOCK ) < 0 ) // fcntl()
    {
        perror( "fcntl() ERROR" );
        exit( - 1 );
    }
    // zgub wkurzający komunikat błędu "address already in use"
    int yes =1;
    if( setsockopt( v_socket, SOL_SOCKET, SO_REUSEADDR, & yes, sizeof( int ) ) == - 1 ) {
            perror( "setsockopt" );
            exit( 1 );
        }
    socklen_t len = sizeof( server );
    if( bind( v_socket,( struct sockaddr * ) & server, len ) < 0 )
    {
        perror( "bind() ERROR" );
        exit( - 1 );
    }

    if( listen( v_socket, MAX_CONNECTION ) < 0 )
    {
        perror( "listen() ERROR" );
        exit( - 1 );
    }
    std::cout <<" przed while \n";

    struct sockaddr_in from;
    int v_sock_ind = 0;

    ///////////////////////////////////////////////////// WHILE ////////////////////////////////////////////////////

    while (1)
    {

        bzero( & from, sizeof( from ) );
        if (!go_while) {break;}

        usleep(100000);


        if(( v_sock_ind = accept( v_socket,( struct sockaddr * ) & from, & len ) ) < 0 )
        {
            //perror("accept() ERROR");
            continue;
        }
        for (int con_counter=0; con_counter< MAX_CONNECTION; ++con_counter)
        {
            //std::cout << "                           iteracja "<<con_counter<<std::endl;
           // std::cout << "id watku "<< thread_array[con_counter]<< std::endl;
            // std::cout <<  " stan watku " <<  pthread_detach( thread_array[con_counter] )<< std::endl;

            if ( thread_array[con_counter]==0 || pthread_kill(thread_array[con_counter], 0) == ESRCH )

            {
                if ( con_counter!=MAX_CONNECTION -1)
                {
                    //
                 //   socket_arry[con_counter]=v_sock_ind;
                    m_data.s_client_sock =v_sock_ind;
                    m_data.from=from;
                    m_data.server_settings=&server_settings;
                    m_data.pointer.ptr_buf=bufor;
                    m_data.pointer.ptr_who=who;

                    pthread_create (&thread_array[con_counter], NULL,&Server_connectivity_thread,&m_data);
                    std::cout << " watek wystartowal  "<< thread_array[con_counter] << std::endl;
                    std::cout <<  " taki watek " <<  pthread_detach( thread_array[con_counter] )<< std::endl;
                    pthread_detach( thread_array[con_counter] );
                    break;

                }
                else
                {
                    std::cout<<"za duzo kientow\n";

                    float bufor_tmp[ MAX_MSG_LEN ];


                    if(( recv( v_sock_ind, bufor_tmp, max_msg, 0 ) ) <= 0 )
                    {
                        perror( "recv() ERROR" );
                        break;
                    }
                    for (int i =0 ; i < MAX_MSG_LEN ; ++i )
                    {
                        bufor_tmp[i]= -123 ;

                    }

                    if(( send( v_sock_ind, bufor_tmp, max_msg, MSG_DONTWAIT ) ) <= 0 )
                    {
                        perror( "send() ERROR" );
                        break;
                    }
                    shutdown( v_sock_ind, SHUT_RDWR );
                    continue;

                }

            }


            // shutdown( v_sock_ind, SHUT_RDWR );
            //pthread_exit(NULL);
        }
    } // while

    std::cout << " KOOOOONIEC !!!! \n";
    sleep(4);
    std::cout << " koniec gniazda ma wynik : "<< shutdown( v_socket, SHUT_RDWR );

    std::cout << "\n KOOOOONIEC 2222222222222222222222!!!! \n";
    //    for (int con_counter=0; con_counter< MAX_CONNECTION; ++con_counter)
    //    {
    //        pthread_cancel(thread_array[con_counter]);
    //        pthread_join(thread_array[con_counter], NULL );
    //        shutdown( socket_arry[con_counter], SHUT_RDWR );
    //    }

    pthread_exit(NULL);
} // threade

int main()
{
    pthread_mutex_init(&C_connection::mutex_buf, NULL);
    pthread_mutex_init(&C_connection::mutex_who, NULL);
    // pthread_mutex_init(&mutex_exit, NULL);
    pthread_t  main_th;
    pthread_create (&main_th, NULL,&main_thread,NULL);




    std::cout<<"jestem przed\n";
    pthread_join(main_th ,NULL);
    pthread_mutex_destroy(&C_connection::mutex_buf);
    pthread_mutex_destroy(&C_connection::mutex_who);



    std::cout<<"wychodze\n";

    return 0;
}

//    int * test;
//    int licznik;
//    std::cin >> licznik;
//    test = new int [licznik];
//    for ( int i =0 ; i<licznik; ++i)
//    {
//        test[i]=i+2;
//    }
//    for ( int i =0 ; i<licznik; ++i)
//    {
//        std::cout << " " << test[i]<<std::endl;
//    }
//    std::cin >> licznik;
//    std::cout << " zwalniam \n";
//    delete [] test;
