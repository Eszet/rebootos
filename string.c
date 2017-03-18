// $Id: string.c 49 2006-09-20 22:03:44Z jlieder $

#include <stdarg.h>
#include <string.h>
#include <runtime.h>

bool _isws(char c)
{
  return(
    (c == ' ') || (c == '\t') || (c == '\n')
  );
}

bool _isnum(char c)
{
  return(
    (WITHIN(c, '0', '9') || (c == '-'))
  );
}

int atoi(strz s)
{
  int res = 0;
  int cmp = 0;
  char c;
  
  bool sign = (*s == '-');
  if(sign) s++;
  
  while(c = *s++)
  {
    if(WITHIN(c, '0', '9'))
    {
      res *= 10;
      res += (char) (c - '0');
      
      // Wrap around? => Overflow

      if(ABS(res - cmp) > 10 * 10)
        return(0);
      
      cmp = res;
    }
    else
      return(0); // Invalid char
  }
  
  if(sign) res *= -1;
  
  return(res);
}

strz itoa(int32 val, uint base)
{
  static char buf[MAXLEN] = { 0 };
  int i = MAXLEN - 2;		// -2: index 0..64 & trailing zero
  
  bool neg = (val < 0);
  
  if(neg) val = -val;
  
  if(val == 0)			// handle the exception with 0
    return("0");
  
  if((base < 2) || (base > 16))	// disallow illegal base values
    return(buf);
    
  for(; val && i; --i, val /= base)
    buf[i] = "0123456789abcdef"[val % base];
    
  if(neg) buf[i--] = '-';	// negative value, add sign prefix
    
  return(&buf[i+1]);
}

char *itopa(uint32 val, uint base, int pad)
{
  bool zeros = (pad > 0);
  int len = pad > 0 ? pad : -pad;
  
  static char buf[MAXLEN] = { 0 };
  int i = MAXLEN - 1; // - 1 OK
  
  if(!WITHIN(base, 2, 16))	// disallow illegal base values
    return(buf);
    
  if(len-1 > MAXLEN)		// requested padding within range?
    return(NULL /*buf*/);
    
  memset(buf, zeros ? '0' : ' ', MAXLEN);
  
  for(; val && i; --i, val /= base)
    buf[i] = "0123456789ABCDEF"[val % base];

  // buf[len+1] = 0; // + 1 OK <= why is that?
  
  return(&buf[MAXLEN - len]); // - len OK
}

int snprintf(strz buf, int len, strz format, ...)
{
  strz ptr = buf;
  char **arg = (char **) &format;
  arg++;
  
  int maxlen = len - 1;
  char c = 0;			// current character in format string
  int pad_behavior = 0;		// padding behavior (whitespace or zero?)
  
  if(buf == NULL)		// check for valid return buffer
    return(-1);

  while((c = *format++) && (buf - ptr < maxlen))
  {
    if (c != '%')
      *buf++ = c;
    else
    {
      char *p;
      c = *format++;

      switch(c)
      {
      /*
      case '0':
        pad_behavior = 1;
        break;
      
      case ' ':
        pad_behavior = 2;
        break;
      */
      
      case 'x': // hex
        *buf++ = '0';
        *buf++ = 'x';
        
        p = itopa(
          *((iptr *) arg++),
          16,
          sizeof(word) * 2
        );
        
        goto string;  
      
      case 'd': // decimal
      case 'u': // unsinged
        if(pad_behavior == 0)        
          p = itoa(
            *((iptr *) arg++), 
            10
          );
        else
          p = itopa(
            *((iptr *) arg++),
            10,
            pad_behavior == 1 ? 8 : -8
          );          
       
        goto string;
        
      case 'c': // char
        *buf++ = *((char *) arg++);
        
        break;

      case 'B': // bool
        p = *((bool *) arg++) ? 
            "true" : 
            "false";
        
        goto string;
        
      case 'b': // binary
        p = itopa(
          *((iptr *) arg++), 
          2,
          32
        );
       
        goto string;

      case 's': // string
        p = *arg++;

        if(!p)
          p = "(null)";

      string:
        while(*p && (buf - ptr < maxlen))
          *buf++ = *p++;

        break;

      default:
        // *buf++ = *((iptr *) arg++);
        
        break;
      }
    }
  }
  
  *buf = 0;
  
  return(buf - ptr);
}

inline strz strcpy(strz dest, strz src)
{
  strz tmp = dest;
  while((*dest++ = *src++) != 0);
  return(tmp);
}

inline strz strcat(strz dest, strz src)
{
  strz tmp = dest;
  while(*dest) dest++;
  while((*dest++ = *src++) != 0);
  return(tmp);
}

inline uint strlen(const strz s)
{
  unsigned i = 0;
  while(s[i]) i++;
  return(i);
}

inline int strcmp(const strz sa, const strz sb)
{
  signed char res;
  strz cs = sa, ct = sb;

  while(1)
    if((res = *cs - *ct++) != 0 || !*cs++)
      break;
      
  return(res);
}

inline strz strcut(strz dest, const strz src, int sta, int end)
{
  strz *begin = src + sta;
  int index = 0, len = end - sta + 1;
  
  while(index < len)
    dest[index] = *(begin + index++);
    
  dest[index] = 0;
  return(dest);
}

int _strtok(strz str, char token, char **arr)
{
  strz tmp = str;
  
  for(; *tmp; tmp++)
    if(*tmp == token)
      *tmp = 0;
      
  *(++tmp) = 0;

  int i = 0, sc = 0;
  strz start = str;

  while(sc < 2)
  {
    if(*str == 0)
    {
      sc++;
      
      arr[i] = start;
      if(str > start) i++;
      start = str + 1;
    }
    else
      sc = 0;
      
    str++;
  }
  
  return(i);
}
