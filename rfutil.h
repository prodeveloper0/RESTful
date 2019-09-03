#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>

bool inline timeover(unsigned long ts, unsigned long interval) {
  return (((unsigned long)(millis() - ts)) >= interval);
}

bool _strcmp(const char* s, const char* ss, const char eos) {
  while((*s != '\0' && *s != eos) && (*ss != '\0' && *ss != eos)) {
    if(*(s++) != *(ss++))
      return false;
  }
  return (*s == '\0' || *s == eos) && (*ss == '\0' || *ss == eos);
}

int _struntil(const char* s, const char eos) {
  const char *sbegin = s;
  
  while(*(s) != '\0' && *(s) != eos)
    ++s;
    
  return (s - sbegin);
}

bool _strcmp_P(const __FlashStringHelper* s, const __FlashStringHelper* ss, const char eos) {
  const char PROGMEM* ps = (const char PROGMEM*)s;
  const char PROGMEM* pss = (const char PROGMEM*)ss;
  int i = 0;
  int j = 0;
  char sc = pgm_read_byte(&(ps[i]));
  char ssc = pgm_read_byte(&(pss[j]));
  
  while((sc != '\0' && sc != eos) && (ssc != '\0' && ssc != eos)) {
    if(sc != ssc)
      return false;
    
    sc = pgm_read_byte(&(ps[++i]));
    ssc = pgm_read_byte(&(pss[++j]));
  }
  
  return (sc == '\0' || sc == eos) && (ssc == '\0' || ssc == eos);
}

bool _strcmp_P(const __FlashStringHelper* s, const char* ss, const char eos) {
  const char PROGMEM* ps = (const char PROGMEM*)s;
  int i = 0;
  int j = 0;
  char sc = pgm_read_byte(&(ps[i]));
  char ssc = ss[j];
  
  while((sc != '\0' && sc != eos) && (ssc != '\0' && ssc != eos)) {
    if(sc != ssc)
      return false;
    
    sc = pgm_read_byte(&(ps[++i]));
    ssc = ss[++j];
  }
  
  return (sc == '\0' || sc == eos) && (ssc == '\0' || ssc == eos);
}

bool _strcmp_P(const char* s, const __FlashStringHelper* ss, const char eos) {
  const char PROGMEM* pss = (const char PROGMEM*)ss;
  int i = 0;
  int j = 0;
  char sc = s[i];
  char ssc = pgm_read_byte(&(pss[j]));
  
  while((sc != '\0' && sc != eos) && (ssc != '\0' && ssc != eos)) {
    if(sc != ssc)
      return false;
    
    sc = s[++i];
    ssc = pgm_read_byte(&(pss[++j]));
  }
  
  return (sc == '\0' || sc == eos) && (ssc == '\0' || ssc == eos);
}

int _struntil_P(const __FlashStringHelper* s, const char eos) {
  int cnt = 0;
  const char PROGMEM* ps = (const char PROGMEM*)s;
  char c = pgm_read_byte(&(ps[cnt]));
  
  while(c != '\0' && c != eos)
    c = pgm_read_byte(&(ps[++cnt]));
    
  return cnt;
}
