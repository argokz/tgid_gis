#include "stdafx.h"
#include "gid6.h"

#include "Win.h"

#include "InputDia.h"

#include "jsmn.h"

const char *keys[] = {
#include "keys.h"
};


CString parse_json_user(const char* pBuf)
{
    int l = strlen(pBuf);

    int numtok = 200;
    jsmntok_t* t = (jsmntok_t*)malloc(numtok * sizeof(jsmntok_t));

    jsmn_parser p;
    jsmn_init(&p);
    int r = jsmn_parse(&p, pBuf, l, t, numtok);

    CString s2 = "";

    CString out = "";

    for (int i = 0; i < r; i++) {
        if (t[i].type == JSMN_STRING) {
            CString s1(&pBuf[t[i].start], t[i].end - t[i].start);
            //                            if (s1 == "elevation") {
            if (s1 == "isValid") {
                s2 = CString(&pBuf[t[i + 1].start], t[i + 1].end - t[i + 1].start);

                if (s2 == "false") {
                    out = "false";
                }
                if (s2 == "true") {
                    out = "true";
                }
            }
        }
    }
    free(t);

    return out;
}

#define HASP_LEFT 7

void psw(int k, char* s);


bool check_password()
{
    CString s, text;
    int n = rand();
    s.Format("Введите пароль для %d", n);

    CInputDialog inD(AfxGetMainWnd( ), _T(""), s, text);

    if (inD.DoModal() != IDOK) return FALSE;

    char ss[256];

    psw(n, ss);

    if (lstrcmpi(ss, inD.m_strInput)) {
      MessageBox(GetActiveWindow(), "Неверный пароль", "",  MB_OK);
      return false;
    }
    return true;
}


/*
DownloadManager2 download_manager1;
DownloadManager2 download_manager2;



void encode(int n, unsigned int *R1, unsigned int *L1)
{
    n = n;

    for (int r = 0; r < 10; r++) {
        for (int i = 0; i < n; i++) {
            unsigned int r1 = R1[i]; 
            unsigned int l1 = L1[i]; 

            unsigned int f1 = (l1 + 0x2345) % 0xFFFF;
            
            R1[i] = l1;
            L1[i] = r1 ^ f1;
        }
    }
}

CString encode(CString text)
{
    int len = text.length();
    int l = (len+1)/2;

    if (l < 64) l = 64;


    vector<unsigned int> v(l*2, 0);

    for (int i = 0; i < len; i++) {
        v[i] = text.data()[i].unicode();
    }


    encode(l, &v[0], &v[l]);

    CString s = "";

    for (int i = 0; i < l*2; i++) {
        s += CString("%1").arg(v[i], 4, 16);
    }

    return s;
}


CString decode(CString text)
{
    int len = text.length()/4;
    int l = (len+1)/2;

    vector<unsigned int> v(len, 0);


    QTextStream ts(&text, QIODeviceBase::Text|QIODeviceBase::ReadOnly);

    int i = 0;

    while (!ts.atEnd()) {
        CString qq = ts.read(4);
        v[i] = qq.toInt(NULL, 16);
        i ++;
    }

    encode(l, &v[l], &v[0]);

    text = "";

    for (int i = 0; i < len; i++) {
        if (v[i] != 0) {
            text += QChar(v[i]);
        }
    }

    return text;
}

*/

CString encode(CString text)
{
    return text;
}

CString decode(CString text)
{
    return text;
}


extern int demoversion;
bool update_now = false;

bool check_update_key() 
{
    return update_now;
}




void write_key(CString key25, unsigned int left);

void exit_app() {
    write_key("", 0);

    AfxMessageBox("Обратитесь к производителю");
    demoversion = 1;
    exit(0);


    PostQuitMessage(0);
    AfxGetMainWnd()->SendMessage(WM_QUIT);
}


#define HALF_SHIFT 16
#define LOW_16_MASK 0xFFFF

