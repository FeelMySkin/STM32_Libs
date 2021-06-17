#include "help_functions.h"

bool CompareWildStrings(const char* wild, char* string, char wild_card)
{
	char *cp = NULL;//, *mp = NULL;
	const char *mp;

  while ((*string) && (*wild != wild_card)) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == wild_card) {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == wild_card) {
    wild++;
  }
  return !*wild;
}
//

bool ComparePartStrings(const char* str1, const char* str2)
{
	while(*str1 != 0 && *str2 != 0)
	{
		if(*str1 != *str2) return false;
		str2++;
		str1++;
	}
	if(*str1 == 0 && *str2 !=0) return false;
	return true;
}
//

uint16_t GetLengthToSymbol(const char* str,char symbol)
{
	uint8_t shift = 0;
	while(*str != symbol && *str !=0)
	{
		shift++;
		str++;
	}
	return shift;
}
//

uint16_t CopyString(char* str_to,const char* str_from,uint16_t length)
{
	for(int i = 0;i<length;++i)
	{
		str_to[i] = 0;
		if(str_from[i] == 0) return i;
		str_to[i] = str_from[i];
	}
	str_to[length] = 0;
	return length;
}
//

uint16_t CountCRC16(uint16_t base,uint16_t next)
{
	base^=next;
	for(int i = 0;i<8;++i) base=(base&0x0001)!=0 ? (base>>1)^0xA001 : base>>1;
	return base;
}
//
