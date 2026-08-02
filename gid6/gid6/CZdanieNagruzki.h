#pragma once


// CZdanieNagruzki dialog

#include "Ultimate Grid/AdoCUG.h"
#include "Ultimate Grid/ADOImpDatasource.h"

class CAdoFile;

class CZdanieNagruzki : public CDialogEx
{
    DECLARE_DYNAMIC(CZdanieNagruzki)

public:
    CZdanieNagruzki(bool all, bool disable_rn, CWnd* pParent);   // standard constructor
    virtual ~CZdanieNagruzki();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ZDANIE_NAGRUZKI };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnSelchangeMkr();
    virtual void OnSelchangeStreet();
    virtual void OnSelchangeDom();
    virtual void OnChangeStreetFind();
    virtual void OnChangeMkrFind();
    virtual void OnChkChangeRn();
    virtual void OnOK();
    virtual void OnChangeVal();


    void setGrid(CString mkr, CString street, CString dom);
    void setNagr(CString mkr, CString street, CString dom);
    void setDoms(CString mkr, CString street);

    void init_dialog();

    DECLARE_MESSAGE_MAP()
public:
    CListBox m_ctrl_mkr;
    CListBox m_ctrl_street;
    CListBox m_ctrl_dom;
    CCheckListBox m_ctrl_rn;

    int m_no_all;
    int m_all;
    
    CEdit m_ctrl_mkr_find;
    CEdit m_ctrl_street_find;
    CEdit m_ctrl_dom_find;
    CString m_mkr_find;
    CString m_street_find;
    CString m_dom_find;
    AdoCug m_grid1;
    virtual BOOL OnInitDialog();

    void init_q(bool all);
    void init(CString mkr, CString steet, CString dom);

    CString m_mkr2 = "";
    CString m_street2 = "";
    CString m_house2 = "";

    CString m_ADDR;
    CString m_ADM_RAYON;
    CString m_RAYON;
    CString m_IST;
    CString m_TG;
    CString m_UCHASTOK;
    double m_ALL;
    double m_OTOP;
    double m_GVS;
    double m_VENT;
    double m_PAR;



private:
  _tagDSList *m_pDSList;
    CString m_mkr = "";
    CString m_street = "";
    CString m_dom = "";

    CString add_q; 
    CString add_q2; 
    CString m_str_rn = "";

    CString m_zdan = ZDANIYA_2;

    int c_ist;
    bool m_disable_rn;


    CAdoFile* m_ado;
public:
    afx_msg void OnLbnSelchangeRn();
};
