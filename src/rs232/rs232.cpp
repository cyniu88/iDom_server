/***************************************************************************** 
 *                                                                           *
 *  File  : rs232.cpp                                                        *
 *  Author: Grzegrz Pietkiewicz                                              *
 *  Date  : 05.11.98                                                         *
 *                                                                           *
 *  Description: Biblioteka obs³ugi portu szeregowego pod linux'em           *
 *               Zdefiniowano podstawoe procedury do obs³ugi portu           *
 *               szeregowego.                                                *
 *               Patrz: man termios, man fcntl, man unistd,                  *
 *                      Linux Programers Guide                               *
 *                                                                           *
 *****************************************************************************/

#include <strings.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>



// ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd

#define ISetON(x) tds.c_iflag |= x;          // Ustawienie input modes flag
#define ISetOFF(x) tds.c_iflag &= ~(x);      // Zgaszenie input modes flag
#define OSetON(x)  tds.c_oflag |= x;         // Ustawienie output modes flag
#define OSetOFF(x) tds.c_oflag &= ~(x);      // Zgaszenie Output modes flag
#define OSet(field,mask)  tds.c_oflag = (tds.c_oflag & ~(field)) | mask;
#define CSetON(x)  tds.c_cflag |= x;         // Ustawienie control modes flag
#define CSetOFF(x) tds.c_cflag &= ~(x);      // Zgaszenie control modes flag
#define LSetON(x)  tds.c_lflag |= x;         // Ustawienie local modes flag
#define LSetOFF(x) tds.c_lflag &= ~(x);      // Zgaszenie local modes flag

using namespace std;

int SetCom(int fd){
  termios tds;
  if (tcgetattr(fd,&tds)) {  perror("B³±d ustawienia:"); return(-1); };
  if (cfsetispeed(&tds,B38400)){ perror("B³±d ustawienia:"); return(-1); };
  if (cfsetospeed(&tds,B38400)){ perror("B³±d ustawienia:"); return(-1); };
  // Ustawienia dla wejscia
  ISetOFF(INPCK);  // kontrola parzystosci
  ISetOFF(ICRNL);  // NL->CR
  ISetOFF(ISTRIP); // strip 8 bit
  ISetON(IXON);    // kontrola XON/XOFF dla wejscia
  ISetON(IXOFF);   // kontrola XON/XOFF dla wyjscia
  ISetON(IXANY);
  // Ustawienia dla wyjscia
  OSet(NLDLY,NL1); 
  OSet(CRDLY,CR0);
  OSet(FFDLY,FF0);
  OSetOFF(OPOST); // # enable implementation-defined output processing
  OSetOFF(ONLCR); // # map NL to CR-NL on output
  OSetOFF(OCRNL); // map CR to NL on output
  OSetOFF(ONOCR); // don't output CR at column 0
  OSetOFF(ONLRET);// don't output CR
  OSetOFF(OFILL); // 
  // Flagi kontroli
  CSetOFF(CSTOPB); // Two stop bits
  CSetON(CREAD);   // Enable Receiver
  CSetOFF(PARENB); // Parity generation on out / checking at in
  CSetOFF(PARODD); // Odd parity
  CSetOFF(CRTSCTS);// Flow Control
  CSetOFF(HUPCL);  // lower  modem control lines after last process closes 
                   // the device (hang up)
  CSetON(CLOCAL);   // ignore modem control lines - now not ignored
  tds.c_cflag = (tds.c_cflag & ~CSIZE)  | CS8;
  // Local modes
  LSetOFF(ICANON);  // canonic mode
  LSetOFF(ECHO);    // echo
  LSetOFF(ECHOE);   // echo dla canonic mode z dzialajacym erase, itp..
  LSetOFF(ECHOK);   // jw. tylko kill character
  LSetOFF(ECHOCTL); // generuje sygnaly sterujace
  LSetOFF(ECHOKE);
  LSetOFF(FLUSHO);  // flush output
  LSetON(NOFLSH);   // disable flushing
  LSetOFF(IEXTEN); // disenable implementation-defined input processing
  // ************************
  if (tcsetattr(fd,TCSANOW,&tds)<0) { return(-1); }
  else return(1);
}

// ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd





#define BUFFER_LEN 256
#define READ_ERROR -1
#define TIME_OUT_ERROR -2
#define ECHO_ERROR -3

/* Potrzebne s± funkcje do :
	- otwarcia po³±czenia
	- zamkniêcia po³±czenia
	- ustalenia parametrów po³±czenia ( na sta³e )
	- zapisu ze sprawdzeniem czy by³o echo
	- odczytu
*/

// Sta³a definuje nazwy plikow portów
//const char *PORT[]={ "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3" };


// Otwarcie pliku COM , wymagany nr portu ( 1,2,3,4 ) , zwraca identyfikator
// pliku gdy wszystko by³o OK.
int OpenCom(const char* nr_portu) {
  int com;
  if ((com=open(nr_portu ,O_RDWR | O_NONBLOCK))<0) {
    perror("B³±d otwarcia portu ");
    return(-1);
  }
  return(com);
};

// Zamkniecie pliku portu. Patrz "man close"
int CloseCom(int com){
  return(close(com));
};

int SendWait(int com,const char *toSend,int timeOut=2)
{
  int wCode=0,sended=0,charToSend=strlen(toSend);
  if (toSend!=NULL) 
    {
      for ( time_t start_time=time(NULL) ; sended<charToSend;
	    start_time=time(NULL) )
	{ // Powtarzamy je¿eli by³o Œle
	  while ( (wCode=write(com,toSend+sended,1)) < 0 ) 
	    {
	      if (errno!=EAGAIN) // Powa¿ny b³±d, koniec
		{ 
		  perror("B³±d zapisu "); 
		  return(READ_ERROR); 
		};
	      if (timeOut>0) // Sprawdzamy czy nie by³o timeOut'a
		{ 
		  if(time(NULL)-start_time > timeOut) 
		    { 
		      cout << "B³±d zapisu : Time Out\n";
		      return(TIME_OUT_ERROR); 
		    };
		}
	    }
	  sended++;
	}
    } // Do if !=NULL
  return(sended);
}

// Wys³anie tekstu po portu COM. Brak oczekiwania na echo. Zwraca d³.
// wys³anego ³añcucha.
int Send(int com,const char *tosend){
  int sended; // pomocnicza. Ile by³o wys³ane
  if (tosend!=NULL){ 
    if ( (sended=write(com,tosend,strlen(tosend))) <0 ) {
      perror("B³±d zapisu do portu ");
      return(-1);
    }
    return(sended);
  }
  else return(0);
};

// Wyslanie tekstu i przejscie do nowej lini
int SendLn(int com,const char *tosend,const char *nline="\r\n"){
  int sended;                                // pomocnicza. Ile by³o wys³ane
  char sbuffor[strlen(tosend)+strlen(nline)];// buffor na tekst+znak konca lini
  strcpy(sbuffor,tosend);                    // kopiowanie lancucha
  strcat(sbuffor,nline);                     // i dodanie znaku konca lini  
  if (tosend!=NULL){        // zabezpieczenie przed wyslaniem pustego lancucha 
    if ( (sended=write(com,sbuffor,strlen(sbuffor)) ) <0 ) {
      perror("B³±d zapisu do portu ");
      return(-1);
    }
    return(sended);
  }
  else return(0);
};


