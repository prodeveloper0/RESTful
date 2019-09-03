#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "rfutil.h"

/*
 * RESTful Framework for Arduino
 * 
 * Written in 2017 by pr0ximo.
 * Version 0.4.1
 * 
 * Changelog
 * Version 0.1.0: First version
 * Version 0.1.1: Add request line, parameter processor
 * Version 0.1.2: Optimize memory consumption
 * Version 0.2.0: Change all things!! more efficient and simple!
 * Version 0.2.1: Add HTTP response codes
 * Version 0.3.0: You can add handler like nodejs express!
 * Version 0.3.1: Add __FlashStringHelper* body for reducing memory consumption
 * Version 0.3.2: Add HTTP header response
 * Version 0.3.3: Add HTTP header request handler
 * Version 0.3.4: String processor security patch and URL format mismatching bug fix
 * Version 0.3.5: Request handler can receive HTTP body from client
 * Version 0.3.6: Add function to reduce SRAM consumption
 * Version 0.4.0: Change buffer policy to reduce memory fragmentation
 * Version 0.4.1: Self code static analysis and security bug fix
 * 
 */

#define HTTP_100_CONTINUE                         F("HTTP/1.1 100 Continue\r\n")
#define HTTP_101_SWITCHING_PROTOCOLS              F("HTTP/1.1 101 Switching Protocols\r\n")
#define HTTP_200_OK                               F("HTTP/1.1 200 OK\r\n")
#define HTTP_201_CREATED                          F("HTTP/1.1 201 Created\r\n")
#define HTTP_202_ACCEPTED                         F("HTTP/1.1 202 Accepted\r\n")
#define HTTP_203_NON_AUTHORITATIVE_INFORMATION    F("HTTP/1.1 203 Non-Authoritative Information\r\n")
#define HTTP_204_NO_CONTENT                       F("HTTP/1.1 204 No Content\r\n")
#define HTTP_205_RESET_CONTENT                    F("HTTP/1.1 205 Reset Content\r\n")
#define HTTP_206_PARTIAL_CONTENT                  F("HTTP/1.1 206 Partial Content\r\n")
#define HTTP_300_MULTIPLE_CHOICES                 F("HTTP/1.1 300 Multiple Choices\r\n")
#define HTTP_301_MOVED_PERMANENTLY                F("HTTP/1.1 301 Moved Permanently\r\n")
#define HTTP_302_FOUND                            F("HTTP/1.1 302 Found\r\n")
#define HTTP_303_SEE_OTHER                        F("HTTP/1.1 303 See Other\r\n")
#define HTTP_304_NOT_MODIFIED                     F("HTTP/1.1 304 Not Modified\r\n")
#define HTTP_305_USE_PROXY                        F("HTTP/1.1 305 Use Proxy\r\n")
#define HTTP_307_TEMPORARY_REDIRECT               F("HTTP/1.1 307 Temporary Redirect\r\n")
#define HTTP_400_BAD_REQUEST                      F("HTTP/1.1 400 Bad Request\r\n")
#define HTTP_401_UNAUTHORIZED                     F("HTTP/1.1 401 Unauthorized\r\n")
#define HTTP_402_PAYMENT_REQUIRED                 F("HTTP/1.1 402 Payment Required\r\n")
#define HTTP_403_FORBIDDEN                        F("HTTP/1.1 403 Forbidden\r\n")
#define HTTP_404_NOT_FOUND                        F("HTTP/1.1 404 Not Found\r\n")
#define HTTP_405_METHOD_NOT_ALLOWED               F("HTTP/1.1 405 Method Not Allowed\r\n")
#define HTTP_406_NOT_ACCEPTABLE                   F("HTTP/1.1 406 Not Acceptable\r\n")
#define HTTP_407_PROXY_AUTHENTICATION_REQUIRED    F("HTTP/1.1 407 Proxy Authentication Required\r\n")
#define HTTP_408_REQUEST_TIME_OUT                 F("HTTP/1.1 408 Request Time-out\r\n")
#define HTTP_409_CONFLICT                         F("HTTP/1.1 409 Conflict\r\n")
#define HTTP_410_GONE                             F("HTTP/1.1 410 Gone\r\n")
#define HTTP_411_LENGTH_REQUIRED                  F("HTTP/1.1 411 Length Required\r\n")
#define HTTP_412_PRECONDITION_FAILED              F("HTTP/1.1 412 Precondition Failed\r\n")
#define HTTP_413_REQUEST_ENTITY_TOO_LARGE         F("HTTP/1.1 413 Request Entity Too Large\r\n")
#define HTTP_414_REQUEST_URI_TOO_LARGE            F("HTTP/1.1 414 Request-URI Too Large\r\n")
#define HTTP_415_UNSUPPORTED_MEDIA_TYPE           F("HTTP/1.1 415 Unsupported Media Type\r\n")
#define HTTP_416_REQUESTED_RANGE_NOT_SATISFIABLE  F("HTTP/1.1 416 Requested range not satisfiable\r\n")
#define HTTP_417_EXPECTATION_FAILED               F("HTTP/1.1 417 Expectation Failed\r\n")
#define HTTP_500_INTERNAL_SERVER_ERROR            F("HTTP/1.1 500 Internal Server Error\r\n")
#define HTTP_501_NOT_IMPLEMENTED                  F("HTTP/1.1 501 Not Implemented\r\n")
#define HTTP_502_BAD_GATEWAY                      F("HTTP/1.1 502 Bad Gateway\r\n")
#define HTTP_503_SERVICE_UNAVAILABLE              F("HTTP/1.1 503 Service Unavailable\r\n")
#define HTTP_504_GATEWAY_TIME_OUT                 F("HTTP/1.1 504 Gateway Time-out\r\n")
#define HTTP_505_HTTP_VERSION_NOT_SUPPORTED       F("HTTP/1.1 505 HTTP Version not supported\r\n")
#define HTTP_END_OF_REQUEST                       F("\r\n")


