#if !defined(_PS_ALMA)
#define _PS_ALMA

struct UMAG
{
	int id;
	CString name;
	CString opis;
	//  CString kod;
	CString kod1, uzel1;
	CString kod2, uzel2;
	CString kod();
};


struct UMSRS
{
	int id;
	int id2;
	CString name;
	CString opis;
	//  CString kod;
	//  int re;
	int ue;
	int nach;
	int mag;
	CString kod1, uzel1;
	CString kod2, uzel2;
};

struct UE
{
	int id;
	int ist;
	int re;
	CString vid;
	CString nomer;
	int nach;
};


struct UMS
{
    int is_ms;
	int id;
    int id2;
	CString name;

	//  CString uch;

	CString opis;
	CString kod();
	//  int re;
	int ue;

	//  int nach;
	int mag;

	CString kod1, uzel1;
	CString kod2, uzel2;
};

struct URS
{
    int is_ms;
	int id;
    int id2;
	//  CString name;
	CString opis;
	CString kod();
	CString uzel;
	//  int re;
	int ue;
	//  int nach;
	CString kod1, uzel1;
	CString kod2, uzel2;
};

struct NACH
{
	int id;
	CString name;
	CString typ;
};

class PS_ALMA
{
public:
	set<CString> set_MAG;
	map<int, CString> map_RE;
	map<int, UMS> map_UMS;
	map<int, URS> map_URS;
	map<int, NACH> map_NACH;
	map<int, UE> map_UE;
	map<int, CString> map_Konstr;
	map<int, UMSRS> mapMSRS;
	//  map<CString, UMSRS> mapMSRS2;
	map<int, UMAG> mapMAG;

public:
	void init();


	UMS* getMS(const char* kod);
	URS* getRS(const char* kod);
//	UMSRS* getMSRS(const char* kod);
    int getMsRs2(int id, int ms_rs);

	const UE* getUE(int ue);

	UMS* getMs(int ms);
	URS* getRs(int rs);
	UMAG* getMag(int mag);
    UMSRS* getMsRs(int id);
};

template <class T> T* get_Vmap(map<int, T>& map_RE, int id)
{
	map<int, T>::iterator it = map_RE.find(id);
	if (it != map_RE.end()) return &it->second;
	return NULL;
}

CString get_V(map<int, CString>& map_RE, int id);


extern PS_ALMA ps_alma;

#endif
