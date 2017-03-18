// $Id: string.h 49 2006-09-20 22:03:44Z jlieder $

#ifndef __STRING_H__
#define __STRING_H__

#include <types.h>

#define MAXLEN 65

int atoi(strz s);
strz itoa(int32 val, uint base);
strz itopa(uint32 val, uint base, int pad);

int snprintf(strz buf, int len, strz format, ...);

inline strz strcpy(strz dest, strz src);
inline strz strcat(strz dest, strz src);
inline uint strlen(const strz s);
inline int strcmp(const strz sa, const strz sb);

inline strz strcut(strz dest, const strz src, int sta, int end);
int _strtok(strz str, char token, char **arr); // modifies str!
                          
#endif
