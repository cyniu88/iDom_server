/***************************************************************************** 
 *                                                                           *
 *  File  : rs232.hh                                                         *
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

#define BUFFER_LEN 256
#define READ_ERROR -1
#define TIME_OUT_ERROR -2
#define ECHO_ERROR -3


// Sta³a definuje nazwy plikow portów
// const char *PORT[]={ "/dev/cua0", "/dev/cua1", "/dev/cua2", "/dev/cua3" };

// Otwarcie pliku COM , wymagany nr portu ( 1,2,3,4 ) , zwraca identyfikator 
// pliku gdy wszystko by³o OK.
extern int OpenCom(const char * nr_portu) ;

// Ustawienie domy¶lnych parametrów po³±czenia
extern int SetCom(int fd);

// Ustawienie prêdkosci transmisji. (  0,50,75,110,134,150,200,300,600,1200,
// 1800,2400,4800,9600,19200,38400,57600,115200,230400 )
extern int SetBaudRate(int com,int brate);

// Ustawienie bitów stopu (1,2)
extern int SetStopBits(int com,int sbits);

// Ustawienie bitów danych (5,6,7,8)
extern int SetDataBits(int com,int dbits);

// Ustawienie parzysto¶ci (NONE,EVEN,ODD);
extern int SetParity(int com,const char *parity);

// Ustawienie kontroli przep³ywu XON/XOFF (ENABLE,DISABLE)
extern int SetXONXOFF(int com,const char *state);

// Opró¿nienie buforów portu
extern int FlushCom(int com);

// Zamkniecie pliku portu. Patrz "man close"
extern int CloseCom(int com);

// Wys³anie tekstu po portu COM. Brak oczekiwania na echo. Zwraca d³. wys³anego
// ³añcucha.
extern int Send(int com,const char *tosend);

extern int SendWait(int com,const char *tosend, int TimeOut=2);

// Wyslanie tekstu i przejscie do nowej lini.
extern int SendLn(int com, char *tosend, char *nline="\n\r");

// Pobranie ci±gu znaków o zadanej d³ugo¶ci. Mo¿liwe ustawienie warto¶ci
// czasu oczekiwania
extern int Receive(int com, char *RText, int LnSize,int TimeOut=0);

// Pobranie ci±gu znaków zakoñczonych przez CR. Funkcja zwraca d³ugo¶æ
// przeczytanej lini. Mo¿liwe ustawienie warto¶ci czasu czekania.
extern int ReceiveLn(int com, char *RText, int LnSize,int TimeOut=0);

// Wys³anie tekstu i test czy echo zwróci³o to samo. B³±d po przekroczeniu
// czasu oczekiwania. Gdy echo by³o OK zwraca d³ugo¶æ wys³aneg ³añcucha
extern int SendWaitEcho(int com, char *tosend,int WaitTime=2);

extern int sendWait(int com,const char *toSend,int timeOut=2);

// Wys³anie tekstu z przej¶ciem do nowej lini i test czy echo zwróci³o to
// samo. B³±d po przekroczeniu czasu oczekiwania. Gdy echo by³o OK zwraca
// d³ugo¶æ wys³aneg ³añcucha
extern int SendWaitEchoLn(int com, char *tosend,int WaitTime=2, char *nline="\r\n");


