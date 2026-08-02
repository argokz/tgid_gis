#pragma once
#include "PropertyGridCombo.h"
#include "PropertyGridInPlaceEdit.h"

#include <map>
#include <vector>
//#include <string>
using namespace std;

// CPropertyGrid

#define WM_PG_ITEMCHANGED WM_USER+486
#define WM_PG_FOCUSCHANGED  WM_USER+487
#define WM_PG_MESSAGE  WM_USER+500


typedef UINT HSECTION;
typedef UINT HITEM;

//typedef int HSECTION;
//typedef int HITEM;

struct Doc
{
  CString ext,filter, path;
};

struct CIsmenenia
{
  double f1, f2, f3, f4, f5, f6, f7, f8, f_sr, f_pr_sr;
};


class ICustomItem;
class CPropertyGrid;

struct CItemChanged
{
    int m_type;
    bool m_is_lookup;
    bool m_undefined;
//    EItemType m_type;
    CString m_name;
    CString m_schema;
    CString m_table;

    CString sql_value_old = "";
    int index_old;
    bool m_undefined_old;
    
    COleVariant var;
    int index;

    CString sql_value;

    int m_nValue;
    double m_dValue;
    CString m_strValue;
    bool m_bValue;
    COleDateTime m_dtValue;
    COLORREF m_clrValue;
    LOGFONT m_lfValue;


    int id;
};

class CPropertyGrid : public CWnd
{
  DECLARE_DYNAMIC(CPropertyGrid)

public:
  // display mode
  enum EDisplayMode
  {
    DM_CATEGORIZED = 0,
    DM_ALPHABETICAL,
    DM_NOSORT
  };

  // editing
  enum EEditMode
  {
    EM_CUSTOM = 0,
    EM_INPLACE,
    EM_DROPDOWN,
    EM_DROPDOWN2,
    EM_MODAL,
    EM_MODAL2
  };

  enum EItemType
  {
    IT_CUSTOM = 0,
    IT_STRING,
    IT_TEXT,
    IT_INTEGER,
    IT_DOUBLE,
    IT_COMBO,
    IT_BOOLEAN,
    IT_DATE,
    IT_DATETIME,
    IT_FILE,
    IT_FOLDER,
    IT_COLOR,
    IT_FONT,
    IT_FUN
  };

public:
  CPropertyGrid();
  virtual ~CPropertyGrid();

  // customization
  bool GetShadeTitles();
  void SetShadeTitles(bool shade_titles);
  bool GetDrawLines();
  void SetDrawLines(bool draw_lines);
  bool GetDrawGutter();
  void SetDrawGutter(bool draw_gutter);
  bool GetFocusDisabled();
  void SetFocusDisabled(bool focus_disabled);
  bool GetBoldModified();
  void SetBoldModified(bool bold_modified);
  bool GetBoldEditables();
  void SetBoldEditables(bool bold_editables);

  // gutter width
  int GetGutterWidth();
  void SetGutterWidth(int gutter_width);

  // custom colors
  void SetTextColor(COLORREF clrText);
  void SetTitleColor(COLORREF clrText);
  void SetBackColor(COLORREF clrBack);
  void SetShadeColor(COLORREF clrShade);
  void SetFocusColor(COLORREF clrFocus);
  void SetHiliteColor(COLORREF clrHilite);
  void SetEditableColor(COLORREF clrEditable);
  void SetDisabledColor(COLORREF clrDisabled);

  // localization
  void SetTrueFalseStrings(CString strTrue, CString strFalse);
  void SetOkCancelStrings(CString strOk, CString strCancel);
  void SetDateTimeStrings(CString strDate, CString strTime);
  void SetUndefinedString(CString strUndefined);
  void SetEmptyString(CString strEmpty);
  void SetName(const char *name);

  bool check_exists(CString fn);


  // add a section
//  HSECTION AddSection(CString title, bool collapsed = false, HSECTION after = -1);
  HSECTION AddSection(CString title, bool collapsed, HSECTION after, int number);

