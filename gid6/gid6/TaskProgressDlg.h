#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTaskProgressDialog dialog

class CTaskProgressDialog : public CDialog
{
// Construction / Destruction
public:
//    CTaskProgressDialog(UINT nCaptionID = 0);   // standard constructor
    CTaskProgressDialog(const char *capt = NULL);   // standard constructor
    ~CTaskProgressDialog();
public:
  BOOL IsSelectMode() const;
  void SetSelectMode(BOOL bSel = TRUE);
  void SetCurTask(int ID);
  void SetTasks(const CStringArray &sTaskList);

    BOOL Create(CWnd *pParent=NULL);

    // Checking for Cancel button
    BOOL CheckCancelButton();
    // Progress Dialog manipulation
    void SetStatus(LPCTSTR lpszMessage);
    void SetRange(int nLower,int nUpper);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt();
    void SetText(const CString &text);

    CString m_text = "";

    
// Dialog Data
    //{{AFX_DATA(CTaskProgressDialog)
  enum { IDD = IDD_TASKPROGRESS_DLG };
    CProgressCtrl m_Progress;
  //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTaskProgressDialog)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
  UINT      m_nCaptionID;
  CString m_capt;
    int       m_nLower;
    int       m_nUpper;
    int       m_nStep;
    
    BOOL      m_bCancel;
    BOOL      m_bParentDisabled;
    BOOL      m_bSelMode;       // Selection mode or hot mode

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 
    void UpdatePercent(int nCurrent);
    void PumpMessages();

    // Generated message map functions
    //{{AFX_MSG(CTaskProgressDialog)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

class Percent2 {
  public:
    Percent2(const char *capt = NULL);
    ~Percent2();
    bool SetValue(int p);
    void SetText(const CString &text);
  
  private:
    int pp;
//    CProgressCtrl *indic;
    CTaskProgressDialog *pm;

};


