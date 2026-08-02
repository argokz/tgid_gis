#if !defined(__httpparser_h)    
#define __httpparser_h

class CHttpParser
{
public:

  CHttpParser(const char *request);
  string getV(const char *key);

  string url;
  string v;
  map<string, string> map_v;

};

#endif                          