/*
 * Header
 * 
 * 
 */
class Header {
friend class RESTful;
private:
  char* _buf;
  int _bufsz;
  int _pos;
  
private:
  void setbuf(char* buf, int bufsz) {
    this->_buf = buf;
    this->_bufsz = bufsz;
    this->_pos = strlen(buf);
  }
  
private:
  Header() {
    this->_buf = NULL;
    this->_bufsz = 0;
    this->_pos = 0;
  }
  
private:
  char* str() const {
    return this->_buf;
  }
  
public:
  bool transmissible() const {
    return (this->_buf != NULL && this->_bufsz != 0 && this->_pos != 0);
  }
  
  bool available() const {
    return (this->_buf != NULL && this->_bufsz != 0);
  }
  
public:
  void set(const String& key, const String& value) {
    int keysz = key.length();
    int valuesz = value.length();
    
    if ((keysz + valuesz + 5) > (_bufsz - _pos))
      return;
    
    strcat(this->_buf, key.c_str());
    strcat_P(this->_buf, (char*)(F(": ")));
    strcat(this->_buf, value.c_str());
    strcat_P(this->_buf, (char*)(F("\r\n")));
    
    this->_pos += (keysz + valuesz + 4);
  }
  
  void set(const __FlashStringHelper* key, const String& value) {
    int keysz = strlen_P((char*)key);
    int valuesz = value.length();
    
    if ((keysz + valuesz + 5) > (_bufsz - _pos))
      return;
    
    strcat_P(this->_buf, (char*)key);
    strcat_P(this->_buf, (char*)(F(": ")));
    strcat(this->_buf, value.c_str());
    strcat_P(this->_buf, (char*)(F("\r\n")));
    
    this->_pos += (keysz + valuesz + 4);
  }
  
  void set(const String& key, const __FlashStringHelper* value) {
    int keysz = key.length();
    int valuesz = strlen_P((char*)value);
    
    if ((keysz + valuesz + 5) > (_bufsz - _pos))
      return;
    
    strcat(this->_buf, key.c_str());
    strcat_P(this->_buf, (char*)(F(": ")));
    strcat_P(this->_buf, (char*)value);
    strcat_P(this->_buf, (char*)(F("\r\n")));
    
    this->_pos += (keysz + valuesz + 4);
  }
  
