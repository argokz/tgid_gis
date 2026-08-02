#include "stdafx.h"
//#include "gidr.h"

#include "http_parser.h"

#include "httpparser.h"


static int on_info(http_parser* p) 
{
  return 0;
}

static int on_url(http_parser *p, const char *at, size_t length) 
{
  CHttpParser *pd = (CHttpParser *) p->data;
  pd->url = string(at, length);

  struct http_parser_url u;
  http_parser_url_init(&u);
  http_parser_parse_url(at, length, 0, &u);


  return 0;
}

static int on_header_field(http_parser *p, const char *at, size_t length) 
{
  CHttpParser *pd = (CHttpParser *) p->data;
  pd->v = string(at, length);
  return 0;
}

static int on_header_value(http_parser *p, const char *at, size_t length) 
{
  CHttpParser *pd = (CHttpParser *) p->data;
  string v = string(at, length);
  pd->map_v[pd->v] = v;
  return 0;
}

static int on_status(http_parser *p, const char *at, size_t length) 
{
/*
  char *s = new char[length+1];
  strncpy(s, at, length);
  s[length] = 0;
  printf("on_status %s\n", s);
  delete [] s;
*/
  return 0;
}

static int on_body(http_parser *p, const char *at, size_t length) 
{
/*
  char *s = new char[length+1];
  strncpy(s, at, length);
  s[length] = 0;
  printf("on_body %s\n", s);
  delete [] s;
*/
  return 0;
}

CHttpParser::CHttpParser(const char *request)
{
  http_parser_settings settings;

  settings.on_message_begin = on_info;
  settings.on_headers_complete = on_info;
  settings.on_message_complete = on_info;
  settings.on_header_field = on_header_field;
  settings.on_header_value = on_header_value;
  settings.on_url = on_url;
  settings.on_status = on_status;
  settings.on_body = on_body;

  struct http_parser parser;
  http_parser_init(&parser, HTTP_REQUEST);

//  CHttpParser pd;
  parser.data = (char *) this;

  size_t parsed = http_parser_execute(&parser, &settings, request, strlen(request));
}



string CHttpParser::getV(const char *key)
{
  map<string, string>::const_iterator it = map_v.find(string(key));
  if (it != map_v.end()) {
    return it->second;
  }
  return string("");
}
