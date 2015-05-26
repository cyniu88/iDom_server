TEMPLATE = app
CONFIG += console
CONFIG -= qt
LIBS += -pthread

SOURCES +=  src/iDom_server.cpp src/parser/parser.cpp src/serialib/serialib.cpp \
    src/c_connection/c_connection.cpp \
    src/functions/functions.cpp

HEADERS += src/parser/parser.hpp src/serialib/serialib.h src/wiadomosc/wiadomosc.h \
   src/c_connection/c_connection.h \
    src/functions/functions.h

