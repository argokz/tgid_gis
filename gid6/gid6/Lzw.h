#ifndef __LZW_H__
#define __LZW_H__

union LS {
  unsigned long l;
  unsigned short s[2];
};

class LZW {
public:
  LZW(int size = 4096) {
    cepp = new unsigned short[size];
    cepk = new unsigned char[size];
  };
  ~LZW() {
    delete [] cepp;
    delete [] cepk;
  };

  void decod(unsigned char *p, unsigned char *u);

private:

  unsigned char Read_Cep_Wr(short code) {
  
    unsigned char st[256];
    unsigned char n = 0;

    if (code < cc2) {
      *uu++ = code;
    }
    else {
      do {
        st[n++] = cepk[code];
        code = cepp[code];
      } while (code >= cc2);
  
      *uu++ = code;
  
      while (n > 0) *uu++ = st[--n];
    }
    return code;
  };
  
  short get_code() {
    LS c1;
    unsigned char l;
  
    if (beg >= 16) {
      buf.s[0] = buf.s[1];
      buf.s[1] = *pp++;
      beg -= 16;
    }
  
    l = beg+N;
  
    if (l <= 16) {
      c1.s[0] = buf.s[0] >> beg;
    }
    else {
      c1.l = buf.l >> beg;
    }
    beg = l;
  
    return c1.s[0];
  };

  unsigned char *cepk;
  unsigned short *cepp;

  unsigned short cc2, N;

  unsigned short *pp;
  char *uu;

  LS buf;

  unsigned char beg;
};


#endif //__LZW_H__