int Receive(int com, char *RText, int LnSize,int TimeOut=0)
{
  char rbuffor[1];
  int rcode,readed=0;
  time_t start_time=time(NULL);
  for(;;)
  {
    if((rcode=read(com,rbuffor,1))<0)
    {
      if(errno!=EAGAIN) 
      { 
        perror("B³±d odczytu "); 
        return(READ_ERROR); 
      }
    }
    if(rcode==1)
    { 
      start_time=time(NULL);
      // przeczytano znak
      RText[readed]=*rbuffor;
      // Wyjscie po przekroczeniu dlugosci bufora
      if (++readed>=(LnSize-1))
      { 
        RText[readed]=char(0); 
        return(readed);
      }
    }
    if (TimeOut>0)
    { 
      if(time(NULL)-start_time > TimeOut)
      {
	RText[readed]=char(0);
        return(TIME_OUT_ERROR);
      }
    }
  }
}

// Pobranie ci±gu znaków o zadanej dl. Mo¿liwe ustawienie warto¶ci time out.
//int Receive(int com, char *RText, int LnSize,int TimeOut=0){
//  char rbuffor[1];
//  int rcode,readed=0;
//  time_t start_time=time(NULL);
//  for(;;){
//    if ((rcode=read(com,rbuffor,1))<0) {
//      if (errno!=EAGAIN) { perror("B³±d odczytu "); return(READ_ERROR); };
//    };
//    if (rcode==1){ // przeczytano znak
//      RText[readed]=*rbuffor;
//      // Wyjscie po przekroczeniu dlugosci bufora
//      if (++readed>=(LnSize-1)) { RText[readed]=char(0); return(readed); };
//    };
//    if (TimeOut>0){ 
//      if(time(NULL)-start_time > TimeOut) { return(TIME_OUT_ERROR); };
//    };
//  };
//};

int ReadCom(int com, char *text){
  int readed=0;
  if ((readed=read(com,text,sizeof(text) ))<0) {
      if (errno!=EAGAIN) { perror("B³±d odczytu "); return(READ_ERROR); }
      else return(0);
  };
  return(readed);
};


// Pobranie ci±gu znaków zakoñczonych przez 0x13. Funkcja zwraca dl.
// przeczytanej lini. Mo¿liwe ustawienie warto¶ci time out.
int ReceiveLn(int com, char *RText, int LnSize,int TimeOut=0){
  char rbuffor[1];
  int rcode,readed=0;
  time_t start_time=time(NULL);
  for(;;){
    if ((rcode=read(com,rbuffor,1))<0) {
      if (errno!=EAGAIN) { perror("B³±d odczytu:"); return(READ_ERROR); };
    };
    if (rcode==1){ // przeczytano znak
      if (*rbuffor!=char(13)){  // nie bylo entera
        RText[readed]=*rbuffor;
	// test przekroczenia bufora
	if (++readed>=(LnSize-1)) { RText[readed]=char(0); return(readed); };
      } else { RText[readed]=char(0); return(readed); }; // wyjscie po enterze
    };
    if (TimeOut>0){ 
      if(time(NULL)-start_time > TimeOut) { return(TIME_OUT_ERROR); };
    };
  };
};

// Wys³anie z oczekiwaniem echa
int SendWaitEcho(int com,const char *tosend,int WaitTime=2){
  int sended;                     // pomocnicza. Ile by³o wys³ane
  if (tosend!=NULL){ 
    if ( (sended=write(com,tosend,strlen(tosend))) <0 ) {
      perror("B³±d zapisu do portu ");
      return(-1);
    }
    int rcode;
    unsigned int readed=0;
    char rbuffor[1];
    time_t start_time=time(NULL);
    for(;;){
      if ((rcode=read(com,rbuffor,1))<0) {
	if (errno!=EAGAIN) { perror("B³±d odczytu "); return(READ_ERROR); };
      };
      if (rcode==1){  // przeczytano znak
	if (tosend[readed++]!=rbuffor[0]) { return(ECHO_ERROR); };
        //     ^ Niezgodno¶æ echa ^
	if (readed==strlen(tosend)) { return(readed); };
	//   ^ Wyjscie gdy ca³o¶æ zosta³a porównana ^
      };
      if (WaitTime>0){ // Sprawdzenie TimeOut
	if(time(NULL)-start_time > WaitTime) { return(TIME_OUT_ERROR); };
      };
    };
  }
  else return(0);
};

