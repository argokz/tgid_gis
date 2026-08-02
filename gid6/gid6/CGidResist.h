#pragma once


// CGidResist dialog

class CGidResist : public CDialogEx
{
    DECLARE_DYNAMIC(CGidResist)

public:
    CGidResist(CWnd* pParent, double len);   // standard constructor
    virtual ~CGidResist();

  double m_edit1;
  double m_edit2;
  double m_edit3;

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_UTP_GID_RESIST };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnChange();

    DECLARE_MESSAGE_MAP()
};
