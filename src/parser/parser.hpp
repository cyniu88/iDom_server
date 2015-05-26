#ifndef PARSER_H_
#define PARSER_H_
#include <string>
#include <vector>

struct adresy_mac{
    std::string name_MAC;
    std::string MAC;
    bool obecny;
    int opcja1;
    int opcja2;
    int opcja3;
    int opcja4;
    int opcja5;
    int opcja6;
};

struct addres_another_servers {
    int id;
    std::string SERVER_IP;
};

struct konfiguracja{
    std::string portRS232;
    std::string BaudRate;
    std::string PORT;
    std::string SERWER_IP;
    int ID_server;
    std::vector <adresy_mac> A_MAC;
    int v_delay  ;
    std::vector <addres_another_servers> AAS;

};



konfiguracja parsowanie (const char* sciezka_pliku  );
#endif