// Wys³anie z przej¶ciem do nowej lini i oczekiwaniem echa
int SendWaitEchoLn(int com,const char *tosend,int WaitTime=2,const char *nline="\r\n"){
  int i;                                     // pomocnicza. Ile by³o wys³ane
  char sbuffor[strlen(tosend)+strlen(nline)];// buffor na tekst+znak konca lini
  strcpy(sbuffor,tosend);                    // kopiowanie lancucha
  strcat(sbuffor,nline);                     // i dodanie znaku konca lini  
  if (tosend!=NULL){ 
    if ( (i=write(com,sbuffor,strlen(sbuffor))) <0 ) {
      perror("B³±d zapisu do portu ");
      return(-1);
    }
    int rcode;
    unsigned int readed=0;
    char rbuffor[1];
    time_t start_time=time(NULL);
    for(;;){
      if ((rcode=read(com,rbuffor,1))<0) {
	if (errno!=EAGAIN) { perror("B³±d odczytu "); return(READ_ERROR); };
      };
      if (rcode==1){   // przeczytano znak
	if (sbuffor[readed++]!=rbuffor[0]) { return(ECHO_ERROR); }; 
        // Niezgodno¶æ echa
	if (readed==strlen(sbuffor)) { return(readed); };
	// ^ Wyjscie gdy ca³o¶æ zosta³a porównana ^
      };
      if (WaitTime>0){ // Sprawdzenie TimeOut
	if(time(NULL)-start_time > WaitTime) { return(TIME_OUT_ERROR); };
      };
    };
  }
  else return(0);
};


int SetBaudRate(int com,int brate){
  termios tds;
  speed_t brate_c;
  switch(brate){
  case 0:
    brate_c=B0;
    break;
  case 50:
    brate_c=B50;
    break;
  case 75:
    brate_c=B75;
    break;
  case 110:
    brate_c=B110;
    break;
  case 134:
    brate_c=B134;
    break;
  case 150:
    brate_c=B150;
    break;
  case 200:
    brate_c=B200;
    break;
  case 300:
    brate_c=B300;
    break;
  case 600:
    brate_c=B600;
    break;
  case 1200:
    brate_c=B1200;
    break;
  case 1800:
    brate_c=B1800;
    break;
  case 2400:
    brate_c=B2400;
    break;
  case 4800:
    brate_c=B4800;
    break;
  case 9600:
    brate_c=B9600;
    break;
  case 19200:
    brate_c=B19200;
    break;
  case 38400:
    brate_c=B38400;
    break;
  case 57600:
    brate_c=B57600;
    break;
  case 115200:
    brate_c=B115200;
    break;
  case 230400:
    brate_c=B230400;
    break;
  default:
    cout << "B³±d ! Podano niepoprawn± warto¶æ prêdko¶ci \r\n";
    cout << "Ustawiono warto¶æ domy¶ln± 9600\r\n";
    brate_c=B9600;
  };
  if (tcgetattr(com,&tds)) {  perror("SetBaudRate "); return(-1); };
  if (cfsetispeed(&tds,brate_c)){ return(-1); };
  if (cfsetospeed(&tds,brate_c)){ return(-1); };
  if (tcsetattr(com,TCSANOW,&tds)){ perror("SetBaudRate"); return(-1); };
  return(1);
};

int SetStopBits(int com,int sbits){
  termios tds;
  if (tcgetattr(com,&tds)) {  perror("SetStopBits "); return(-1); };
  switch(sbits){
    case 1: 
      CSetOFF(CSTOPB); break; // Jeden bit stopu  
    case 2: 
      CSetON(CSTOPB); break; // Dwa bity stopu
  default:
    cout<<"B³±d ! Niepoprawna ilo¶æ bitów stopu\r\n";
    return(-1);
  };
  if (tcsetattr(com,TCSANOW,&tds)){ perror("SetStopBits"); return(-1); };
  return(1);
};
 
