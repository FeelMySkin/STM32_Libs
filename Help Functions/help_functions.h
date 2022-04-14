#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H

#include "defines.h"

bool CompareWildStrings(const char* wild, const char* string, char wild_card='*');
bool ComparePartStrings(const char* str1, const char* str2);
uint16_t GetLengthToSymbol(const char* str,char symbol);
uint16_t CopyString(char* str_to,const char* str_from,uint16_t length=65000);
uint16_t CountCRC16(uint16_t base,uint16_t next);

#endif
//
