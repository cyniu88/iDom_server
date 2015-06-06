#include "iDom_server.h"




//////////// watek wysylajacy/obdbierajacy dane z portu RS232 ////////
void *Send_Recieve_rs232_thread (void *przekaz){
    thread_data_rs232 *data_rs232;

    data_rs232 = (thread_data_rs232*)przekaz;
    serialib port_arduino;   // obiekt port rs232

    log_file_mutex.mutex_lock();
    log_file_cout << "otwarcie portu RS232 " << (int)port_arduino.Open(data_rs232->portRS232.c_str(), atoi( data_rs232->BaudRate.c_str()))<<std::endl;
    log_file_cout << "w buforze jest bajtow " << port_arduino.Peek() << std::endl;
     log_file_mutex.mutex_unlock();


    while (1)
    {
        usleep(500);
        pthread_mutex_lock(&C_connection::mutex_who);
        if (data_rs232->pointer.ptr_who[0] == RS232)
        {
            pthread_mutex_lock(&C_connection::mutex_buf);

            //std::cout<<"  mutex rs232 start \n";
            {

                //bufor[i]+=1;
                // data_rs232->pointer.ptr_buf[i]+=1;
                //   port_arduino.WriteChar(bufor[i]);
             }

            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {

              //  std::cout <<" " <<
                data_rs232->pointer.ptr_buf[i]+=1 ;

                //   port_arduino.WriteChar(bufor[i]);
            }
            //std::cout << "\n";
           // sleep(15);
            //             port_arduino.Write(bufor, MAX_MSG_LEN);

            //            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            //            {

            //                bufor[i]+=0;

            //            }
            //            //port_arduino.Write(bufor,MAX_MSG_LEN);
            //            //port_arduino.WriteChar('test');

            //            port_arduino.Read(bufor, MAX_MSG_LEN,500);
            //std::cout<<"  mutex rs232 koniec \n";
            data_rs232->pointer.ptr_who[0] = data_rs232->pointer.ptr_who[1];
            data_rs232->pointer.ptr_who[1]= RS232;
            // who[1]=RS232;
            //std::cout << " idze do watku " << data_rs232->pointer.ptr_who[0]  << std::endl;
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

    master.c_start_master();

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

        // ###########################  analia wiadomoscu ####################################//
        if ( client->c_analyse_exit() == false )
        {
            client->c_send("exit");

            go_while = false;
            break;
        }

        client->c_analyse();

        // ###############################  koniec analizy   wysylanie wyniku do RS232 lub  TCP ########################

        if( client->c_send(0 ) == -1 )
        {
            perror( "send() ERROR" );
            break;
        }
    }

    delete client;
    pthread_exit(NULL);

} ////////////  server_connectivity_thread

void *main_thread( void * unused)
{
    time_t czas;
    struct tm * ptr;
    time( & czas );
    ptr = localtime( & czas );
    std::string data = asctime( ptr );
    config server_settings;     // strukruta z informacjami z pliku konfigu
    struct sockaddr_in server;
    int v_socket;
    int max_msg = MAX_MSG_LEN*sizeof(int32_t);
    thread_data m_data;
    thread_data node_data; // przekazywanie do watku
    pthread_t thread_array[MAX_CONNECTION]={0,0,0,0,0,0,0,0,0,0};
    pthread_t rs232_thread_id;

    unsigned int who[2]={FREE, FREE};
    int32_t bufor[ MAX_MSG_LEN ];


    log_file_mutex.mutex_lock();
    log_file_cout << "\n*****************************************************************\n*****************************************************************\n  "<<  " \t\t\t\t\t start programu " << std::endl;
    log_file_mutex.mutex_unlock();
    server_settings  =  read_config ( "/etc/config/iDom_server"    );

    log_file_mutex.mutex_lock();
    log_file_cout << INFO  << "ID serwera\t"<< server_settings.ID_server << std::endl;
    log_file_cout << INFO  << "PortRS232\t"<< server_settings.portRS232 << std::endl;
    log_file_cout << INFO  << "BaudRate RS232\t"<< server_settings.BaudRate << std::endl;
    log_file_cout << INFO  << "port TCP \t"<< server_settings.PORT << std::endl;
    log_file_cout << INFO  << "serwer ip \t"<< server_settings.SERVER_IP  <<std::endl;
    log_file_cout << INFO  << "dodatkowe NODY w sieci:\n"  <<std::endl;

//    for (u_int i=0;i<server_settings.A_MAC.size();++i){
//        std::cout << server_settings.A_MAC[i].name_MAC<<" "<< server_settings.A_MAC[i].MAC<<" " << server_settings.A_MAC[i].option1 <<
//                     " " << server_settings.A_MAC[i].option2<<
//                     " " << server_settings.A_MAC[i].option3<<
//                     " " << server_settings.A_MAC[i].option4<<
//                     " " << server_settings.A_MAC[i].option5<<
//                     " " << server_settings.A_MAC[i].option6<<std::endl;
//    }
    for (u_int i=0;i<server_settings.AAS.size();++i){
        log_file_cout << INFO << server_settings.AAS[i].id<<" "<< server_settings.AAS[i].SERVER_IP <<std::endl;
    }

    log_file_cout << INFO << " \n" << std::endl;

    // int go = atoi(  server_settings.BaudRate.c_str());

    thread_data_rs232 data_rs232;
    data_rs232.BaudRate=server_settings.BaudRate;
    data_rs232.portRS232=server_settings.portRS232;
    data_rs232.pointer.ptr_buf=bufor;
    data_rs232.pointer.ptr_who=who;
    pthread_create(&rs232_thread_id,NULL,&Send_Recieve_rs232_thread,&data_rs232 );
    log_file_cout << INFO << "-----------------------------------------------"<< std::endl;
    log_file_mutex.mutex_unlock();
    int SERVER_PORT = atoi(server_settings.PORT.c_str());
    server_settings.SERVER_IP = conv_dns(server_settings.SERVER_IP);
    const char *SERVER_IP = server_settings.SERVER_IP.c_str();



    if (server_settings.ID_server == 1001){
        std::cout << "startuje noda do polaczen z innym \n";

        node_data.server_settings=&server_settings;
        node_data.pointer.ptr_buf=bufor;
        node_data.pointer.ptr_who=who;

        pthread_create (&thread_array[3], NULL,&f_serv_con_node ,&node_data);
        log_file_mutex.mutex_lock();
        log_file_cout << INFO << " watek wystartowal dla NODA MASTERA "<< thread_array[3] << std::endl;
        log_file_mutex.mutex_unlock();
        pthread_detach( thread_array[3] );


    }

    else
    {
        log_file_mutex.mutex_lock();
        log_file_cout << INFO << "NIE startuje NODA MASTERA do polaczen z innymi " << std::endl;
        log_file_mutex.mutex_unlock();

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
    if( setsockopt( v_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == - 1 ) {
            perror( "setsockopt" );
            exit( 1 );
        }
    socklen_t len = sizeof( server );
    if( bind( v_socket,( struct sockaddr * ) & server, len ) < 0 )
    {

        log_file_mutex.mutex_lock();
        log_file_cout << CRITICAL << "BIND problem: " <<  strerror(  errno )<< std::endl;
        log_file_mutex.mutex_unlock();
        perror( "bind() ERROR" );
        exit( - 1 );
    }

    if( listen( v_socket, MAX_CONNECTION ) < 0 )
    {
        log_file_mutex.mutex_lock();
        log_file_cout << CRITICAL << "Listen problem: " <<  strerror(  errno )<< std::endl;
        log_file_mutex.mutex_unlock();
        perror( "listen() ERROR" );
        exit( - 1 );
    }
    //std::cout <<" przed while \n";

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

        ////////////////////////   jest połacznie   wiec wstawiamy je  do nowego watku  i  umieszczamy id watku w tablicy  w pierwszym wolnym miejscy ////////////////////
        for (int con_counter=0; con_counter< MAX_CONNECTION; ++con_counter)
        {
            if ( thread_array[con_counter]==0 || pthread_kill(thread_array[con_counter], 0) == ESRCH )   // jesli pozycja jest wolna (0)  to wstaw tam  jesli jest zjęta wyslij sygnal i sprawdz czy waŧek żyje ///

            {
                if ( con_counter!=MAX_CONNECTION -1)
                {

                    m_data.s_client_sock =v_sock_ind;
                    m_data.from=from;
                    m_data.server_settings=&server_settings;
                    m_data.pointer.ptr_buf=bufor;
                    m_data.pointer.ptr_who=who;

                    pthread_create (&thread_array[con_counter], NULL,&Server_connectivity_thread,&m_data);

                    log_file_mutex.mutex_lock();
                    log_file_cout << INFO << " watek wystartowal  "<< thread_array[con_counter] << std::endl;
                    log_file_mutex.mutex_unlock();

                    pthread_detach( thread_array[con_counter] );
                    break;

                }
                else
                {

                    log_file_mutex.mutex_lock();
                    log_file_cout << INFO << " za duzo klientow "<< thread_array[con_counter] << std::endl;
                    log_file_mutex.mutex_unlock();

                    int32_t bufor_tmp[ MAX_MSG_LEN ];


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

    log_file_mutex.mutex_lock();
    log_file_cout << INFO << " koniec programu  "<<   std::endl;
    log_file_mutex.mutex_unlock();

    sleep(2);
    //std::cout << " koniec gniazda ma wynik : "<< shutdown( v_socket, SHUT_RDWR );

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
    pthread_mutex_init(&Logger::mutex_log, NULL);
    pthread_t  main_th;
    pthread_create (&main_th, NULL,&main_thread,NULL);

    pthread_join(main_th ,NULL);
    pthread_mutex_destroy(&Logger::mutex_log);
    pthread_mutex_destroy(&C_connection::mutex_buf);
    pthread_mutex_destroy(&C_connection::mutex_who);

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