int SetDataBits(int com,int dbits){
  termios tds;
  if (tcgetattr(com,&tds)) {  perror("SetDataBits "); return(-1); };
  switch(dbits){
  case 5:  tds.c_cflag = (tds.c_cflag & ~CSIZE)  | CS5; break;
  case 6:  tds.c_cflag = (tds.c_cflag & ~CSIZE)  | CS6; break;
  case 7:  tds.c_cflag = (tds.c_cflag & ~CSIZE)  | CS7; break;
  case 8:  tds.c_cflag = (tds.c_cflag & ~CSIZE)  | CS8; break;
  default:
    cout<<"B³±d ! Niepoprawna ilo¶æ bitów danych\r\n";
    return(-1);
  };
  if (tcsetattr(com,TCSANOW,&tds)){ perror("SetDataBits"); return(-1); };
  return(1);
};

int SetParity(int com,const char *parity){
  termios tds;
  if (tcgetattr(com,&tds)) {  perror("SetParity "); return(-1); };
  if (strcmp(parity,"NONE")==0) { 
    CSetOFF(PARENB); 
    ISetOFF(INPCK);
  } // Disable parity
  else if (strcmp(parity,"EVEN")==0){
    CSetOFF(PARODD); // Even parity
    ISetON(INPCK);
    CSetON(PARENB);  // Enable parity
  } 
  else if (strcmp(parity,"ODD")==0){
    CSetON(PARODD);  // Odd parity
    ISetON(INPCK);
    CSetON(PARENB);  // Enable parity
  } 
  else {
    cout<<"B³±d ! Niepoprawna warto¶æ parzysto¶ci\r\n";
    return(-1);
  };
  if (tcsetattr(com,TCSANOW,&tds)){ perror("SetParity"); return(-1); };
  return(1);
};

int SetXONXOFF(int com,const char *state){
  termios tds;
  if (tcgetattr(com,&tds)) {  perror("SetXONXOFF "); return(-1); };
  if (strcmp(state,"ENABLE")==0) { 
    ISetON(IXON);    // kontrola XON/XOFF dla wejscia
    ISetON(IXOFF);   // kontrola XON/XOFF dla wyjscia
    ISetON(IXANY);
  }
  else if (strcmp(state,"DISABLE")==0){
    ISetOFF(IXON);    // kontrola XON/XOFF dla wejscia
    ISetOFF(IXOFF);   // kontrola XON/XOFF dla wyjscia
    ISetOFF(IXANY);
  } 
  else {
    cout<<"B³±d ! Niepoprawna warto¶æ kontroli przep³ywu XON/XOFF\r\n";
    return(-1);
  };
  if (tcsetattr(com,TCSANOW,&tds)) {  perror("SetXONXOFF "); return(-1); };
  return(1);
};


int FlushCom(int com){
  return(tcflush(com,TCIOFLUSH));
};







//SetCRTSCTS
//  CSetOFF(CRTSCTS);// Flow Control

/*
int SendWEcho(int com, char *ToSend, int WaitTime=2){
  if (Send(com,ToSend)<0) { return(-1); }; 
  char tbuffor[strlen(ToSend)+1];       
  if (Receive(com,tbuffor,sizeof(tbuffor),WaitTime)<0){ return(-1); };
  if (strcmp(ToSend,tbuffor)!=0) { return(-1);};
  return(1);
};

int SendLnWEcho(int com, char *tosend, int WaitTime=2, char *nline="\r\n"){
  char sbuffor[strlen(tosend)+strlen(nline)]; 
  for(int i=0;sbuffor[i]=tosend[i];i++);     
  strcat(sbuffor,nline);                      
  if (Send(com,sbuffor)<0) { return(-1); }
  char tbuffor[strlen(sbuffor)+1];
  if (Receive(com,tbuffor,sizeof(tbuffor),WaitTime)<0){ return(-1); };
  if (strcmp(sbuffor,tbuffor)!=0) { return(-1);};
  return(1);
};
*/
