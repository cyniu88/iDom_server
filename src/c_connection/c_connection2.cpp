
#include "c_connection.h"
#include <iostream>

void C_connection::c_write_buf(int *to){

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
            //std::cout <<" po kopiowaniu \n";
            pthread_mutex_unlock(&mutex_buf);

            //who[0]= RS232;
            pointer->ptr_who[0] = *to;
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
    } // end while

}


void C_connection::c_read_buf(int c_pthread_self)
{

    while (1)
    {
        usleep(500);
        // std::cout << " sprawdzam czy jest moj watek  " << pthread_self ()<< std::endl;
        pthread_mutex_lock(&mutex_who);
        if (pointer->ptr_who[0] == c_pthread_self  )
        {

            pthread_mutex_lock(&mutex_buf);
            for (int i =0 ; i < MAX_MSG_LEN ; ++i )
            {
                c_get(pointer->ptr_buf[i],i);

            }

            //std::cout <<" po kopiowaniu \n";
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