  void set(const __FlashStringHelper* key, const __FlashStringHelper* value) {
    int keysz = strlen_P((char*)key);
    int valuesz = strlen_P((char*)value);
    
    if ((keysz + valuesz + 5) > (_bufsz - _pos))
      return;
    
    strcat_P(this->_buf, (char*)key);
    strcat_P(this->_buf, (char*)(F(": ")));
    strcat_P(this->_buf, (char*)value);
    strcat_P(this->_buf, (char*)(F("\r\n")));
    
    this->_pos += (keysz + valuesz + 4);
  }
  
  // Getting specific header field value
  String get(const String& key) {
    int i = 0;
    String value;
    
    do {
      if (!this->transmissible())
        break;
      
      int kcnt = _struntil(&this->_buf[i], ':');
      int vcnt = _struntil(&this->_buf[2 + i + kcnt], '\r');
      
      if(_strcmp(&this->_buf[i], key.c_str(), ':')) {
        value = String();
        value.reserve(vcnt);
        
        for(int j = 0;j < vcnt;++j)
          value += this->_buf[2 + i + kcnt + j];
        break;
      }
      
      i += 3 + kcnt + vcnt;
    } while(this->_buf[i++] != '\0');
    
    return value;
  }
  
  String get(const __FlashStringHelper* key) {
    int i = 0;
    String value;
    
    do {
      if (!this->transmissible())
        break;
      
      int kcnt = _struntil(&this->_buf[i], ':');
      int vcnt = _struntil(&this->_buf[2 + i + kcnt], '\r');
      
      if(_strcmp_P(&this->_buf[i], key, ':')) {
        value = String();
        value.reserve(vcnt);
        
        for(int j = 0;j < vcnt;++j)
          value += this->_buf[2 + i + kcnt + j];
        break;
      }
      
      i += 3 + kcnt + vcnt;
    } while(this->_buf[i++] != '\0');
    
    return value;
  }
};


/*
 * Request
 * 
 * 
 */
class Request {
friend class RESTful;
private:
  char* _method;
  char* _url;
  char* _query;
  char* _url_format;
  char* _protocol_version;
  bool _failed;
  Header* _hdr;
  
private:
  void setbuf(char* str) {
    this->_failed = true;
    char* line = str;
    
    // Method parsing
    this->_method = strtok(line, " ");
    if (this->_method == NULL)
      return;
    
    // URL parsing
    this->_url = strtok(NULL, " ");
    if(this->_url == NULL)
      return;
    
    // HTTP version parsing
    this->_protocol_version = strtok(NULL, " ");
    if(this->_protocol_version == NULL)
      return;
    
    // URL query parsing
    strtok(this->_url, "?");
    this->_query = strtok(NULL, "?");
    
    // HTTP version format vailidating
    if(strstr(this->_protocol_version, "HTTP/") == NULL)
      return;
    
    this->_failed = false;
  }
  
private:
  Request(const Header* ihdr) {
    this->_method = NULL;
    this->_url = NULL;
    this->_query = NULL;
    this->_url_format = NULL;
    this->_protocol_version = NULL;
    this->_failed = true;
    this->_hdr = ihdr;
  }
  
public:
  bool failed() const {
    return this->_failed;
  }
  
  char* method() const {
    return this->_method;
  }
  
  char* url() const {
    return this->_url;
  }
  
  char* query() const {
    return this->_query;
  }
  
  char* protocol_version() const {
    return this->_protocol_version;
  }
  
  Header* header() {
    return this->_hdr;
  }
  
private:
  char* url_format() const {
    return this->_url_format;
  }
  
  void url_format(char* url_format) {
    this->_url_format = url_format;
  }
  
public:
  // Getting specific URL query value
  String query(const String& key) {
    int i = 0;
    String value;
    
    do {
      if (this->_failed)
        break;
      
      int kcnt = _struntil(&this->_query[i], '=');
      int vcnt = _struntil(&this->_query[1 + i + kcnt], '&');
      
      if(_strcmp(&this->_query[i], key.c_str(), '=')) {
        value = String();
        value.reserve(vcnt);
        
        for(int j = 0;j < vcnt;++j)
          value += this->_query[i + kcnt + 1 + j];
        break;
      }
      
      i += kcnt + 1 + vcnt;
    } while(this->_query[i++] != '\0');
    
    return value;
  }
  
