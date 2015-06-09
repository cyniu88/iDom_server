#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "parser.hpp"


config read_config  ( const char* file_path    )
{
    int counter  = 1;
    int counter2 = 1;
    std::string s_mac="MAC1";
    std::string s_node="NODE1";
    std::string content ;
    std::string variable ;
    std::string v_value ;
    std::fstream config_file;

    config  v_set;

    config_file.open(file_path, std::ios::in  );
    if( config_file.good() == false )
    {
        log_file_mutex.mutex_lock();
        log_file_cout << "Brak pliku konfiguracyjnego" << std::endl;
        log_file_mutex.mutex_unlock();

        return v_set;
    }

    while( !config_file.eof() )
    {
        getline (config_file, content);

        for (unsigned i=0;  i<content.length(); ++i)
        {
            if ( content.at(i)==':' && content.at(i+1)=='=')
            {   for (unsigned int j=i+2;j<content.length(); ++j )
                {
                    if ( content.at(j)==' '|| content.at(j)==0x9 ){   // przerwij odczyt  jesli znajdzesz spacje lub tab
                        break;
                    }
                    // std::cout << (int)content.at(j);
                    v_value.push_back(content.at(j));  // v_value zmiennej
                }

                // std::cout << std::endl;
                break;
            }
            if (content.at(i)!=' ' )
            {
                variable.push_back(content.at(i));  // variable
            }
        }
        
        if (variable == "portRS232")
        {
            v_set.portRS232= v_value;
        }
        if (variable == "ID")
        {


            v_set.ID_server = atoi(  v_value.c_str());
        }
        if (variable == "SERVER_IP")
        {
            v_set.SERVER_IP= v_value;
        }
        if (variable == "BaudRate")
        {
            v_set.BaudRate= v_value;
        }
        if (variable == "DELAY")
        {
            v_set.v_delay  = atoi(v_value.c_str());
        }
        if (variable == "PORT")
        {
            v_set.PORT= v_value;
        }

        if (variable == s_mac)
        {    addresses_mac temp_a_mac;
            temp_a_mac.name_MAC=s_mac;
            temp_a_mac.MAC=v_value;

            for (unsigned int i=0 ; i< content.size();++i)
            {
                if (content.at(i)==0x9 && content.at(++i)==0x9)
                {
                    temp_a_mac.option1= content.at(i+1)-((int)'0');
                    temp_a_mac.option2= content.at(i+3)-((int)'0');
                    temp_a_mac.option3= content.at(i+5)-((int)'0');
                    temp_a_mac.option4= content.at(i+7)-((int)'0');
                    temp_a_mac.option5= content.at(i+9)-((int)'0');
                    temp_a_mac.option6= content.at(i+11)-((int)'0');
                }
            }
            v_set.A_MAC.insert(v_set.A_MAC.begin() + counter-1, temp_a_mac);
            s_mac="MAC";
            ++counter;
            s_mac.push_back((char)(((int)'0')+counter));
            //  std::cout << " content "<< content << std::endl;


        }

        if (variable == s_node)
        {
           address_another_servers temp_a_node;
            temp_a_node.id=atoi( v_value.c_str());
           // std::cout << " id noda z v_valuei " << v_value <<std::endl;
            variable="";
            for (unsigned int i=0 ; i< content.size();++i)
            {
                if (content.at(i)==0x27)
                {

                    ++i;
                    while (content.at(i)!= 0x27)
                    {

                        variable.push_back(content.at(i));
                        // std::cout << " jestem w while   "<< content.at(i) <<std::endl;
                        ++i;
                    }
                    break;
                }

            }
            temp_a_node.SERVER_IP =variable;
           // std::cout << " laduje do kontenera  " << counter2-1 << std::endl;
            v_set.AAS.insert(v_set.AAS.begin() + counter2-1, temp_a_node);

            // std::cout << "zaladowalem do kontenera \n ";
            s_node="NODE";
            ++counter2;
            s_node.push_back((char)(((int)'0')+counter2));
          //  std::cout << " id noda "<< v_set.AAS[counter2-1].id << std::endl;


        }
        variable = v_value = "";
    }




    config_file.close();
    


    return v_set;
}