unsigned int swap_int(unsigned int num)
{
    return (num >> HALF_SHIFT) ^ ((num & LOW_16_MASK) << HALF_SHIFT);
}

unsigned int F1(unsigned int num, unsigned int key)
{
    num ^= key;
    num *= num;
    return swap_int(num);
}

unsigned int F2(unsigned int num, unsigned int key)
{
    num = swap_int(num);
    num = int(sqrt(num)+0.1);
    num ^= key;
    return num;
}

CString decrypt(CString txt, unsigned int key)
{
    return txt;
}



CString encrypt(CString txt, unsigned int key)
{
    return txt;
}


bool check_protection_add(CString txt)
{
//    QMessageBox::warning(nullptr, "", "Добавил");
    
    return false;
}


CString readHtml(CString url)
{
    CString tmpn;

    tmpn.Format("%s\\tempfile.tmp", getenv("TMP"));

    int ret = URLDownloadToFile(0, url, tmpn, 0, 0);

    if (ret == S_OK) {
        if (IsFile(tmpn)) {
            CString out =  readFile(tmpn);
            DeleteFile(tmpn);
            return out;
        }
    }
    return "";
}

#if 0
void ask_protection(bool force, CString &city, bool & yes, bool & add)
{
    yes = false;
    add = false;

    city = AfxGetApp()->GetProfileString(szSection, "hasp_city", "");

    city =  decode(city);

    unsigned int left = AfxGetApp()->GetProfileInt(szSection, "hasp_n1", 0);
    unsigned int key  = AfxGetApp()->GetProfileInt(szSection, "hasp_n2", 0);

    if (key == 0) {
        key = rand();
    }

    left = F2(left, key);

    if (city == "" || left == 0 || left > HASP_LEFT || force) {
        CString name = "";

        CInputDialog inD(AfxGetMainWnd(), "Кто вы?", "Кто вы?", name);
        if (inD.DoModal() != IDOK) {
            exit_app();
            return;
        }

        city = inD.m_strInput;

        if (!check_password()) {
            exit_app();
            return;
        }

        CString url;
        url.Format("https://lan.avto-glass.kz/check.php?q=%s", city);

        CString s = readHtml(url);

        if (s != "") {
            bool valid = parse_json_user(s);
            if (!valid) {
                exit_app();
                return;
            }
        }
        left = HASP_LEFT;
        add = true;
    }

    left -= 1;

    key = rand();
    left = F1(left, key);

    AfxGetApp()->WriteProfileString(szSection, "hasp_city", encode(city));
    AfxGetApp()->WriteProfileInt(szSection, "hasp_n1", left);
    AfxGetApp()->WriteProfileInt(szSection, "hasp_n2", key);

    yes = true;
}

bool check_protection2()
{
    CString city;
    bool add, yes;

    ask_protection(false, city, yes, add);

    if (yes) {
        CString url;
        url.Format("https://lan.avto-glass.kz/check.php?q=%s", city);

        CString s = readHtml(url);
//        check_protection_reply(s);
    }

    if (add) {
/*
        CString _url = CString("https://lan.avto-glass.kz/check.php?q=%1").arg(city);

        QUrl url = QUrl::fromPercentEncoding(QUrl::toPercentEncoding(_url));

        download_manager2.doDownload(url, check_protection_add);
*/
    }
    return false;
}
#endif


CString rand_text16(int n)
{
    CString txt = "";

    for (int i = 0; i < n; i++) {
        CString s;
        unsigned int c = rand() % 0xFF;
        s.Format("%02x", c);
        txt += s;
    }

    return txt;
}

const int N1 = 5;
const int N2 = 5;

