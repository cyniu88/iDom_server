
#include "c_connection.h"
#include <iostream>


void C_connection::c_start_master ()
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