  // add items
  HITEM AddCustomItem(HSECTION, CString label, ICustomItem* pItem, bool editable = true, HITEM after = -1);
  HITEM AddStringItem(HSECTION section, CString label, CString value, bool editable = true, HITEM after = -1);
  HITEM AddTextItem(HSECTION section, CString label, CString value, bool editable = true, HITEM after = -1);
  HITEM AddIntegerItem(HSECTION section, CString label, int value, CString format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDoubleItem(HSECTION section, CString label, double value, CString format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddComboItem(HSECTION section, CString label, const vector<CString>& values, int cur, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddBoolItem(HSECTION section, CString label, bool value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDateItem(HSECTION section, CString label, COleDateTime value, CString format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDateTimeItem(HSECTION section, CString label, COleDateTime value, CString format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddFileItem(HSECTION section, CString label, CString value, CString filter = "", bool editable = true, HITEM after = -1);
  HITEM AddFolderItem(HSECTION section, CString label, CString value, CString title = "", bool editable = true, HITEM after = -1);
  HITEM AddColorItem(HSECTION section, CString label, COLORREF value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddFontItem(HSECTION section, CString label, LOGFONT value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddCurrencyItem(HSECTION section, CString label, COleCurrency value, CString format, bool editable = true, bool undefined = false, HITEM after = -1);

  void SetAddCol(int col);

  // contents
  void ResetContents();
  bool RemoveSection(HSECTION hs);
  bool RemoveItem(HITEM item);
  void ValidateChanges();

  void SetItemLookup(HITEM item, const CString strValue, int index);
  void SetItemLength(HITEM item, int length);
  void SetItemName(HITEM item, const CString name);
  void SetItemHelp(HITEM item, const CString help);
  bool isModified();
  void SetItemTable(HITEM item, const CString schema, const CString table, int id);
  bool getChanges(list<CItemChanged> &lst);
  void SetItemFile(HITEM item, const CString filtr);
  void SetItemFun(HITEM item, const CString fun);
  void SetItemValidate(HITEM item, const CString fun);
  void SetItemHide(HITEM item);
  void SetItemAfter(HITEM item, const CString fun);
  void SetItemBefore(HITEM item, const CString fun);

  void SetItemFunDisable(HITEM item, const CString fun, const map<int, list<CString> > & map_disable, const map <CString, map<int, list<CString> > > & map_disable0);

  
  void SetItemReadOnly(HITEM item, bool readonly = true);
  void SetItemNoEdit(HITEM item);

  void SetItemButtom(HITEM item);


  // status
  int GetNumSections();
  int GetSectionSize(HSECTION hs);

  // get item value
  bool GetItemValue(HITEM item, CString& strValue) const;
  bool GetItemValue(HITEM item, int& nValue) const;
  bool GetItemValue(HITEM item, double& dValue) const;
  bool GetItemValue(HITEM item, bool& bValue) const;
  bool GetItemValue(HITEM item, COleDateTime& dtValue) const;
  bool GetItemValue(HITEM item, COLORREF& clrValue) const;
  bool GetItemValue(HITEM item, LOGFONT& lfValue) const;

  bool SetItemNull(HITEM item);

  // set item value
  bool SetItemValue(HITEM item, const CString strValue);
  bool SetItemValue(HITEM item, const int nValue);
  bool SetItemValue(HITEM item, const double nValue);
  bool SetItemValue(HITEM item, const bool bValue);
  bool SetItemValue(HITEM item, const COleDateTime dtValue);
  bool SetItemValue(HITEM item, const COLORREF clrValue);
  bool SetItemValue(HITEM item, const LOGFONT lfValue);

  // for custom items
  int GetTextMargin();
  CFont* GetFontNormal();
  CFont* GetFontBold();

  // appearance stuff
  void SetDisplayMode(EDisplayMode display_mode);
  void ExpandAll(bool expand);
  void ExpandInit();
  void ExpandCollapse();
  void ExpandSection(HSECTION hs, bool expand);
  bool IsSectionCollapsed(HSECTION hs);

protected:
  class CItem
  {
  public:
    HITEM m_id;
    bool m_buttom;
    bool m_editable;
    bool m_noedit;  // Не редактировать в окне, только по кнопке
    bool m_undefined;
    bool m_modified;
    bool m_modified_for_after;
    EItemType m_type;
    CString m_label;
    CString m_help;

    CString m_name;
    CString m_schema;
    CString m_table;
    int m_id_bd;

    vector<CString> m_options;

    int m_nValue;
    double m_dValue;
    CString m_strValue;
    bool m_bValue;
    COleDateTime m_dtValue;
    COLORREF m_clrValue;
    LOGFONT m_lfValue;
    ICustomItem* m_pCustom;

    bool m_undefined_old;
    int m_nValue_old;
    double m_dValue_old;
    CString m_strValue_old;
    bool m_bValue_old;
    int m_index_old;
    COleDateTime m_dtValue_old;
    COLORREF m_clrValue_old;
    LOGFONT m_lfValue_old;

    CRect m_rcName;
    CRect m_rcValue;

    bool m_is_lookup;
    bool m_is_fun;
    bool m_is_validate;
    bool m_is_after;
    bool m_is_before;

    bool m_is_fun_disable;


    map<int, list<CString> > m_map_disable;
    
    bool m_is_disabled;
    bool m_is_hidden;
    int m_index;
    CString m_strLookup;
    CString m_strFun;
    CString m_strValidate;
    CString m_strAfter;
    CString m_strBefore;
    int m_length;

    int m_col;

    bool operator==(const HITEM& item) const;
    bool operator==(const CString& label) const;

    void ValidateChanges();
  };

  friend bool item_alpha_sort(vector<CPropertyGrid::CItem>::iterator it1, vector<CPropertyGrid::CItem>::iterator it2);

  class CSection
  {
  public:
    HSECTION m_id;

    int m_number;
    
    CString m_title;
    bool m_collapsed;
    vector<CItem> m_items;

    CRect m_rcSign;
    CRect m_rcTitle;

    bool operator==(const HSECTION& section) const;
  };

  vector<CSection> m_sections;
  map<int, CItem> m_old_items;

  HSECTION m_focused_section;
  HITEM m_focused_item;

  EDisplayMode m_display_mode;

  bool m_shade_titles;
  bool m_draw_lines;
  bool m_draw_gutter;
  bool m_focus_disabled;
  bool m_bold_modified;
  bool m_bold_editables;

  int m_gutter_width;
  bool m_resizing_gutter;
  CPoint m_ptLast;

  CFont m_fntNormal;
  CFont m_fntBold;

  int m_line_height;

  CRect m_rect_button;
  CRect m_rect_button_real;
  CWnd* m_control;
  bool m_button_pushed;
  bool m_button_depressed;
  bool m_value_clicked;
  bool m_custom_tracking;

  HSECTION m_section_id;
  HITEM m_item_id;

  CString m_strTrue;
  CString m_strFalse;
  CString m_strOk;
  CString m_strCancel;
//  CString m_strSave;
  CString m_strDate;
  CString m_strTime;
  CString m_strUndefined;
  CString m_strEmpty;

  COLORREF m_clrText;
  COLORREF m_clrTitle;
  COLORREF m_clrBack;
  COLORREF m_clrShade;
  COLORREF m_clrFocus;
  COLORREF m_clrHilite;
  COLORREF m_clrEditable;
  COLORREF m_clrDisabled;

  int m_col;
  int m_cols;

  list<Doc> list_doc;

public:
    CItem* FindItemName(CString name) const;
    CItem* FindItemFunction(CString name) const;
    HITEM FindHItemName(CString name) const;
    int GetID(CString item) const;

    CString ask_yes(CString name, EItemType type, bool look, const COleVariant &value);

//    template <class T> bool SetNameValueAsk(CString name, T value);
    bool SetNameValueAsk(CString name, int value, bool &ask, int & num);
    bool SetNameValueAsk(CString name, double value, bool &ask, int & num);
    bool SetNameValueAsk(CString name, COleDateTime & value, bool &ask, int & num);
    bool SetNameValueLAsk(CString name, int value, bool &ask, int & num);

    bool GetNameValue(CString item, int& nValue) const;
    bool GetNameValue(CString name, CString& strValue) const;
    bool GetNameValue(CString name, double& dValue) const;
    bool GetNameValue(CString name, COleDateTime& dtValue) const;
    
    bool SetNameValue(CString name, const CString strValue);
    bool SetNameValueInit(CString name, const CString strValue);
    bool SetNameValue(CString name, int dValue);
    bool SetNameValue(CString name, double dValue);
    bool SetNameValue(CString name, const COleDateTime dtValue);
    bool SetNameNull(CString name);
    int prop_fun(CString fun, HITEM item);
    int prop_after(CString fun, HITEM item);
    int prop_fun2(int id);
    void viewNsDb(int id);

    void remont_uch(int n, HITEM item);

    void defect_meropr(const char *defect2, HITEM item);
    void defect_doc(const char *defect2, HITEM item);

    void view_documents(CString fun, HITEM item);
    void view_meropr(CString fun, HITEM item);
    void view_meropr2(CString fun, HITEM item);
    void view_link(CString fun, HITEM item);
    void view_filtr(CString fun, HITEM item);
    void view_UT(CItem* pItem);
    void view_UTBIG(CItem* pItem);

    void viewLookup(CItem* pItem);

//    CString getVal(vector<CItem>::iterator& it);
    CString getVal(CItem* pItem) const;

    bool getStr(vector<CItem>::iterator& it, CString strValue);

    void copyHps();
    void copyPts();

    bool checkTU(int i);
    bool checkTU();
    bool check(bool modified, bool cancel);
    bool required(CItem* item);
    bool check_column(CItem *item, CString v, CString &msg, CString &msg_not_null, CString &msg_sezon, int &id);
    bool check_NotNull(CString fn);
    bool notNullMessage(CString fn);

    bool isTU();

    bool isCanMod(HITEM item, CString s_new = "");
    bool isCanModTU(HITEM item);


    bool check_open(CSection* pSection, bool msg = true);
    bool check_openTU(CSection* pSection, bool msg);

    bool change_value_tu(HITEM item);
    void copyVal(const char *tn, const char *pic, int n);
    void copyVal2(const char *fn1, const char *fn2);
    bool copyVal3(const char *fn, const char *fn1, const char *pic, int n);
    bool readIsmemenia(CIsmenenia &ism, int n);
    bool isIsmeneniaZero(int n);
    int lastIsmenenia();

    

    void Print();
    void Print_struct();

    void RecalcOff();
    void RecalcOn();
    void save_profile();

    bool is_potr_real_save;
    double m_otopl = 0, m_gv = 0, m_ventil = 0, m_rez = 0;

  void initCxema(const char *cxema);
  void setCxema1(const char* otop1);
  void InitFont();
  void SetLineHeiht();

  void FocusName(CString name);

protected:
  DECLARE_MESSAGE_MAP()

  // init control
  void InitControl();


  // drawing
  void DrawItem(CDC& dc, int w, int x, int y, vector<CItem>::iterator& it);

  // item management
  CSection* FindSection(HSECTION hs) const;
  CItem* FindItem(HITEM hi) const;
  HITEM AddItem(HSECTION hs, EItemType type, CString label, void* pValue, bool editable, bool undefined, HITEM after);

  // scrolling stuff
  CScrollBar m_scrollbar;
  bool m_scroll_enabled;
  bool m_recalc_on;

  int GetScrollOffset();
  void RecalcLayout();
  CString m_szSection;
  CString m_table;

  // editing
  EEditMode GetEditMode(CItem& item);
  void DeleteEditControl();
  void EditFocusedItem();
  CString getSQLStr(CItem item);

  // movement in list
  void MoveForward(HSECTION& focused_section, HITEM& focused_item);

  // keyboard
  void FocusNextItem();
  void FocusPrevItem();

  void itemChanged(CItem* pItem);

  void onChangeFocus(int focused_item);

  void FocusItem(HITEM item);
  HSECTION getSection(HITEM item);
  void fun_disable(CItem* pItem);
  void set_disable(CItem* pItem, int id, bool yes_no);
  void init_disable();
  void setCxema(const char *otop1, const char *otop2);
  bool isHidden(vector<CItem> &items);


protected:
  virtual void PreSubclassWindow();
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nHitTest, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg LRESULT OnComboSelChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnEditChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnDateChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnMessage(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  LRESULT OnShurfUtverdit(WPARAM wParam, LPARAM lParam);
  LRESULT OnRemontUtverdit(WPARAM wParam, LPARAM lParam);
  LRESULT OnOpresUtverdit(WPARAM wParam, LPARAM lParam);

};