CString key25_encode(unsigned int key0, CString key25)
{
    key0 &= 0xFF;

    CString a1 = rand_text16(N1);
    CString a2 = rand_text16(N2);

    int nkey = sizeof(keys) / sizeof(keys[0]);
    unsigned int key = key0 % nkey;

    CString s;

    int l = key25.GetLength();

    CString key2 = "";
    s.Format("%02x", key0);
    key2 += s;

    for (int k = 0, i = 0; k < l; k += 2, i++) {
        CString s = key25.Mid(k, 2);
        unsigned int c;
        sscanf(s, "%x", &c);
        c ^= (keys[key][i]+256)%256;
        s.Format("%02x", c);
        key2 += s;
    }

    key2 = a1 + key2 + a2;

    return key2;
}

CString key25_decode(CString key25)
{
    key25 = key25.Mid(N1 * 2, key25.GetLength() - (N1 + N2) * 2);


    int nkey = sizeof(keys) / sizeof(keys[0]);
    unsigned int key;

    int l = key25.GetLength();

    CString s = key25.Mid(0, 2);

    sscanf(s, "%x", &key);
    key = key % nkey;

    CString txt = "";

    for (int k = 2, i = 0; k < l; k += 2, i++) {
        CString s = key25.Mid(k, 2);
        unsigned int c;
        sscanf(s, "%x", &c);
        c ^= (keys[key][i] + 256) % 256;
        s.Format("%02x", c);
        txt += s;
    }
    return txt;
}

CString ask_key25()
{
    CString key25 = "";
    unsigned int key = rand();

    update_now = true;

    CInputDialog inD(AfxGetMainWnd(), "", "Введите ключ", key25);
    if (inD.DoModal() != IDOK) {
        return "";
    }

    update_now = false;
//    CString s1 = key25_encode(key, inD.m_strInput);
//    CString s2 = key25_decode(s1);

//s    CString ss;
//    ss.Format("%d\n%s\n%s\n%s", key, inD.m_strInput, s1, s2);
//    AfxMessageBox(ss);

    return inD.m_strInput;
}

void write_key(CString key25, unsigned int left)
{
    unsigned int key = rand();
    left = F1(left, key);

    AfxGetApp()->WriteProfileString(szSection, "hasp_city", key25_encode(key+1, key25_encode(key, key25)));
    AfxGetApp()->WriteProfileInt(szSection, "hasp_n1", left);
    AfxGetApp()->WriteProfileInt(szSection, "hasp_n2", key);
}

CString read_key25()
{
    CString  key25 = AfxGetApp()->GetProfileString(szSection, "hasp_city", "");
    key25 = key25_decode(key25);
    key25 = key25_decode(key25);
    
    unsigned int left = AfxGetApp()->GetProfileInt(szSection, "hasp_n1", 0);
    unsigned int key = AfxGetApp()->GetProfileInt(szSection, "hasp_n2", 0);

    left = F2(left, key);

    if (left == 0 || left > HASP_LEFT) {
        return "";
    }

    write_key(key25, left - 1);

    return key25;
}

CString check_protection_reply(CString txt)
{
    CString valid = parse_json_user(txt);
    return valid;
}



CString ask_server(CString key25)
{
    CString url;

    unsigned int key = rand();

    CString key2 = key25_encode(key, key25);

    url.Format("https://lan.avto-glass.kz/check.php?q=%s", key2);

    CString s = readHtml(url);
//    AfxMessageBox(s);
    return check_protection_reply(s);
}

bool check_protection()
{
//    return true;
    CString key25 = read_key25();

    bool new_key = false;

    if (key25 == "") {
        key25 = ask_key25();
        new_key = true;
        if (key25 == "") {
            exit_app();
            return false;
        }
    }

    CString out = ask_server(key25);

    if (out == "false" || new_key && out != "true") {
        exit_app();
        return false;
    }
    if (out == "true") {
        write_key(key25, HASP_LEFT);
    }
    return true;
}

#if 0

bool check_protection1()
{
    CString key25 = read_key25();

    if (key25 == "") {
        key25 = ask_key25();
        if (key25 == "") {
            exit_app();
            return false;
        }
    }

    if (ask_server(key25)) {
        write_key(key25, HASP_LEFT);
    }

    return true;
}
#endif