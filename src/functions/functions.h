#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <stdlib.h>
#include "../wiadomosc/wiadomosc.h"
std::string conv_dns (std::string temp);

void ChangeEndianness(int32_t * value);
void binary(int val);

#endif // FUNCTIONS_H