  String query(const __FlashStringHelper* key) {
    int i = 0;
    String value;
    
    do {
      if (this->_failed)
        break;
      
      int kcnt = _struntil(&this->_query[i], '=');
      int vcnt = _struntil(&this->_query[1 + i + kcnt], '&');
      
      if(_strcmp_P(&this->_query[i], key, '=')) {
        value = String();
        value.reserve(vcnt);
        
        for(int j = 0;j < vcnt;++j)
          value += this->_query[i + kcnt + 1 + j];
        break;
      }
      
      i += kcnt + 1 + vcnt;
    } while(this->_query[i++] != '\0');
    
    return value;
  }
  
  // Getting specific URL parameter value
  String parameter(const String& key) {
    int i = 0;
    int j = 0;
    String value;
    
    do {
      if (this->_failed)
        break;
      
      int fcnt = _struntil(&this->_url_format[i], '/');
      int ucnt = _struntil(&this->_url[j], '/');
      
      if(this->_url_format[i] == ':') {
        if(_strcmp(&this->_url_format[i + 1], key.c_str(), '/')) {
          value = String();
          value.reserve(ucnt);
          
          for(int k = j;k < j + ucnt;++k)
            value += this->_url[k];
          break;
        }
      }
      
      i += fcnt;
      j += ucnt;
    } while(this->_url_format[i++] != '\0' && this->_url[j++] != '\0');
    
    return value;
  }
  
  String parameter(const __FlashStringHelper* key) {
    int i = 0;
    int j = 0;
    String value;
    
    do {
      if (this->_failed)
        break;
      
      int fcnt = _struntil(&this->_url_format[i], '/');
      int ucnt = _struntil(&this->_url[j], '/');
      
      if(this->_url_format[i] == ':') {
        if(_strcmp_P(&this->_url_format[i + 1], key, '/')) {
          value = String();
          value.reserve(ucnt);
          
          for(int k = j;k < j + ucnt;++k)
            value += this->_url[k];
          break;
        }
      }
      
      i += fcnt;
      j += ucnt;
    } while(this->_url_format[i++] != '\0' && this->_url[j++] != '\0');
    
    return value;
  }
};


/*
 * Response
 * 
 * 
 */
class Response {
friend class RESTful;
private:
  const __FlashStringHelper* _status;
  const __FlashStringHelper* _constbody;
  String _body;
  bool _use_constbody;
  Header* _hdr;
  
private:
  Response(const Header* ohdr) {
    this->_status = NULL;
    this->_constbody = NULL;
    this->_use_constbody = false;
    this->_hdr = ohdr;
  }
  
public:
  __FlashStringHelper* status() const {
    return this->_status;
  }
  
  void status(const __FlashStringHelper* status) {
    if (status != NULL)
      this->_status = status;
  }
  
  __FlashStringHelper* constbody() const {
    return this->_constbody;
  }
  
  void constbody(const __FlashStringHelper* constbody) {
    if (constbody != NULL)
      this->_constbody = constbody;
  }
  
  String& body() {
    return this->_body;
  }
  
  void body(const String& body) {
    this->_body = body;
  }
  
  bool use_constbody() const {
    return this->_use_constbody;
  }
  
  void use_constbody(bool use_constbody) {
    this->_use_constbody = use_constbody;
  }
  
  Header* header() {
    return this->_hdr;
  }
};


typedef void (*RESTCALLBACK)(Request*, Response*, EthernetClient*);
typedef struct _RESTHANDLER_ {
  const char* method;
  const char* url;
  RESTCALLBACK request_callback;
} RESTHANDLER;


/*
 * RESTful Framework for Arduino
 * 
 * 
 */
