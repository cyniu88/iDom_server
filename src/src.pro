TEMPLATE = app
CONFIG += console
CONFIG -= qt
LIBS += -pthread

SOURCES +=  iDom_server.cpp parser/parser.cpp serialib/serialib.cpp \
    c_connection/c_connection.cpp \
    functions/functions.cpp

HEADERS += parser/parser.hpp serialib/serialib.h wiadomosc/wiadomosc.h \
   c_connection/c_connection.h \
    functions/functions.h

