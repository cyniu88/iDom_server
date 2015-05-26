#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "parser.hpp"


konfiguracja parsowanie( const char* sciezka_pliku    )
{
    int licznik  = 1;
    int licznik2 = 1;
    std::string s_mac="MAC1";
    std::string s_node="NODE1";
    std::string zawartosc ;
    std::string zmienna ;
    std::string wartosc ;
    std::fstream config;

    konfiguracja ustawienia;

    config.open(sciezka_pliku, std::ios::in  );
    if( config.good() == false )
    {
        std::cout << "Brak pliku konfiguracyjnego" << std::endl;
        return ustawienia;
    }

    while( !config.eof() )
    {
        getline (config, zawartosc);

        for (unsigned i=0;  i<zawartosc.length(); ++i)
        {
            if ( zawartosc.at(i)==':' && zawartosc.at(i+1)=='=')
            {   for (unsigned int j=i+2;j<zawartosc.length(); ++j )
                {
                    if ( zawartosc.at(j)==' '|| zawartosc.at(j)==0x9 ){   // przerwij odczyt  jesli znajdzesz spacje lub tab
                        break;
                    }
                    // std::cout << (int)zawartosc.at(j);
                    wartosc.push_back(zawartosc.at(j));  // wartosc zmiennej
                }

                // std::cout << std::endl;
                break;
            }
            if (zawartosc.at(i)!=' ' )
            {
                zmienna.push_back(zawartosc.at(i));  // zmienna
            }
        }
        
        if (zmienna == "portRS232")
        {
            ustawienia.portRS232= wartosc;
        }
        if (zmienna == "ID")
        {


            ustawienia.ID_server = atoi(  wartosc.c_str());
        }
        if (zmienna == "SERWER_IP")
        {
            ustawienia.SERWER_IP= wartosc;
        }
        if (zmienna == "BaudRate")
        {
            ustawienia.BaudRate= wartosc;
        }
        if (zmienna == "DELAY")
        {
            ustawienia.v_delay  = atoi(wartosc.c_str());
        }
        if (zmienna == "PORT")
        {
            ustawienia.PORT= wartosc;
        }

        if (zmienna == s_mac)
        {    adresy_mac temp_a_mac;
            temp_a_mac.name_MAC=s_mac;
            temp_a_mac.MAC=wartosc;

            for (unsigned int i=0 ; i< zawartosc.size();++i)
            {
                if (zawartosc.at(i)==0x9 && zawartosc.at(++i)==0x9)
                {
                    temp_a_mac.opcja1= zawartosc.at(i+1)-((int)'0');
                    temp_a_mac.opcja2= zawartosc.at(i+3)-((int)'0');
                    temp_a_mac.opcja3= zawartosc.at(i+5)-((int)'0');
                    temp_a_mac.opcja4= zawartosc.at(i+7)-((int)'0');
                    temp_a_mac.opcja5= zawartosc.at(i+9)-((int)'0');
                    temp_a_mac.opcja6= zawartosc.at(i+11)-((int)'0');
                }
            }
            ustawienia.A_MAC.insert(ustawienia.A_MAC.begin() + licznik-1, temp_a_mac);
            s_mac="MAC";
            ++licznik;
            s_mac.push_back((char)(((int)'0')+licznik));
            //  std::cout << " zawartosc "<< zawartosc << std::endl;


        }

        if (zmienna == s_node)
        {
            addres_another_servers temp_a_node;
            temp_a_node.id=atoi( wartosc.c_str());
            std::cout << " id noda z wartosci " << wartosc <<std::endl;
            zmienna="";
            for (unsigned int i=0 ; i< zawartosc.size();++i)
            {
                if (zawartosc.at(i)==0x27)
                {

                    ++i;
                    while (zawartosc.at(i)!= 0x27)
                    {

                        zmienna.push_back(zawartosc.at(i));
                         std::cout << " jestem w while   "<< zawartosc.at(i) <<std::endl;
                        ++i;
                    }
                    break;
                }

            }
            temp_a_node.SERVER_IP =zmienna;
           // std::cout << " laduje do kontenera  " << licznik2-1 << std::endl;
            ustawienia.AAS.insert(ustawienia.AAS.begin() + licznik2-1, temp_a_node);

            // std::cout << "zaladowalem do kontenera \n ";
            s_node="NODE";
            ++licznik2;
            s_node.push_back((char)(((int)'0')+licznik2));
          //  std::cout << " id noda "<< ustawienia.AAS[licznik2-1].id << std::endl;


        }
        zmienna = wartosc = "";
    }




    config.close();
    


    return ustawienia;
}
/*
addres_another_servers  read_routing_table ( const char* sciezka_pliku    )
{
    addres_another_servers t;
    config.open(sciezka_pliku, std::ios::in  );
    if( config.good() == false )
    {
        std::cout << "Brak pliku konfiguracyjnego" << std::endl;
        return t;
    }

    while( !config.eof() )
    {
        getline (config, zawartosc);

        for (unsigned i=0;  i<zawartosc.length(); ++i)
        {
            if ( zawartosc.at(i)==':' && zawartosc.at(i+1)=='=')
            {   for (unsigned int j=i+2;j<zawartosc.length(); ++j )
                {
                    if ( zawartosc.at(j)==' '|| zawartosc.at(j)==0x9 ){   // przerwij odczyt  jesli znajdzesz spacje lub tab
                        break;
                    }
                    // std::cout << (int)zawartosc.at(j);
                    wartosc.push_back(zawartosc.at(j));  // wartosc zmiennej
                }

                // std::cout << std::endl;
                break;
            }
            if (zawartosc.at(i)!=' ' )
            {
                zmienna.push_back(zawartosc.at(i));  // zmienna
            }
        }
//        if (zmienna == "ID_node")
//        {


//            ustawienia.ID_server = atoi(  wartosc.c_str());
//        }

        if (zmienna == s_mac)
        {    adresy_mac temp_a_mac;
            temp_a_mac.name_MAC=s_mac;
            temp_a_mac.MAC=wartosc;

            for (unsigned int i=0 ; i< zawartosc.size();++i)
            {
                if (zawartosc.at(i)==0x9 && zawartosc.at(++i)==0x9)
                {
                    temp_a_mac.opcja1= zawartosc.at(i+1)-((int)'0');
                    temp_a_mac.opcja2= zawartosc.at(i+3)-((int)'0');
                    temp_a_mac.opcja3= zawartosc.at(i+5)-((int)'0');
                    temp_a_mac.opcja4= zawartosc.at(i+7)-((int)'0');
                    temp_a_mac.opcja5= zawartosc.at(i+9)-((int)'0');
                    temp_a_mac.opcja6= zawartosc.at(i+11)-((int)'0');
                }
            }
            ustawienia.A_MAC.insert(ustawienia.A_MAC.begin() + licznik-1, temp_a_mac);
            s_mac="MAC";
            ++licznik;
            s_mac.push_back((char)(((int)'0')+licznik));
            //  std::cout << " zawartosc "<< zawartosc << std::endl;


        }

        zmienna = wartosc = "";

    }


    config.close();

    return t;
}
*/