class RESTful {
private:
  char* _buf;
  char* _rbuf;
  RESTHANDLER* _hdlr;
  int _bufsz;
  int _rbufsz;
  int _hdlrsz;
  
private:
  int _recvtimeout;
  
private:
  static void buildreq(char* buf, int bufsz, int rbufsz, Request* req, Response* res) {
    int eor = _struntil(buf, '\r');
    char* hdrstr = buf + eor + 2;
    
    buf[eor] = '\0';
    res->status(HTTP_404_NOT_FOUND);
    req->setbuf(buf);
    req->header()->setbuf(hdrstr, bufsz - (eor + 2));
    
    if (res->header() && rbufsz) {
      buf[bufsz] = '\0';
      res->header()->setbuf(buf + bufsz, rbufsz);
    }
  }
  
  static bool recvall(EthernetClient* client, unsigned long interval, char* buf, int bufsz) {
    unsigned long ts = millis();
    int recvsz = 0;
    bool isblank = true;
    
    while ((recvsz < bufsz - 1) && !timeover(ts, interval) && client->connected()) {
      if (!client->available())
        continue;
      
      char c = client->read();
      buf[recvsz++] = c;
      
      if ((c == '\n') && isblank) {
        buf[recvsz] = '\0';
        return true;
      }
      
      isblank = ((c == '\n') ? (true) : ((c == '\r') ? isblank : false));
      ts = millis();
    }
    
    return false;
  }
  
  static bool urlmatch(const char* format, const char* url) {
    int i = 0;
    int j = 0;
    
    while(true) {
      int fcnt = _struntil(&format[i], '/');
      int ucnt = _struntil(&url[j], '/');
      
      if(!_strcmp(&format[i], &url[j], '/')) {
        if(format[i] != ':' || ucnt <= 0)
          return false;
      }
      
      i += (fcnt);
      j += (ucnt);
      
      if(format[i] == '\0' || url[j] == '\0')
        break;
      
      ++i; ++j;
    }
    
    return !(strlen(&format[i]) + strlen(&url[j]));
  }
  
  static RESTHANDLER* findhdlr(RESTHANDLER* handler, int len, Request* req, Response* res) {
    for (int i = 0;i < len;++i) {
      RESTHANDLER* h = &(handler[i]);
      
      if ((!strcmp(h->method, req->method())) && urlmatch(h->url, req->url())) {
        res->status(HTTP_200_OK);
        req->url_format(h->url);
        return h;
      }
    }
    
    return NULL;
  }
  
public:
  int buffer_size() const {
    return this->_bufsz;
  }
  
  int reserved_buffer_size() const {
    return this->_rbufsz;
  }
  
  int timeout() const {
    return this->_recvtimeout;
  }
  
  void timeout(int timeout) {
    this->_recvtimeout = timeout;
  }

public:
  RESTful(char* buf, int bufsz, int rbufsz, RESTHANDLER* handler, int hdlrsz) {
    this->_rbufsz = rbufsz;
    this->_bufsz = bufsz - rbufsz;
    this->_rbuf = buf + this->_bufsz;
    this->_buf = buf;
    this->_hdlr = handler;
    this->_hdlrsz = hdlrsz;
    this->_recvtimeout = 7000;
  }
  
public:
  void loop(EthernetClient& client) {
    // Receive request
    memset(this->_buf, 0x00, this->_bufsz + this->_rbufsz);
    if (recvall(&client, this->_recvtimeout, this->_buf, this->_bufsz)) {
        Header ihdr;
        Header ohdr;
        Request req(&ihdr);
        Response res(&ohdr);
        
        // Build request and response object
        buildreq(this->_buf, this->_bufsz, this->_rbufsz, &req, &res);
        
        // Check request is valid
        if (!req.failed()) {
          // Search request handler
          RESTHANDLER* hdlr = findhdlr(this->_hdlr, this->_hdlrsz, &req, &res);
          
          // Process request
          if (hdlr != NULL)
            hdlr->request_callback(&req, &res, &client);
          
          // Send response and header fields
          client.print(res.status());
          if (ohdr.transmissible())
            client.print(ohdr.str());
          client.print(HTTP_END_OF_REQUEST);
          
          // Send response body
          if (res.use_constbody())
            client.print(res.constbody());
          else
            client.print(res.body());
          return;
        }
    }
    client.print(HTTP_400_BAD_REQUEST);
    client.print(HTTP_END_OF_REQUEST);
  }
};

