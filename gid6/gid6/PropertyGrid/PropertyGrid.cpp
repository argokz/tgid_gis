// PropertyGrid.cpp : implementation file
// PropertyGrid.cpp : implementation file
// PropertyGrid.cpp : implementation file
//

#include "stdafx.h"

#include "CustomItem.h"
#include "PropertyGrid.h"
#include "PropertyGridDirectoryPicker.h"
#include "PropertyGridMonthCalCtrl.h"
#include "DynDialogEx.h"
#include <algorithm>
#include <shlwapi.h>

#include "../win.h"
#include "../resource.h"
#include "../wm_user.h"


#include "../electro.h"


#include <assert.h>

CString getPsAdoName();

CString dateToString(COleDateTime v);
CString dateTimeToString(COleDateTime v);

bool viewVirtualDlg(CWnd* wnd, const char* _bd, const char* q, long& id, const char* cap, CString& text, bool fnd = false, bool add = false, bool edit = false, bool del = false, CString tn = "", bool chk = false);

CString getVirtualDlgText(const char* bd, const char* q, long id);

bool check_season(COleDateTime dt1);
bool get_season(int& y1, int& y2, COleDateTime& date1, COleDateTime& date2);
void set_season();
CString dateToStringV(COleDateTime v);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int regex_match(const char* p, const char* buf, vector<CString>& match);

#define IDC_MONTHCAL 1023

bool LoadFont2(const TCHAR* szSection, LOGFONT& lf, COLORREF& color);

// CPropertyGrid

static const int margin = 2;


// Переводит строку с запятыми в списое

list<CString> readArgs(CString str)
{
    list<CString> args;
    args.clear();
    int nTokenPos = 0;
    CString strToken = str.Tokenize(_T(","), nTokenPos);

    while (!strToken.IsEmpty())
    {
        args.push_back(strToken);
        strToken = str.Tokenize(_T(","), nTokenPos);
    }
    return args;
}



bool StringToDate(CString strValue, COleDateTime & dt)
{
    CString ss = strValue;
    ss.Replace(",", ".");

    int d, m, y;
    int n = sscanf(strValue, "%d.%d.%d", &d, &m, &y);
    if (n == 3) {
        COleDateTime dt2(y, m, d, 0, 0, 0);
        dt = dt2;
        return true;
    }
    return false;
}


IMPLEMENT_DYNAMIC(CPropertyGrid, CWnd)
CPropertyGrid::CPropertyGrid()
{
    is_potr_real_save = false;
    m_section_id = 0;
    m_item_id = 0;
    m_resizing_gutter = false;
    m_button_pushed = false;
    m_button_depressed = false;
    m_value_clicked = false;
    m_custom_tracking = false;
    m_scroll_enabled = false;
    m_draw_lines = true;
    m_shade_titles = true;
    m_draw_gutter = true;
    m_focus_disabled = true;
    m_bold_modified = false;
    m_bold_modified = true;
    m_bold_editables = false;
    m_display_mode = DM_CATEGORIZED;
    m_control = NULL;
    m_recalc_on = true;

    m_rect_button = CRect(0, 0, 0, 0);
    m_rect_button_real = CRect(0, 0, 0, 0);
    m_ptLast = CPoint(0, 0);

    m_strTrue = "Да";
    m_strFalse = "Нет";
    m_strDate = "Дата";
    m_strTime = "Время";
    m_strUndefined = "";
    m_strEmpty = "";

    m_clrBack = GetSysColor(COLOR_WINDOW);
    m_clrShade = GetSysColor(COLOR_3DFACE);
    m_clrText = GetSysColor(COLOR_WINDOWTEXT);
    m_clrTitle = GetSysColor(COLOR_WINDOWTEXT);
    m_clrFocus = GetSysColor(COLOR_HIGHLIGHT);
    m_clrHilite = GetSysColor(COLOR_HIGHLIGHTTEXT);
    m_clrEditable = GetSysColor(COLOR_WINDOWTEXT);
    m_clrDisabled = GetSysColor(COLOR_GRAYTEXT);

    m_clrDisabled = 0x214365;

    m_clrDisabled = 0x00003F;

    m_clrDisabled = 0x040679;


    ///    m_clrDisabled = GetSysColor(COLOR_WINDOWTEXT);



    m_focused_section = -1;
    m_focused_item = -1;

    m_col = 0;
    m_cols = 1;

    m_szSection = szSection;
    m_table = "PropertyGrid";

}

CPropertyGrid::~CPropertyGrid()
{
}

//
// customization
//

bool CPropertyGrid::GetShadeTitles()
{
    return m_shade_titles;
}

void CPropertyGrid::SetShadeTitles(bool shade_titles)
{
    m_shade_titles = shade_titles;
    if (GetSafeHwnd())
        Invalidate();
}

bool CPropertyGrid::GetDrawLines()
{
    return m_draw_lines;
}

void CPropertyGrid::SetDrawLines(bool draw_lines)
{
    m_draw_lines = draw_lines;
    if (GetSafeHwnd())
        Invalidate();
}

bool CPropertyGrid::GetDrawGutter()
{
    return m_draw_gutter;
}

void CPropertyGrid::SetDrawGutter(bool draw_gutter)
{
    m_draw_gutter = draw_gutter;
    if (GetSafeHwnd())
        Invalidate();
}

bool CPropertyGrid::GetFocusDisabled()
{
    return m_focus_disabled;
}

void CPropertyGrid::SetFocusDisabled(bool focus_disabled)
{
    m_focus_disabled = focus_disabled;
    if (GetSafeHwnd())
        Invalidate();
}

bool CPropertyGrid::GetBoldModified()
{
    return m_bold_modified;
}

void CPropertyGrid::SetBoldModified(bool bold_modified)
{
    m_bold_modified = bold_modified;
}

bool CPropertyGrid::GetBoldEditables()
{
    return m_bold_editables;
}

void CPropertyGrid::SetBoldEditables(bool bold_editables)
{
    m_bold_editables = bold_editables;
}

//
// gutter width
//

int CPropertyGrid::GetGutterWidth()
{
    return m_gutter_width;
}

void CPropertyGrid::SetGutterWidth(int gutter_width)
{
    m_gutter_width = gutter_width;
    if (GetSafeHwnd())
        Invalidate();
}

//
// custom colors
//

void CPropertyGrid::SetTextColor(COLORREF clrText)
{
    if (m_clrText == m_clrEditable)
        m_clrEditable = clrText;
    m_clrText = clrText;
    if (GetSafeHwnd())
        Invalidate();
}

void CPropertyGrid::SetTitleColor(COLORREF clrTitle)
{
    m_clrTitle = clrTitle;
    if (GetSafeHwnd())
        Invalidate();
}

void CPropertyGrid::SetBackColor(COLORREF clrBack)
{
    m_clrBack = clrBack;
    if (GetSafeHwnd())
        Invalidate();
}

void CPropertyGrid::SetShadeColor(COLORREF clrShade)
{
    m_clrShade = clrShade;
    if (GetSafeHwnd())
        Invalidate();
}

void CPropertyGrid::SetFocusColor(COLORREF clrFocus)
{
    m_clrFocus = clrFocus;
    if (GetSafeHwnd())
        Invalidate();
}

void CPropertyGrid::SetHiliteColor(COLORREF clrHilite)
{
    m_clrHilite = clrHilite;
    if (GetSafeHwnd())
        Invalidate();
}

void CPropertyGrid::SetEditableColor(COLORREF clrEditable)
{
    m_clrEditable = clrEditable;
    if (GetSafeHwnd())
        Invalidate();
}

void CPropertyGrid::SetDisabledColor(COLORREF clrDisabled)
{
    m_clrDisabled = clrDisabled;
    if (GetSafeHwnd())
        Invalidate();
}

//
// localization
//

void CPropertyGrid::SetTrueFalseStrings(CString strTrue, CString strFalse)
{
    m_strTrue = strTrue;
    m_strFalse = strFalse;
}

void CPropertyGrid::SetOkCancelStrings(CString strOk, CString strCancel)
{
    m_strOk = strOk;
    m_strCancel = strCancel;
    //    m_strSave = "Сохранить";
}

void CPropertyGrid::SetDateTimeStrings(CString strDate, CString strTime)
{
    m_strDate = strDate;
    m_strTime = strTime;
}

void CPropertyGrid::SetUndefinedString(CString strUndefined)
{
    m_strUndefined = strUndefined;
}

void CPropertyGrid::SetEmptyString(CString strEmpty)
{
    m_strEmpty = strEmpty;
}

//
// appearance
//

void CPropertyGrid::SetDisplayMode(EDisplayMode display_mode)
{
    m_display_mode = display_mode;
    RecalcLayout();
}

void CPropertyGrid::ExpandAll(bool expand)
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
        if (!it->m_collapsed || check_open(&*it, false)) {
            it->m_collapsed = !expand;
        }
    }
    RecalcLayout();
}


void CPropertyGrid::ExpandInit()
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
        if (!it->m_collapsed && !check_open(&*it, false)) {
            it->m_collapsed = true;
        }
    }
    RecalcLayout();
}


void CPropertyGrid::ExpandCollapse()
{
    bool expand = false;

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
        if (it->m_collapsed && check_open(&*it, false)) {
            expand = true;
            break;
        }
    }
    ExpandAll(expand);
}


void CPropertyGrid::ExpandSection(HSECTION hs, bool expand)
{
    CSection* pSection = FindSection(hs);
    if (pSection)
    {
        int number = pSection->m_number;

        if (expand) {
            for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
                if (it->m_number != number && it->m_number > 0) {
                    it->m_collapsed = true;
                }
            }
        }

        if (pSection->m_collapsed) {
            if (check_open(pSection, true)) {
                pSection->m_collapsed = !expand;
            }
        }
        else {
            pSection->m_collapsed = !expand;
        }
        RecalcLayout();
    }
}

bool CPropertyGrid::IsSectionCollapsed(HSECTION hs)
{
    CSection* pSection = FindSection(hs);
    if (pSection)
        return pSection->m_collapsed;
    return false;
}

//
// item management
//

bool CPropertyGrid::CItem::operator==(const HITEM& item) const
{
    return m_id == item;
}

bool CPropertyGrid::CItem::operator==(const CString& label) const
{
    return m_label == label;
}

bool CPropertyGrid::CSection::operator==(const HSECTION& section) const
{
    return m_id == section;
}

void CPropertyGrid::CItem::ValidateChanges()
{
    // save the values
    m_undefined_old = m_undefined;
    m_nValue_old = m_nValue;
    m_dValue_old = m_dValue;
    m_strValue_old = m_strValue;
    m_bValue_old = m_bValue;
    m_dtValue_old = m_dtValue;
    m_clrValue_old = m_clrValue;
    m_index_old = m_index;

    memcpy(&m_lfValue_old, &m_lfValue, sizeof(LOGFONT));

    // callback for custom
    if (m_type == IT_CUSTOM)
        m_pCustom->ValidateChanges();
}

HSECTION CPropertyGrid::AddSection(CString title, bool collapsed, HSECTION after, int number)
{
    CString tt;
    tt.Format("%s/%s", m_table, title);
    collapsed = AfxGetApp()->GetProfileInt(m_szSection, title, collapsed);


    // build it
    CSection section;
    section.m_id = m_section_id;
    section.m_title = title;
    section.m_collapsed = collapsed;
    section.m_number = number;

    // insert it
    // if after does not exist then it is appended
    vector<CSection>::iterator it = find(m_sections.begin(), m_sections.end(), after);
    m_sections.insert(it, section);


    ExpandInit();


    // done

    RecalcLayout();
    return m_section_id++;
}

HITEM CPropertyGrid::AddItem(HSECTION hs, EItemType type, CString label, void* pValue, bool editable, bool undefined, HITEM after)
{
    // check section exists
    vector<CSection>::iterator it = find(m_sections.begin(), m_sections.end(), hs);
    if (it == m_sections.end())
        return -1;

    // check item does not already exists
    //  vector<CItem>::iterator it2 = find(it->m_items.begin(), it->m_items.end(), label);
    //  if (it2 != it->m_items.end())
    //    return -1;

    // build the item
    CItem item;
    item.m_id = m_item_id++;
    item.m_type = type;
    item.m_label = label;
    item.m_name = label;
    item.m_help = "";

    item.m_buttom = false;
    item.m_editable = editable;
    item.m_noedit = false;
    
    item.m_undefined = undefined;
    item.m_is_lookup = false;
    item.m_is_fun = false;
    item.m_is_validate = false;
    item.m_is_fun_disable = false;

    item.m_is_disabled = false;
    item.m_is_hidden = false;
    item.m_strFun = "";
    item.m_strValidate = "";
    item.m_strAfter = "";
    item.m_strBefore = "";
    item.m_modified = false;
    item.m_modified_for_after = false;
    item.m_schema = "";
    item.m_index = -1;
    item.m_id_bd = -1;
    item.m_length = -1;

    item.m_col = m_col;

    // assign the value
    if (type == IT_CUSTOM) item.m_pCustom = (ICustomItem*)pValue;
    else if (type == IT_STRING || type == IT_TEXT || type == IT_FILE || type == IT_FOLDER) item.m_strValue = *(CString*)pValue;
    else if (type == IT_COMBO || type == IT_INTEGER) item.m_nValue = *(int*)pValue;
    else if (type == IT_DOUBLE) item.m_dValue = *(double*)pValue;
    else if (type == IT_BOOLEAN) item.m_bValue = *(bool*)pValue;
    else if (type == IT_DATE || type == IT_DATETIME) item.m_dtValue = *(COleDateTime*)pValue;
    else if (type == IT_COLOR) item.m_clrValue = *(COLORREF*)pValue;
    else if (type == IT_FONT) memcpy(&item.m_lfValue, pValue, sizeof(LOGFONT));
    else assert(false);

    // finish and add
    item.ValidateChanges();
    it->m_items.push_back(item);
    RecalcLayout();

    m_old_items[item.m_id] = item;

    return item.m_id;
}

HITEM CPropertyGrid::AddCustomItem(HSECTION section, CString label, ICustomItem* pItem, bool editable, HITEM after)
{
    pItem->m_pGrid = this;
    return AddItem(section, IT_CUSTOM, label, pItem, editable, false, after);
}

HITEM CPropertyGrid::AddStringItem(HSECTION section, CString label, CString value, bool editable, HITEM after)
{
    return AddItem(section, IT_STRING, label, &value, editable, false, after);
}

HITEM CPropertyGrid::AddTextItem(HSECTION section, CString label, CString value, bool editable, HITEM after)
{
    return AddItem(section, IT_TEXT, label, &value, editable, false, after);
}

HITEM CPropertyGrid::AddIntegerItem(HSECTION section, CString label, int value, CString format, bool editable, bool undefined, HITEM after)
{
    HITEM it = AddItem(section, IT_INTEGER, label, &value, editable, undefined, after);
    CItem* pItem = FindItem(it);
    if (pItem) pItem->m_options.push_back(format);
    return it;
}


HITEM CPropertyGrid::AddDoubleItem(HSECTION section, CString label, double value, CString format, bool editable, bool undefined, HITEM after)
{
    HITEM it = AddItem(section, IT_DOUBLE, label, &value, editable, undefined, after);
    CItem* pItem = FindItem(it);
    if (pItem) pItem->m_options.push_back(format);
    return it;
}

HITEM CPropertyGrid::AddComboItem(HSECTION section, CString label, const vector<CString>& values, int cur, bool editable, bool undefined, HITEM after)
{
    HITEM it = AddItem(section, IT_COMBO, label, &cur, editable, undefined, after);
    CItem* pItem = FindItem(it);
    if (pItem) pItem->m_options = values;
    return it;
}

HITEM CPropertyGrid::AddBoolItem(HSECTION section, CString label, bool value, bool editable, bool undefined, HITEM after)
{
    return AddItem(section, IT_BOOLEAN, label, &value, editable, undefined, after);
}


HITEM CPropertyGrid::AddDateItem(HSECTION section, CString label, COleDateTime value, CString format, bool editable, bool undefined, HITEM after)
{
    HITEM it = AddItem(section, IT_DATE, label, &value, editable, undefined, after);
    CItem* pItem = FindItem(it);
    if (pItem) pItem->m_options.push_back(format);
    return it;
}

HITEM CPropertyGrid::AddDateTimeItem(HSECTION section, CString label, COleDateTime value, CString format, bool editable, bool undefined, HITEM after)
{
    HITEM it = AddItem(section, IT_DATETIME, label, &value, editable, undefined, after);
    CItem* pItem = FindItem(it);
    if (pItem) pItem->m_options.push_back(format);
    return it;
}

HITEM CPropertyGrid::AddCurrencyItem(HSECTION section, CString label, COleCurrency cy, CString format, bool editable, bool undefined, HITEM after)
{
    CString value = cy.Format();
    HITEM it = AddItem(section, IT_STRING, label, &value, editable, undefined, after);
    CItem* pItem = FindItem(it);
    if (pItem) {
//        pItem->m_options.push_back(format);
        SetItemLength(it, 20);
    }
    return it;
}



HITEM CPropertyGrid::AddFileItem(HSECTION section, CString label, CString value, CString filter, bool editable, HITEM after)
{
    HITEM it = AddItem(section, IT_FILE, label, &value, editable, false, after);
    CItem* pItem = FindItem(it);
    if (pItem) pItem->m_options.push_back(filter);
    return it;
}

HITEM CPropertyGrid::AddFolderItem(HSECTION section, CString label, CString value, CString title, bool editable, HITEM after)
{
    HITEM it = AddItem(section, IT_FOLDER, label, &value, editable, false, after);
    CItem* pItem = FindItem(it);
    if (pItem) pItem->m_options.push_back(title);
    return it;
}

HITEM CPropertyGrid::AddColorItem(HSECTION section, CString label, COLORREF value, bool editable, bool undefined, HITEM after)
{
    return AddItem(section, IT_COLOR, label, &value, editable, undefined, after);
}

HITEM CPropertyGrid::AddFontItem(HSECTION section, CString label, LOGFONT value, bool editable, bool undefined, HITEM after)
{
    return AddItem(section, IT_FONT, label, &value, editable, undefined, after);
}

void CPropertyGrid::ResetContents()
{
    m_sections.clear();
    m_section_id = 0;
    m_item_id = 0;
    RecalcLayout();
}

bool CPropertyGrid::RemoveSection(HSECTION hs)
{
    vector<CSection>::iterator it = find(m_sections.begin(), m_sections.end(), hs);
    if (it == m_sections.end()) return false;
    m_sections.erase(it);
    return true;
}

bool CPropertyGrid::RemoveItem(HITEM item)
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        vector<CItem>::iterator it2 = find(it->m_items.begin(), it->m_items.end(), item);
        if (it2 != it->m_items.end())
        {
            it->m_items.erase(it2);
            return true;
        }
    }
    return false;
}

int CPropertyGrid::GetNumSections()
{
    return int(m_sections.size());
}

int CPropertyGrid::GetSectionSize(HSECTION hs)
{
    CSection* pSection = FindSection(hs);
    if (pSection) return int(pSection->m_items.size());
    return 0;
}

void CPropertyGrid::ValidateChanges()
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
            it2->ValidateChanges();
    }
}

CPropertyGrid::CSection* CPropertyGrid::FindSection(HSECTION hs) const
{
    vector<CSection>::const_iterator it = find(m_sections.begin(), m_sections.end(), hs);
    if (it == m_sections.end()) return NULL;
    return const_cast<CSection*>(&(*it));
}



CPropertyGrid::CItem* CPropertyGrid::FindItem(HITEM hi) const
{
    for (vector<CSection>::const_iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        vector<CItem>::const_iterator it2 = find(it->m_items.begin(), it->m_items.end(), hi);
        if (it2 != it->m_items.end())
            return const_cast<CItem*>(&(*it2));
    }
    return NULL;
}


HITEM CPropertyGrid::FindHItemName(CString name) const
{
    CItem* it = FindItemName(name);
    if (it) return it->m_id;

    return 0;
}

CPropertyGrid::CItem* CPropertyGrid::FindItemName(CString name) const
{
    for (vector<CSection>::const_iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        vector<CItem>::const_iterator it2 = it->m_items.begin();
        for (; it2 != it->m_items.end(); it2++)
        {
            if (it2->m_name == name) {
                return const_cast<CItem*>(&(*it2));
            }
        }
    }
    return NULL;
}

CPropertyGrid::CItem* CPropertyGrid::FindItemFunction(CString name) const
{
    for (vector<CSection>::const_iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        vector<CItem>::const_iterator it2 = it->m_items.begin();
        for (; it2 != it->m_items.end(); it2++)
        {
            if (it2->m_strFun == name) {
                return const_cast<CItem*>(&(*it2));
            }
        }
    }
    return NULL;
}




bool CPropertyGrid::GetItemValue(HITEM item, CString& strValue) const
{
    strValue = "";

    CString s;
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;
    if (pItem->m_undefined) return false;

    // check
    if (pItem->m_type == IT_STRING || pItem->m_type == IT_TEXT || pItem->m_type == IT_FILE || pItem->m_type == IT_FOLDER)
    {
        strValue = pItem->m_strValue;
        return true;
    }
    else if (pItem->m_type == IT_COMBO)
    {
        if (pItem->m_nValue < 0 || pItem->m_nValue > int(pItem->m_options.size()) - 1) return false;
        strValue = pItem->m_options[pItem->m_nValue];
        return true;
    }
    else // if (pItem->m_type == IT_DOUBLE)
    {
        strValue = getVal(pItem);
        return true;
    }
    return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, int& nValue) const
{
    nValue = 0;
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;
    if (pItem->m_undefined) return false;

    if (pItem->m_is_lookup) {
        nValue = pItem->m_index;
        return true;
    }

    // check
    if (pItem->m_type == IT_COMBO || pItem->m_type == IT_INTEGER)
    {
        nValue = pItem->m_nValue;
        return true;
    }
    return false;
}

int CPropertyGrid::GetID(CString name) const
{
    CItem* item = FindItemName(name);

    if (item) {
        return item->m_id_bd;
    }
    return 0;
}





bool CPropertyGrid::GetNameValue(CString name, int& nValue) const
{
    CItem* item = FindItemName(name);

    if (item) {
        return GetItemValue(item->m_id, nValue);
    }
    return false;
}


bool CPropertyGrid::GetNameValue(CString name, CString& strValue) const
{
    CItem* item = FindItemName(name);

    if (item) {
        return GetItemValue(item->m_id, strValue);
    }
    return false;
}

bool CPropertyGrid::GetNameValue(CString name, COleDateTime& dtValue) const
{
    CItem* item = FindItemName(name);

    if (item) {
        return GetItemValue(item->m_id, dtValue);
    }
    return false;
}



bool CPropertyGrid::GetNameValue(CString name, double& dValue) const
{
    CItem* item = FindItemName(name);

    if (item) {
        return GetItemValue(item->m_id, dValue);
    }
    return false;
}

bool CPropertyGrid::SetNameValue(CString name, double dValue)
{
    CItem* item = FindItemName(name);

    if (item) {
        return SetItemValue(item->m_id, dValue);
    }
    return false;
}

bool CPropertyGrid::SetNameValue(CString name, int dValue)
{
    CItem* item = FindItemName(name);

    if (item) {
        return SetItemValue(item->m_id, dValue);
    }
    return false;
}


bool CPropertyGrid::SetNameValue(CString name, const CString strValue)
{
    CItem* item = FindItemName(name);

    if (item) {
        return SetItemValue(item->m_id, strValue);
    }
    return false;
}


bool CPropertyGrid::SetNameValueInit(CString name, const CString strValue)
{
    CItem* item = FindItemName(name);

    if (item) {
        bool b = SetItemValue(item->m_id, strValue);
        item->m_modified = false;
        item->ValidateChanges();
        return b;
        //        }
    }
    return false;
}




bool CPropertyGrid::SetNameValue(CString name, const COleDateTime dtValue)
{
    CItem* item = FindItemName(name);

    if (item) {
        return SetItemValue(item->m_id, dtValue);
    }
    return false;
}




bool CPropertyGrid::GetItemValue(HITEM item, double& dValue) const
{
    dValue = 0;
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;
    if (pItem->m_undefined) return false;

    // check
    if (pItem->m_type == IT_DOUBLE)
    {
        dValue = pItem->m_dValue;
        return true;
    }
    return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, bool& bValue) const
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;
    if (pItem->m_undefined) return false;

    // check
    if (pItem->m_type == IT_BOOLEAN)
    {
        bValue = pItem->m_bValue;
        return true;
    }
    return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, COleDateTime& dtValue) const
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;
    if (pItem->m_undefined) return false;

    // check
    if (pItem->m_type == IT_DATE || pItem->m_type == IT_DATETIME)
    {
        dtValue = pItem->m_dtValue;
        return true;
    }
    return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, COLORREF& clrValue) const
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;
    if (pItem->m_undefined) return false;

    // check
    if (pItem->m_type == IT_COLOR)
    {
        clrValue = pItem->m_clrValue;
        return true;
    }
    return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, LOGFONT& lfValue) const
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;
    if (pItem->m_undefined) return false;

    // check
    if (pItem->m_type == IT_FONT)
    {
        lfValue = pItem->m_lfValue;
        return true;
    }
    return false;
}

bool CPropertyGrid::SetNameNull(CString name)
{
    CItem* item = FindItemName(name);

    if (item) {
        return SetItemNull(item->m_id);
    }
    return false;
}



bool CPropertyGrid::SetItemNull(HITEM item)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    //    pItem->m_nValue = nValue;
    pItem->m_undefined = true;
    pItem->m_modified = true;
    Invalidate();
    return true;
}



bool CPropertyGrid::SetItemValue(HITEM item, const CString strValue)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    if (!isCanMod(item)) return false;

    // check
    if (pItem->m_type == IT_STRING || pItem->m_type == IT_TEXT || pItem->m_type == IT_FILE || pItem->m_type == IT_FOLDER)
    {
        pItem->m_strValue = strValue;
        pItem->m_undefined = false;
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }
    else if (pItem->m_type == IT_DATE) {
        COleDateTime dtValue;

        if (StringToDate(strValue, dtValue)) {
            return SetItemValue(item, dtValue);
        }
/*
        int d, m, y;
        int n = sscanf(strValue, "%d.%d.%d", &d, &m, &y);
        if (n == 3) {
            COleDateTime dtValue(y, m, d, 0, 0, 0);
            return SetItemValue(item, dtValue);
        }
*/
    }
    else if (pItem->m_type == IT_DOUBLE) {
        double d;
        CString ss = strValue;
        ss.Replace(",", ".");

        int n = sscanf(ss, "%lg", &d);
        if (n == 1) {
            return SetItemValue(item, d);
        }
    }
    else if (pItem->m_type == IT_INTEGER) {
        int d;
        CString ss = strValue;
        ss.Replace(" ", "");
        ss.Replace("\xA0", "");

        int n = sscanf(ss, "%d", &d);
        if (n == 1) {
            return SetItemValue(item, d);
        }
    }

    return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const int nValue)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    if (!isCanMod(item)) return false;

    if (pItem->m_is_lookup) {
        pItem->m_index = nValue;

        pItem->m_strValue = getVirtualDlgText(pItem->m_schema, pItem->m_strLookup, nValue);
        pItem->m_index = nValue;

        pItem->m_bValue = (nValue != 0);
        pItem->m_undefined = (pItem->m_index == -1);
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }

    // check
    if (pItem->m_type == IT_COMBO || pItem->m_type == IT_INTEGER || pItem->m_type == IT_DOUBLE)
    {
        pItem->m_nValue = nValue;
        pItem->m_undefined = false;
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }
    return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const double dValue)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    if (!isCanMod(item)) return false;

    // check
    if (pItem->m_type == IT_DOUBLE)
    {
        pItem->m_dValue = dValue;
        pItem->m_undefined = false;
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }
    return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const bool bValue)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    if (!isCanMod(item)) return false;


    // check
    if (pItem->m_type == IT_BOOLEAN)
    {
        pItem->m_bValue = bValue;
        pItem->m_undefined = false;
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }
    return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const COleDateTime dtValue)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    if (!isCanMod(item)) return false;

    // check
    if (pItem->m_type == IT_DATE || pItem->m_type == IT_DATETIME)
    {
        pItem->m_dtValue = dtValue;
        pItem->m_undefined = false;
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }
    return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const COLORREF clrValue)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    if (!isCanMod(item)) return false;

    // check
    if (pItem->m_type == IT_COLOR)
    {
        pItem->m_clrValue = clrValue;
        pItem->m_undefined = false;
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }
    return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const LOGFONT lfValue)
{
    // get the item
    CItem* pItem = FindItem(item);
    if (pItem == NULL) return false;

    if (!isCanMod(item)) return false;

    // check
    if (pItem->m_type == IT_FONT)
    {
        memcpy(&pItem->m_lfValue, &lfValue, sizeof(LOGFONT));
        pItem->m_undefined = false;
        pItem->m_modified = true;
        itemChanged(pItem);
        Invalidate();
        return true;
    }
    return false;
}

int CPropertyGrid::GetTextMargin()
{
    return 2 * margin;
}

CFont* CPropertyGrid::GetFontNormal()
{
    return &m_fntNormal;
}

CFont* CPropertyGrid::GetFontBold()
{
    return &m_fntBold;
}



BEGIN_MESSAGE_MAP(CPropertyGrid, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_LBUTTONUP()
    ON_WM_VSCROLL()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(WM_PG_COMBOSELCHANGED, OnComboSelChanged)
    ON_MESSAGE(WM_PG_ENDLABELEDIT, OnEditChanged)
    ON_MESSAGE(WM_PG_DATESELCHANGED, OnDateChanged)
    ON_MESSAGE(WM_PG_MESSAGE, OnMessage)

    ON_MESSAGE(WM_USER_SHURF_UTVERDIT, OnShurfUtverdit)
    ON_MESSAGE(WM_USER_REMONT_UTVERDIT, OnRemontUtverdit)
//    ON_MESSAGE(WM_USER_OPRES_UTVERDIT, OnOpresUtverdit)


    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEWHEEL()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_GETDLGCODE()
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CPropertyGrid::SetLineHeiht()
{
    // get line height
    CDC* pDC = GetDC();
    CFont* pOldFont = pDC->SelectObject(&m_fntNormal);
    m_line_height = pDC->GetTextExtent("Gg").cy + 2 * margin;
    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);
}


void CPropertyGrid::InitFont()
{
    // fonts
    LOGFONT lf;
    COLORREF color;

    if (LoadFont2(_T("PropGridFont"), lf, color)) {
        m_fntNormal.CreateFontIndirect(&lf);
        lf.lfWeight = FW_BOLD;
        m_fntBold.CreateFontIndirect(&lf);
    }
    else {
        if (GetParent() && GetParent()->GetFont())
        {
            CFont* pFont = GetParent()->GetFont();
            pFont->GetLogFont(&lf);
            m_fntNormal.CreateFontIndirect(&lf);
            lf.lfWeight = FW_BOLD;
            m_fntBold.CreateFontIndirect(&lf);
        }
        else
        {
            //      m_fntNormal.CreatePointFont(85, "Tahoma");
            m_fntNormal.CreatePointFont(100, "Tahoma");
            m_fntNormal.GetLogFont(&lf);
            lf.lfWeight = FW_BOLD;
            m_fntBold.CreateFontIndirect(&lf);
        }
    }
    SetLineHeiht();
    Invalidate();
}


//
// creation and window stuff
//

void CPropertyGrid::InitControl()
{
    // first gutter
    CRect rc;
    GetClientRect(&rc);
    m_gutter_width = rc.Width() / 2;

    // check if already done
    if (m_fntNormal.GetSafeHandle() == NULL)
    {
        InitFont();
    }
    else {
        SetLineHeiht();
    }

    // styles
    ModifyStyle(0, WS_CLIPCHILDREN);

    // try to get some CStrings
    if (m_strOk.IsEmpty())
    {
        m_strOk = "OK";
        if (GetParent() && GetParent()->GetDlgItem(IDOK))
        {
            CString strOk;
            GetParent()->GetDlgItem(IDOK)->GetWindowText(strOk);
            //      m_strOk = strOk;
        }
    }
    if (m_strCancel.IsEmpty())
    {
        m_strCancel = "Отменить";
        if (GetParent() && GetParent()->GetDlgItem(IDCANCEL))
        {
            CString strCancel;
            GetParent()->GetDlgItem(IDCANCEL)->GetWindowText(strCancel);
            //      m_strCancel = strCancel;
        }
    }

    /*
        if (m_strSave.IsEmpty())
        {
            m_strSave = "Отменить";
            if (GetParent() && GetParent()->GetDlgItem(IDCANCEL))
            {
                CString strSave;
                GetParent()->GetDlgItem(IDCANCEL)->GetWindowText(strSave);
                //      m_strCancel = strCancel;
            }
        }
    */


}

int CPropertyGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;
    InitControl();
    return 0;
}

void CPropertyGrid::PreSubclassWindow()
{
    InitControl();
    CWnd::PreSubclassWindow();
}

void CPropertyGrid::save_profile()
{
    CString tt;

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
        tt.Format("%s/%s", m_table, it->m_title);
        AfxGetApp()->WriteProfileInt(m_szSection, it->m_title, it->m_collapsed);
    }
}


void CPropertyGrid::OnDestroy()
{
    save_profile();

    DeleteEditControl();
    CWnd::OnDestroy();
}

void CPropertyGrid::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (m_scrollbar.GetSafeHwnd())
    {
        CRect rect;
        GetClientRect(&rect);
        m_scrollbar.MoveWindow(rect.right - GetSystemMetrics(SM_CXVSCROLL), rect.top, GetSystemMetrics(SM_CXVSCROLL), rect.Height());
        RecalcLayout();
    }
}


bool CPropertyGrid::isHidden(vector<CItem> &items)
{
    bool is_hidden = true;

    for (vector<CItem>::iterator it2 = items.begin(); it2 != items.end(); ++it2)
    {
        if (!it2->m_is_hidden) {
            is_hidden = false;
        }
    }
    return is_hidden;
}


//
// painting
//

BOOL CPropertyGrid::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

bool item_alpha_sort(vector<CPropertyGrid::CItem>::iterator it1, vector<CPropertyGrid::CItem>::iterator it2)
{
    //    return (it1->m_label.compare(it2->m_label) < 0);
    return (it1->m_label.Compare(it2->m_label) < 0);
}

void CPropertyGrid::OnPaint()
{
    CItem* ii = FindItemName("fio_utverzhdaemogo");

    void get_map_vis(map <CString, map<CString, list<CString> > >&_map_vis);

    map <CString, map<CString, list<CString> > > map_vis;
    get_map_vis(map_vis);
    if (map_vis.size() == 1) {
        auto it = map_vis.begin();
        setCxema1(it->first);
    }

    init_disable();

    // stuff needed
    const int sign_size = 8;

    // the scrollbar offset
    int top = GetScrollOffset();

    // the rect
    CRect rc_dummy;
    GetClientRect(&rc_dummy);
    if (m_scroll_enabled)
        rc_dummy.right -= GetSystemMetrics(SM_CXVSCROLL);

    // make sure we do not modify this one
    // because we use it to bitblt
    const CRect rc(rc_dummy);

    // stuff for flicker free drawing
    CDC dcMem;
    CBitmap bmpMem;
    CPaintDC dc(this);

    // create and configure the memdc
    dcMem.CreateCompatibleDC(&dc);
    bmpMem.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
    CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);

    // brush needed
    CBrush brushTitle;
    brushTitle.CreateSolidBrush(m_clrTitle);

    // pen needed
    CPen penShade(PS_SOLID, 1, m_clrShade);
    CPen penTitle(PS_SOLID, 1, m_clrTitle);

    // to make sure we won't leak gdi resources
    CBrush* pOldBrush = dcMem.SelectObject(&brushTitle);
    CPen* pOldPen = dcMem.SelectObject(&penShade);
    CFont* pOldFont = dcMem.SelectObject(&m_fntNormal);

    // needed
    int w = rc.Width();

    // blank
    dcMem.FillSolidRect(rc, m_clrBack);
    dcMem.SetBkMode(TRANSPARENT);

    // IsEmpty text
    if (m_sections.empty())
    {
        CRect rect = rc;
        rect.top += 10;
        rect.DeflateRect(rect.Width() / 4, 0);
        dcMem.DrawText(m_strEmpty, rect, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
    }
    else
    {
        // needed
        int sign_left = margin;

        // we start here
        int y = -top;

        // alphabetical needs special
        if (m_display_mode == DM_ALPHABETICAL)
        {
            // put all the items in a vector
            vector<vector<CItem>::iterator> lst;
            for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
            {
                for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
                    lst.push_back(it2);
            }

            // sort the vector
            sort(lst.begin(), lst.end(), item_alpha_sort);

            // display the items
            for (vector<vector<CItem>::iterator>::iterator it2 = lst.begin(); it2 != lst.end(); ++it2)
            {
                // first reset
                (*it2)->m_rcName.SetRectEmpty();
                (*it2)->m_rcValue.SetRectEmpty();

                // draw if visible
                (*it2)->m_rcName = CRect(0, y, w, y + m_line_height);
                CRect rcInter = (*it2)->m_rcName;
                rcInter.IntersectRect(rc, rcInter);
                if (!rcInter.IsRectEmpty())
                    DrawItem(dcMem, w, sign_left + sign_size, y, *it2);

                // next line
                if ((*it2)->m_col == m_cols - 1) {
                    y += m_line_height;
                }
            }
        }
        else
        {
            // next iterate on sections
            for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
            {
                bool is_hidden = isHidden(it->m_items);
                if (!is_hidden) {

                    // reset
                    it->m_rcSign.SetRectEmpty();
                    it->m_rcTitle.SetRectEmpty();

                    // is visible?
                    it->m_rcTitle = CRect(0, y, w, y + m_line_height);
                    CRect rcInter = it->m_rcTitle;
                    rcInter.IntersectRect(rcInter, rc);
                    if (m_display_mode == DM_CATEGORIZED && !rcInter.IsRectEmpty())
                    {

                        //                    if (!is_hidden) {


                                            // first shade rect
                        if (m_shade_titles)
                            dcMem.FillSolidRect(0, y, w, m_line_height, m_clrShade);

                        // now draw a separator lines
                        if (m_draw_lines)
                        {
                            dcMem.SelectObject(&penShade);
                            dcMem.MoveTo(0, y);
                            dcMem.LineTo(w + 1, y);
                            dcMem.MoveTo(0, y + m_line_height);
                            dcMem.LineTo(w + 1, y + m_line_height);
                        }

                        // now draw gutter
                        if (m_draw_gutter)
                        {
                            dcMem.SelectObject(&penShade);
                            dcMem.MoveTo(m_gutter_width, y);
                            dcMem.LineTo(m_gutter_width, y + m_line_height + 1);
                        }

                        // now draw collapse sign
                        int sign_top = y + margin + 2;
                        dcMem.SelectObject(&penTitle);
                        it->m_rcSign = CRect(sign_left, sign_top, sign_left + sign_size + 1, sign_top + sign_size + 1);
                        dcMem.FrameRect(it->m_rcSign, &brushTitle);
                        dcMem.MoveTo(sign_left + 2, sign_top + sign_size / 2);
                        dcMem.LineTo(sign_left + 2 + sign_size / 2 + 1, sign_top + sign_size / 2);
                        if (it->m_collapsed)
                        {
                            dcMem.MoveTo(sign_left + sign_size / 2, sign_top + 2);
                            dcMem.LineTo(sign_left + sign_size / 2, sign_top + 2 + sign_size / 2 + 1);
                        }

                        // prepare draw text
                        int title_left = sign_left + sign_size + 2 * margin;
                        int title_top = y;
                        dcMem.SelectObject(&m_fntBold);
                        it->m_rcTitle = CRect(title_left, title_top, w, title_top + m_line_height);

                        // draw focus rect
                        if (m_focused_section == it->m_id)
                        {
                            CSize sz = dcMem.GetTextExtent(it->m_title);
                            int rect_left = title_left;
                            int rect_top = title_top + (m_line_height - sz.cy) / 2;
                            int rect_width = sz.cx + 3 * margin;
                            int rect_height = sz.cy;
                            dcMem.DrawFocusRect(CRect(rect_left, rect_top, rect_left + rect_width, rect_top + rect_height));
                        }

                        // now draw text
                        dcMem.SetTextColor(m_clrTitle);

                        dcMem.DrawText(it->m_title, CRect(title_left + GetTextMargin(), title_top, w, title_top + m_line_height), DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
                    }

                    // next line
                    if (m_display_mode == DM_CATEGORIZED)
                        y += m_line_height;

                    // iterate on items
                    if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
                    {
                        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
                        {
                            if (!it2->m_is_hidden) {
                                // reset
                                it2->m_rcName.SetRectEmpty();
                                it2->m_rcValue.SetRectEmpty();

                                // is visible?
                                it2->m_rcName = CRect(0, y, w, y + m_line_height);
                                CRect rcInter = it2->m_rcName;
                                rcInter.IntersectRect(rc, rcInter);
                                if (!rcInter.IsRectEmpty()) {
                                    DrawItem(dcMem, w, sign_left + sign_size, y, it2);
                                }

                                // next line

                                if (it2->m_col == m_cols - 1) y += m_line_height;
                            }
                        }
                    }
                }
            }
        }
    }

    // Blt the changes to the screen DC.
    dc.BitBlt(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, &dcMem, 0, 0, SRCCOPY);

    // Done with off-screen bitmap and DC.
    dcMem.SelectObject(pOldBmp);
    dcMem.SelectObject(pOldFont);
    dcMem.SelectObject(pOldPen);
    dcMem.SelectObject(pOldBrush);
    bmpMem.DeleteObject();
    dcMem.DeleteDC();

    // Validate All
    ValidateRgn(NULL);
    ValidateRect(NULL);
}

CString CPropertyGrid::getSQLStr(CItem item)
{
    if (item.m_undefined) return CString("NULL");


    CString strValue = item.m_strValue;
    switch (item.m_type)
    {
    case IT_TEXT:
    {
        //        size_t j;
        //        for (;( j = strValue.find( "\r\n" )) != CString::npos;)
        //          strValue.replace( j, 2, "¶");
        break;
    }

    case IT_INTEGER:
    {
        CString strTemp;
        CString strFormat = "%d";
        if (item.m_options.size() && !item.m_options.front().IsEmpty()) strFormat = item.m_options.front();
        strTemp.Format(strFormat, item.m_nValue);
        strValue = LPCTSTR(strTemp);
        break;
    }

    case IT_DOUBLE:
    {
        CString strTemp;
        CString strFormat = "%lg";
        if (item.m_options.size() && !item.m_options.front().IsEmpty()) strFormat = item.m_options.front();
        strTemp.Format(strFormat, item.m_dValue);
        strValue = LPCTSTR(strTemp);
        break;
    }

    case IT_DATE:
    {
        CString strTemp = "";
        if (item.m_dtValue != 0) {
            strTemp = item.m_dtValue.Format("'%Y-%m-%d'");
            strTemp.Format("'%s'", dateToString(item.m_dtValue));
        }
        strValue = LPCTSTR(strTemp);
        break;
    }

    case IT_DATETIME:
    {
        CString strTemp = "";
        if (item.m_dtValue != 0) {
            if (item.m_options.size() && !item.m_options.front().IsEmpty()) strTemp = item.m_dtValue.Format(item.m_options.front());
            //        else strTemp = item.m_dtValue.Format();
            else strTemp.Format("'%s'", dateTimeToString(item.m_dtValue));
        }
        strValue = LPCTSTR(strTemp);
        break;
    }

    case IT_BOOLEAN:
    {
        strValue = item.m_bValue ? "1" : "0";
        break;
    }

    case IT_COMBO:
    {
        if (item.m_nValue >= 0 && item.m_nValue<int(item.m_options.size()))
            strValue = item.m_options[item.m_nValue];
        break;
    }

    case IT_FILE:
    case IT_FOLDER:
    {
        TCHAR szBuffer[1024];
        strncpy(szBuffer, strValue, 1024);
        strValue = szBuffer;
        break;
    }

    case IT_COLOR:
    {
        CString strTemp;
        strTemp.Format("%d; %d; %d", GetRValue(item.m_clrValue), GetGValue(item.m_clrValue), GetBValue(item.m_clrValue));
        strValue = LPCTSTR(strTemp);
        break;
    }

    case IT_FONT:
    {
        CString strTemp;
        strTemp.Format("%s; %dpt", item.m_lfValue.lfFaceName, item.m_lfValue.lfHeight);
        strValue = LPCTSTR(strTemp);
        break;
    }
    }
    if (item.m_type == IT_TEXT || item.m_type == IT_STRING) {
        CString strTemp = strValue, strTemp2;

        strTemp.Replace("\\", "\\\\");
        strTemp.Replace("'", "\\'");
        //        strTemp.Replace("\"", "\\\"");
        strTemp.Replace("\x00", "\\x00");
        strTemp.Replace("\x1a", "\\x1a");
        //        strTemp.Replace("\r", "\\r");
        //        strTemp.Replace("\n", "\\n");

        strTemp2.Format("N'%s'", strTemp);
        strValue = LPCTSTR(strTemp2);
    }





    return strValue;
}


void CPropertyGrid::DrawItem(CDC& dc, int w, int x, int y, vector<CItem>::iterator& it)
{
    // brush needed
    CBrush brushText;
    brushText.CreateSolidBrush(m_clrText);

    // pen needed
    CPen penShade(PS_SOLID, 1, m_clrShade);

    // to make sure we won't leak gdi resources
    CBrush* pOldBrush = dc.SelectObject(&brushText);
    CPen* pOldPen = dc.SelectObject(&penShade);
    CFont* pOldFont = dc.SelectObject(&m_fntNormal);

    // first shade rect
    if (m_shade_titles)
        dc.FillSolidRect(0, y, x + 2 * margin, m_line_height, m_clrShade);

    // now draw a separator line
    if (m_draw_lines)
    {
        dc.SelectObject(&penShade);
        dc.MoveTo(0, y + m_line_height);
        dc.LineTo(w + 1, y + m_line_height);
    }

    // now draw gutter
    if (m_draw_gutter)
    {
        dc.SelectObject(&penShade);
        dc.MoveTo(m_gutter_width, y);
        dc.LineTo(m_gutter_width, y + m_line_height + 1);
    }

    // needed
    int name_left = x + 2 * margin + GetTextMargin();
    int name_right = m_gutter_width - 1;
    int value_left = m_gutter_width;
    int value_right = w;

    int ddd = 0;


    // is being edited?
    if (m_focused_item == it->m_id && it->m_editable && GetEditMode(*it) != EM_CUSTOM)
    {
        value_right -= m_line_height;

        // the rect of the button
        m_rect_button = CRect(w - m_line_height, y, w, y + m_line_height);
        m_rect_button_real = CRect(w - m_line_height, y, w, y + m_line_height);

        UINT pushed = m_button_depressed ? DFCS_PUSHED : 0;


        if (it->m_is_lookup) {
            // draw a button
            dc.DrawFrameControl(m_rect_button_real, DFC_BUTTON, DFCS_BUTTONPUSH | pushed);
            dc.SelectObject(&m_fntBold);
            dc.DrawText("...", m_rect_button_real, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
            //      ddd = m_line_height;

            if (GetEditMode(*it) == EM_INPLACE) {
                m_rect_button.left = m_gutter_width;
                m_rect_button.right -= m_line_height;
            }
        }
        else if (it->m_is_fun) {
            // draw a button

//            m_rect_button_real = CRect(w - m_line_height, y, w, y + m_line_height);

            CString txtBut = "...";

            if (it->m_strFun[0] == '$') {
                txtBut = "Посмотреть";
                m_rect_button_real = CRect(m_gutter_width, y, w, y + m_line_height);
            }


            dc.DrawFrameControl(m_rect_button_real, DFC_BUTTON, DFCS_BUTTONPUSH | pushed);
            dc.SelectObject(&m_fntBold);
            dc.DrawText(txtBut, m_rect_button_real, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
            //      ddd = m_line_height;

            if (GetEditMode(*it) == EM_INPLACE) {
                m_rect_button.left = m_gutter_width;
                m_rect_button.right -= m_line_height;
            }
        }

        else {
            // now draw the button
            switch (GetEditMode(*it))
            {
            case EM_MODAL:
                // draw a button
                dc.DrawFrameControl(m_rect_button_real, DFC_BUTTON, DFCS_BUTTONPUSH | pushed);
                dc.SelectObject(&m_fntBold);
                dc.DrawText("...", m_rect_button_real, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
                break;

            case EM_DROPDOWN:
                // draw an arrow
                dc.DrawFrameControl(m_rect_button_real, DFC_SCROLL, DFCS_SCROLLDOWN | pushed);
                break;

            case EM_DROPDOWN2:
                // draw an arrow
                dc.DrawFrameControl(m_rect_button_real, DFC_SCROLL, DFCS_SCROLLDOWN | pushed);
                m_rect_button.left = m_gutter_width;
                m_rect_button.right -= m_line_height;
                break;

            case EM_INPLACE:
                // whole area is edit
                m_rect_button.left = m_gutter_width;
                break;

            default:
                assert(false);
            }
        }
    }

    // update the rects
    it->m_rcName = CRect(0, y, m_gutter_width, y + m_line_height);
    it->m_rcValue = CRect(value_left, y, value_right - ddd, y + m_line_height);
    CRect rcValue = it->m_rcValue;

    int rw = rcValue.Width();

    rcValue.left = rcValue.left + it->m_col * rw / m_cols;
    rcValue.right = rcValue.left + (it->m_col + 1) * rw / m_cols;

    rcValue.left += GetTextMargin();

    // focused
    if (m_focused_item == it->m_id)
    {
        int rect_left = name_left - 2 * margin;
        int rect_right = name_right;
        dc.FillSolidRect(rect_left, y, rect_right - rect_left + 1, m_line_height, m_clrFocus);
        dc.SetTextColor(m_clrHilite);
    }
    else
    {
        dc.SetTextColor(m_clrText);
    }

    // put label and value
    dc.SelectObject(&m_fntNormal);

    //    if (!it->m_editable) {
    //        dc.SetTextColor(m_clrDisabled);
    //    }

    if (it->m_editable) dc.SetTextColor(m_clrEditable);
    else dc.SetTextColor(m_clrDisabled);

    CString label = it->m_label;

    CItem* pItem = FindItem(it->m_id);
    if (pItem) {
        if (required(pItem)) {
            label = "*" + it->m_label;
        }
    }

    if (m_focused_item == it->m_id)
    {
        dc.SetTextColor(m_clrHilite);
    }

    dc.DrawText(label, -1, CRect(name_left, y, name_right, y + m_line_height), DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

    // get back to normal text
    if (it->m_editable) dc.SetTextColor(m_clrEditable);
    else dc.SetTextColor(m_clrDisabled);

    // custom item
    if (it->m_type == IT_CUSTOM)
    {
        int save = dc.SaveDC();
        it->m_pCustom->DrawItem(dc, it->m_rcValue, m_focused_item == it->m_id);
        dc.RestoreDC(save);
    }
    else
    {
        // modified flag
        bool modified = (it->m_strValue != it->m_strValue_old);

        // now draw text
        CString strValue = it->m_strValue;
        switch (it->m_type)
        {
        case IT_TEXT:
        {
//            size_t j;
            //            for (; (j = strValue.find("\r\n")) != CString::npos;)
            //                strValue.replace(j, 2, "¶");
            strValue.Replace("\r\n", "¶");
            break;
        }

        case IT_INTEGER:
        {
            CString strTemp;
            CString strFormat = "%d";
            if (it->m_options.size() && !it->m_options.front().IsEmpty()) strFormat = it->m_options.front();
            strTemp.Format(strFormat, it->m_nValue);
            strValue = LPCTSTR(strTemp);
            modified = (it->m_nValue != it->m_nValue_old);
            break;
        }

        case IT_DOUBLE:
        {
            CString strTemp, strTempOld;
            CString strFormat = "%lg";
            //            if (it->m_name == "Central_Meridian") {
            //                strFormat = "%.10lg";
            //            }

            if (it->m_options.size() && !it->m_options.front().IsEmpty()) strFormat = it->m_options.front();
            strTemp.Format(strFormat, it->m_dValue);
            strTempOld.Format(strFormat, it->m_dValue_old);
            strValue = LPCTSTR(strTemp);
            //        modified = (it->m_dValue != it->m_dValue_old);
            modified = (strTemp != strTempOld) && !(it->m_undefined && it->m_undefined_old);

            break;
        }

        case IT_DATE:
        {
            CString strTemp;
            if (it->m_undefined) {
                strValue = m_strUndefined;
            }
            else {
                if (it->m_dtValue != 0) {
                    if (it->m_options.size() && !it->m_options.front().IsEmpty()) strTemp = it->m_dtValue.Format(it->m_options.front());
                    else strTemp = it->m_dtValue.Format(VAR_DATEVALUEONLY);
                    strValue = LPCTSTR(strTemp);
                }
                else {
                    strValue = "";
                }

            }
            modified = (it->m_dtValue != it->m_dtValue_old) && !(it->m_undefined && it->m_undefined_old);
            break;
        }

        case IT_DATETIME:
        {
            CString strTemp = "";
            if (it->m_dtValue != 0) {
                if (it->m_options.size() && !it->m_options.front().IsEmpty()) strTemp = it->m_dtValue.Format(it->m_options.front());
                else strTemp = it->m_dtValue.Format();
            }
            strValue = LPCTSTR(strTemp);
            modified = (it->m_dtValue != it->m_dtValue_old);
            break;
        }

        case IT_BOOLEAN:
        {
            strValue = it->m_bValue ? m_strTrue : m_strFalse;
            modified = (it->m_bValue != it->m_bValue_old);
            break;
        }

        case IT_COMBO:
        {
            if (it->m_nValue >= 0 && it->m_nValue<int(it->m_options.size()))
                strValue = it->m_options[it->m_nValue];
            modified = (it->m_nValue != it->m_nValue_old);
            break;
        }

        case IT_FILE:
        case IT_FOLDER:
        {
            TCHAR szBuffer[1024];
            strncpy(szBuffer, strValue, 1024);
            //        strncpy_s(szBuffer, strValue, 1024);
            PathCompactPath(dc.GetSafeHdc(), szBuffer, rcValue.Width());
            strValue = szBuffer;
            break;
        }

        case IT_COLOR:
        {
            // draw a sample rectangle
            CRect rc = rcValue;
            rc.DeflateRect(0, 2, 0, 2);
            rc.top++;
            rc.right = rc.left + m_line_height;
            dc.FrameRect(rc, &brushText);
            rc.DeflateRect(1, 1);
            dc.FillSolidRect(rc, it->m_clrValue);
            rcValue.left = rc.right + 3 * margin;

            // update the text
            CString strTemp;
            strTemp.Format("%d; %d; %d", GetRValue(it->m_clrValue), GetGValue(it->m_clrValue), GetBValue(it->m_clrValue));
            strValue = LPCTSTR(strTemp);
            modified = (it->m_clrValue != it->m_clrValue_old);
            break;
        }

        case IT_FONT:
        {
            CString strTemp;
            strTemp.Format("%s; %dpt", it->m_lfValue.lfFaceName, -MulDiv(it->m_lfValue.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY)));
            strValue = LPCTSTR(strTemp);
            modified = (memcmp(&it->m_lfValue, &it->m_lfValue_old, sizeof(LOGFONT)) != 0);
            break;
        }
        }

        // we must also take undefined state change into account
        modified |= (it->m_undefined != it->m_undefined_old);

        it->m_modified = modified;

        // set proper font
        if (modified && m_bold_modified) dc.SelectObject(&m_fntBold);
        else if (it->m_editable && m_bold_editables) dc.SelectObject(&m_fntBold);
        else dc.SelectObject(&m_fntNormal);

        // now draw it
        if (it->m_undefined) strValue = m_strUndefined;

        if (!it->m_buttom) {
            dc.DrawText(strValue, -1, rcValue, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
        }
    }

    // clean up
    dc.SelectObject(pOldFont);
    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldBrush);
}

//
// mouse interaction
//

void CPropertyGrid::OnRButtonDown(UINT nFlags, CPoint point)
{

#if 0
    HITEM m_focused_item0;

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
        {
            for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
            {
                if (it2->m_rcName.PtInRect(point) || it2->m_rcValue.PtInRect(point))
                {
                    if (it2->m_editable || m_focus_disabled)
                    {
                        m_focused_item0 = it2->m_id;
                    }
                }
            }
        }
    }




    //    if (m_focused_item != -1 && m_focused_item0 == m_focused_item)
    if (m_focused_item != -1)
    {
        CItem* pItem = FindItem(m_focused_item);
        if (pItem) {
            AfxMessageBox(pItem->m_label, MB_OK | MB_ICONINFORMATION);
            //      return;
        }
    }
#endif

    CWnd::OnRButtonDown(nFlags, point);
}


void CPropertyGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
    // destroy edit
    SetFocus();
    DeleteEditControl();


    // click on button?
    if (m_rect_button_real.PtInRect(point))
    {
        m_button_pushed = true;
        m_button_depressed = true;
        SetCapture();
        Invalidate();
        return;
    }

    // click on button?
    if (m_focused_item != -1)
    {
        CItem* pItem = FindItem(m_focused_item);
        if (pItem && pItem->m_type == IT_CUSTOM
            && GetEditMode(*pItem) == EM_CUSTOM
            && pItem->m_pCustom->OnLButtonDown(pItem->m_rcValue, point))
        {
            m_custom_tracking = true;
            SetCapture();
            Invalidate();
            return;
        }
    }

    // resizing gutter?
    if (abs(point.x - m_gutter_width) < 3)
    {
        ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
        m_resizing_gutter = true;
        m_ptLast = point;
        SetCapture();
        Invalidate();
        return;
    }

    // disable focus
    m_focused_item = -1;
    onChangeFocus(m_focused_item);

    m_focused_section = -1;
    m_rect_button.SetRectEmpty();
    m_rect_button_real.SetRectEmpty();

    // did we click on a section
    if (m_display_mode == DM_CATEGORIZED)
    {
        for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
        {
            bool is_hidden = isHidden(it->m_items);
            if (!is_hidden) {

                if (it->m_rcSign.PtInRect(point))
                {
                    if (it->m_collapsed) {
                        int number = it->m_number;
                        for (vector<CSection>::iterator it2 = m_sections.begin(); it2 != m_sections.end(); ++it2) {
                            if (it2->m_number != number && it2->m_number > 0) {
                                if (!it2->m_collapsed) {
                                    UpdateWindow();
                                    // Нужно обновить modified;
                                    int qq;
                                    qq = 1;
                                }

                                it2->m_collapsed = true;
                            }
                        }
                    }

                    if (it->m_collapsed) {
                        if (!check_open(&*it, true)) return;
                    }


                    it->m_collapsed = !it->m_collapsed;
                    m_focused_section = it->m_id;

                    RecalcLayout();
                    return;
                }
                else if (it->m_rcTitle.PtInRect(point))
                {
                    m_focused_section = it->m_id;
                    Invalidate();
                    return;
                }
            }
        }
    }

    // focus
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
        {
            for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
            {
                if (it2->m_rcName.PtInRect(point) || it2->m_rcValue.PtInRect(point))
                {
                    if (it2->m_editable || m_focus_disabled)
                    {
                        m_focused_item = it2->m_id;
                        onChangeFocus(m_focused_item);

                        if (it2->m_rcValue.PtInRect(point))
                            m_value_clicked = (GetEditMode(*it2) == EM_INPLACE || GetEditMode(*it2) == EM_DROPDOWN || GetEditMode(*it2) == EM_DROPDOWN2);
                        Invalidate();
                        return;
                    }
                }
            }
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
    Invalidate();
}

bool CPropertyGrid::isModified()
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        vector<CItem>::iterator it2 = it->m_items.begin();
        for (; it2 != it->m_items.end(); it2++)
        {
            if (it2->m_modified) {
                return true;
            }
        }
    }
    return false;
}


void CPropertyGrid::itemChanged(CItem* pItem)
{
    CWnd* wnd = GetParent();

    if (!wnd) return;

    pItem->m_modified = true;


    CString table = pItem->m_table;

    if (table == "heatPipeSections" && (pItem->m_name == "firstPICdateHP" || pItem->m_name == "PICdateCapital")) {
        SetNameValue("lastTransDate", pItem->m_dtValue);
    }
    else if (table == "heatSources") {
        if (pItem->m_name == "t1_r" || pItem->m_name == "t2_r" || pItem->m_name == "t3_r") {
            double t1, t2, t3;

            GetNameValue("t1_r", t1);
            GetNameValue("t2_r", t2);
            GetNameValue("t3_r", t3);

            if (t3 != t2) {
                double t = (t1 - t3) / (t3 - t2);
                SetNameValue("uf", t);
            }
        }
    }
    else if (table.CompareNoCase("tehnicheskie_usloviya") == 0) {
        change_value_tu(pItem->m_id);
    }

    //    if (pItem->m_is_after && pItem->m_modified_for_after) {
    if (pItem->m_is_after) {
        prop_after(pItem->m_strAfter, pItem->m_id);
        pItem->m_modified_for_after = false;
    }

    if (pItem->m_is_fun_disable) {
        fun_disable(pItem);
    }

    wnd->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
}


void CPropertyGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (m_focused_item != -1)
    {
        CItem* pItem = FindItem(m_focused_item);
        if (pItem)
        {
            if (pItem->m_type == IT_BOOLEAN)
            {
                if (!pItem->m_undefined)
                {
                    pItem->m_bValue = !pItem->m_bValue;
                    pItem->m_modified = true;
                    itemChanged(pItem);
                    Invalidate();
                }
            }
            else if (pItem->m_type == IT_COMBO)
            {
                if (!pItem->m_undefined)
                {
                    pItem->m_nValue = (pItem->m_nValue + 1) % int(pItem->m_options.size());
                    pItem->m_modified = true;
                    itemChanged(pItem);
                    Invalidate();
                }
            }
            else if (GetEditMode(*pItem) == EM_MODAL)
            {
                EditFocusedItem();
            }
        }
    }
    else if (m_focused_section != -1)
    {
        CSection* pSection = FindSection(m_focused_section);
        if (pSection)
        {
            if (!pSection->m_collapsed || check_open(pSection, true)) {
                pSection->m_collapsed = !pSection->m_collapsed;
            }

            Invalidate();
        }
    }

    CWnd::OnLButtonDblClk(nFlags, point);
}

void CPropertyGrid::OnMouseMove(UINT nHitTest, CPoint point)
{
    if (m_custom_tracking)
    {
        CItem* pItem = FindItem(m_focused_item);
        if (pItem)
        {
            pItem->m_pCustom->OnMouseMove(pItem->m_rcValue, point);
            Invalidate();
        }
    }
    else if (m_button_pushed)
    {
        m_button_depressed = m_rect_button_real.PtInRect(point) ? true : false;
        Invalidate();
    }
    else if (m_resizing_gutter)
    {
        ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
        m_gutter_width += point.x - m_ptLast.x;
        CRect rc;
        GetClientRect(&rc);
        if (m_gutter_width < rc.Width() / 5) m_gutter_width = rc.Width() / 5;
        if (m_gutter_width > 4 * rc.Width() / 5) m_gutter_width = 4 * rc.Width() / 5;
        m_ptLast = point;
        Invalidate();
    }
    else if (!m_control)
    {
        if (abs(point.x - m_gutter_width) < 3) ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
        else ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    }

    CWnd::OnMouseMove(nHitTest, point);
}

//#include "VirtualDlg.h"

void CPropertyGrid::set_disable(CItem* pItem, int id, bool yes_no)
{
    map<int, list<CString> >::const_iterator it1 = pItem->m_map_disable.find(id);
    if (it1 != pItem->m_map_disable.end()) {
        CString ss = "";
        for (auto it2 : it1->second) {
            CItem* item = FindItemName(CString(it2));

            if (!item) {
                item = FindItemFunction(CString(it2));
            }

            if (item) {
                //                 if (item->m_editable) {
                item->m_editable = yes_no;
                //                 }
            }
        }
    }
}

void CPropertyGrid::fun_disable(CItem* pItem)
{
    for (auto it : pItem->m_map_disable) {
        set_disable(pItem, it.first, false);
    }

    set_disable(pItem, pItem->m_is_lookup ? pItem->m_index : pItem->m_nValue, true);
}

void CPropertyGrid::init_disable()
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); it2++) {
            CItem* pItem = &*it2;
            if (pItem->m_is_fun_disable) {
                for (auto it : pItem->m_map_disable) {
                    set_disable(pItem, it.first, false);
                }
            }
        }
    }

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); it2++) {
            CItem* pItem = &*it2;
            if (pItem->m_is_fun_disable) {
                set_disable(pItem, pItem->m_is_lookup ? pItem->m_index : pItem->m_nValue, true);
            }
        }
    }
}


void CPropertyGrid::viewLookup(CItem* pItem)
{
    long id = pItem->m_index;
    CString txt;

    if (viewVirtualDlg(this, pItem->m_schema, pItem->m_strLookup, id, pItem->m_label, txt)) {
        pItem->m_strValue = txt;
        pItem->m_index = id;

        pItem->m_undefined = pItem->m_index == -1;
        pItem->m_modified = true;
        itemChanged(pItem);
    }
}




void CPropertyGrid::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_custom_tracking)
    {
        m_custom_tracking = false;
        ReleaseCapture();
        Invalidate();
        CItem* pItem = FindItem(m_focused_item);
        if (pItem)
            pItem->m_pCustom->OnLButtonUp(pItem->m_rcValue, point);
    }
    else if (m_button_pushed || m_value_clicked)
    {
        m_button_pushed = false;
        m_button_depressed = false;
        ReleaseCapture();
        Invalidate();

        if (m_rect_button_real.PtInRect(point) || (m_value_clicked && m_focused_item != -1 && FindItem(m_focused_item) && FindItem(m_focused_item)->m_rcValue.PtInRect(point)))
        {
            m_value_clicked = false;
            CItem* pItem = FindItem(m_focused_item);
            if (pItem)
            {
                if ((pItem->m_is_lookup && !pItem->m_is_fun) && m_rect_button_real.PtInRect(point) && pItem->m_editable) {
                    viewLookup(pItem);

                    if (pItem->m_index != -1) {
                        if (pItem->m_is_fun) {
                            int ret = prop_fun(pItem->m_strFun, pItem->m_id);
                            //                  AfxMessageBox(pItem->m_strFun, MB_OK|MB_ICONINFORMATION);
                        }
                        if (pItem->m_is_fun_disable) {
                            fun_disable(pItem);
                        }
                    }
                    Invalidate();

                }
                else if ((pItem->m_is_fun) && m_rect_button_real.PtInRect(point) && pItem->m_editable) {
                    CString txt;

                    int ret = prop_fun(pItem->m_strFun, pItem->m_id);

                    if (ret != 0) {
                        itemChanged(pItem);
                        Invalidate();
                    }
                    if (ret == 2) {
                        CDialog* wnd = (CDialog*)GetParent();

                        if (wnd) {
                            wnd->SendMessage(WM_COMMAND, 1);
                            return;
                        }
                    }
                }
                else if ((GetEditMode(*pItem) == EM_DROPDOWN || GetEditMode(*pItem) == EM_DROPDOWN2) && (m_rect_button_real.PtInRect(point) || pItem->m_type == IT_BOOLEAN))
                {
                    if (pItem->m_type == IT_CUSTOM)
                    {
                        CRect rc = m_rect_button_real;
                        rc.left = m_gutter_width;
                        pItem->m_pCustom->ShowDropDown(rc);
                    }
                    else if (pItem->m_type == IT_DATE) {
                        if (isCanMod(pItem->m_id, "1")) {
                            // the calendar rect
                            CRect rc = m_rect_button_real;
                            rc.left = m_gutter_width;
                            rc.top += m_line_height;
                            rc.bottom = rc.top + 100;
                            ClientToScreen(&rc);

                            // create it
                            m_control = new CPropertyGridMonthCalCtrl;
                            CPropertyGridMonthCalCtrl* mc = (CPropertyGridMonthCalCtrl*)m_control;
                            mc->CreateEx(0, MONTHCAL_CLASS, NULL, WS_POPUP | WS_BORDER, rc, GetParent(), 0);
                            if (pItem->m_dtValue.GetStatus() == 0 && pItem->m_dtValue != 0) {
                                mc->SetCurSel(pItem->m_dtValue);
                            }

                            if (pItem->m_strValidate.Find("CurrentSeason")) {
                                COleDateTime dt1, dt2;
                                int y1, y2;
                                get_season(y1, y2, dt1, dt2);
                                if (y1 > 0) {
                                    mc->SetRange(&dt1, &dt2);
                                }
                            }

                            if (pItem->m_strValidate.Find("CurrentAndNextSeason")) {
                                COleDateTime dt1, dt2;
                                int y1, y2;
                                get_season(y1, y2, dt1, dt2);
                                if (y1 > 0) {
                                    mc->SetRange(&dt1, 0);
                                }
                            }


                            mc->SetOwner(this);
                            mc->SizeMinReq();

                            // now position it
                            CRect rc2;
                            mc->GetWindowRect(&rc2);
                            rc2.OffsetRect(rc.right - rc2.right, 0);
                            mc->SetWindowPos(NULL, rc2.left, rc2.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
                        }
                    }
                    else
                    {

                        long id = pItem->m_bValue ? 0 : 1;
                        if (pItem->m_undefined) pItem->m_index = -1;

                        CString txt;

                        if (viewVirtualDlg(this, getPsAdoName(), "SELECT id, name FROM YesOrNo ORDER BY ord", id, "", txt)) {
                            pItem->m_strValue = txt;
                            pItem->m_index = id;

                            pItem->m_bValue = (id != 0);
                            pItem->m_undefined = (pItem->m_index == -1);
                            pItem->m_modified = true;
                            itemChanged(pItem);
                            Invalidate();
                        }
#if 0
                        // the combo rect
                        CRect rc = m_rect_button_real;
                        rc.left = m_gutter_width;
                        rc.top += m_line_height;
                        rc.bottom = rc.top + 100;

                        // create it
                        m_control = new CPropertyGridCombo();
                        CPropertyGridCombo* pCombo = (CPropertyGridCombo*)m_control;
                        pCombo->Create(WS_CHILD, rc, this, 0);
                        pCombo->SetColors(m_clrBack, m_clrText, m_clrFocus, m_clrHilite);
                        pCombo->SetFont(&m_fntNormal);

                        if (pItem->m_type == IT_BOOLEAN)
                        {
                            pCombo->AddString(m_strTrue);
                            pCombo->AddString(m_strFalse);
                            if (!pItem->m_undefined)
                                pCombo->SetCurSel(pItem->m_bValue ? 0 : 1);
                        }
                        else
                        {
                            for (vector<CString>::iterator it = pItem->m_options.begin(); it != pItem->m_options.end(); ++it)
                                pCombo->AddString(*it);
                            if (!pItem->m_undefined)
                                pCombo->SetCurSel(pItem->m_nValue);
                        }
                        pCombo->ShowWindow(SW_SHOW);
#endif
                    }
                }
                else if ((GetEditMode(*pItem) == EM_INPLACE || GetEditMode(*pItem) == EM_DROPDOWN2))
                {
                    if (pItem->m_editable && !pItem->m_noedit) {
                        // the in-place edit rect
                        CRect rc = m_rect_button;
                        rc.left++;
                        rc.top += margin;

                        // the value
                        CString strValue;
                        if (pItem->m_type == IT_STRING)
                        {
                            strValue = pItem->m_strValue;
                        }
                        else if (pItem->m_type == IT_INTEGER)
                        {
                            if (!pItem->m_undefined)
                            {
                                CString strTemp;
                                strTemp.Format("%d", pItem->m_nValue);
                                strValue = LPCTSTR(strTemp);
                            }
                        }
                        else if (pItem->m_type == IT_DOUBLE)
                        {
                            if (!pItem->m_undefined)
                            {
                                CString strTemp = "%lg";
                                if (pItem->m_options.size() && !pItem->m_options.front().IsEmpty()) strTemp = pItem->m_options.front();
                                strTemp.Format(strTemp, pItem->m_dValue);
                                strValue = LPCTSTR(strTemp);
                            }
                        }
                        else if (pItem->m_type == IT_DATE)
                        {
                            if (!pItem->m_undefined)
                            {
                                CString strTemp = "";

                                if (pItem->m_dtValue != 0) {
                                    if (pItem->m_options.size() && !pItem->m_options.front().IsEmpty()) strTemp = pItem->m_dtValue.Format(pItem->m_options.front());
                                    else strTemp = pItem->m_dtValue.Format(VAR_DATEVALUEONLY);
                                }
                                strValue = LPCTSTR(strTemp);
                            }
                        }
                        else if (pItem->m_type == IT_CUSTOM)
                        {
                            strValue = pItem->m_pCustom->GetStringForInPlaceEdit();
                        }
                        else
                        {
                            assert(false);
                        }

                        // create it
                        m_control = new CPropertyGridInPlaceEdit(this, rc, WS_CHILD, 1000, strValue);

                        if (pItem->m_is_fun) {

                            CString fun = pItem->m_strFun;

                            if (fun.Find("File ") == 0) {
                                ((CPropertyGridInPlaceEdit*)m_control)->setDoc(fun);
                            }
                        }

                        CPropertyGridInPlaceEdit* pEdit = (CPropertyGridInPlaceEdit*)m_control;
                        pEdit->SetColors(m_clrBack, m_clrText);
                        pEdit->SetFont(&m_fntNormal);
                        if (pItem->m_length > 0) pEdit->SetLimitText(pItem->m_length);
                        pEdit->ShowWindow(SW_SHOW);
                    }
                }
                else if (GetEditMode(*pItem) == EM_MODAL)
                {
                    EditFocusedItem();
                }
                else if (GetEditMode(*pItem) == EM_CUSTOM)
                {
                    pItem->m_pCustom->OnLButtonUp(pItem->m_rcValue, point);
                }
                else if (pItem->m_type == IT_BOOLEAN)
                {
                }
                else
                {
                    assert(false);
                }
            }
        }
    }
    else if (m_resizing_gutter)
    {
        ReleaseCapture();
        m_resizing_gutter = false;
        ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    }

    CWnd::OnLButtonUp(nFlags, point);
}

//
// keyboard interaction
//

UINT CPropertyGrid::OnGetDlgCode()
{
    return CWnd::OnGetDlgCode() | DLGC_WANTCHARS | DLGC_WANTARROWS;
}

void CPropertyGrid::MoveForward(HSECTION& focused_section, HITEM& focused_item)
{
    for (int pass = 0; pass < 2; pass++)
    {
        bool found = false;

        bool stop_on_next_valid = false;
        if (focused_section == -1 && focused_item == -1)
            stop_on_next_valid = true;

        for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
        {
            if (m_display_mode == DM_CATEGORIZED)
            {
                if (it->m_id == focused_section)
                {
                    stop_on_next_valid = true;
                }
                else if (stop_on_next_valid)
                {
                    focused_section = it->m_id;
                    focused_item = -1;
                    found = true;
                    break;
                }
            }

            if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
            {
                for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
                {
                    if (it2->m_id == focused_item)
                    {
                        stop_on_next_valid = true;
                    }
                    else if (stop_on_next_valid)
                    {
                        if (it2->m_editable || m_focus_disabled)
                        {
                            focused_section = -1;
                            focused_item = it2->m_id;
                            found = true;
                            break;
                        }
                    }
                }

                if (found)
                    break;
            }
        }

        if (found)
            break;

        focused_section = -1;
        focused_item = -1;
    }
}

void CPropertyGrid::FocusNextItem()
{
    // simple move forward
    MoveForward(m_focused_section, m_focused_item);

    // ensure visible
    CRect rc(0, 0, 0, 0);
    if (m_focused_section != -1 && FindSection(m_focused_section)) rc = FindSection(m_focused_section)->m_rcTitle;
    else if (m_focused_item != -1 && FindItem(m_focused_item)) rc = FindItem(m_focused_item)->m_rcName;
    if (!rc.IsRectEmpty())
    {
        CRect rect;
        GetClientRect(&rect);
        rect.IntersectRect(rc, rect);
        if (rect.Height() != m_line_height)
            OnVScroll(SB_THUMBPOSITION, rc.bottom, &m_scrollbar);
        //    OnVScroll(SB_THUMBPOSITION, rc.top, &m_scrollbar);
    }

    onChangeFocus(m_focused_item);

    // done
    Invalidate();
}

HSECTION CPropertyGrid::getSection(HITEM item)
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        vector<CItem>::iterator it2 = find(it->m_items.begin(), it->m_items.end(), item);
        if (it2 != it->m_items.end())
        {
            if (it2->m_id == item) {
                return it->m_id;
            }
        }
    }

    return -1;
}


void CPropertyGrid::FocusItem(HITEM item)
{
    //        CRect rect;
    //        GetClientRect(&rect);
    //        rect.IntersectRect(rc, rect);
    //        if (rect.Height() != m_line_height)
    //            OnVScroll(SB_THUMBPOSITION, 0, &m_scrollbar);

    m_focused_section = getSection(item);
    m_focused_item = item;

    CSection* pSection = FindSection(m_focused_section);
    if (pSection) {
        if (pSection->m_collapsed) {
            ExpandSection(m_focused_section, true);
        }
    }

    int l = 0;


    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        bool is_hidden = isHidden(it->m_items);
        if (!is_hidden) l++;

        if (!IsSectionCollapsed(it->m_id)) {
            vector<CItem>::iterator it2 = find(it->m_items.begin(), it->m_items.end(), item);
            for (it2 = it->m_items.begin(); it2 != it->m_items.end(); it2++)
            {
                if (it2->m_id == item) {
                    goto QUIT;
                }
                if (!it2->m_is_hidden) l++;
            }
        }
    }
QUIT:
    OnVScroll(SB_THUMBPOSITION, l * m_line_height, &m_scrollbar);

    onChangeFocus(m_focused_item);
    Invalidate();
}

void CPropertyGrid::FocusName(CString name)
{
    CItem* item = FindItemName(name);

    if (item) {
        FocusItem(item->m_id);
    }
}


void CPropertyGrid::onChangeFocus(int m_focused_item)
{
    CItem* pItem = FindItem(m_focused_item);
    if (pItem) {
        CString s = pItem->m_help;
        if (s == "") {
            s = pItem->m_label;
        }

        GetOwner()->SendMessage(WM_PG_FOCUSCHANGED, m_focused_item, (LPARAM)(const char*)s);
    }
}


void CPropertyGrid::FocusPrevItem()
{
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        if (m_display_mode == DM_CATEGORIZED)
        {
            HSECTION focused_section = it->m_id;
            HITEM focused_item = -1;
            MoveForward(focused_section, focused_item);
            if (focused_section == m_focused_section && focused_item == m_focused_item)
            {
                m_focused_section = it->m_id;
                m_focused_item = -1;
                onChangeFocus(m_focused_item);
                break;
            }
        }

        if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
        {
            bool found = false;
            for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
            {
                if (!it2->m_editable && !m_focus_disabled)
                    continue;

                HSECTION focused_section = -1;
                HITEM focused_item = it2->m_id;
                MoveForward(focused_section, focused_item);
                if (focused_section == m_focused_section && focused_item == m_focused_item)
                {
                    m_focused_section = -1;
                    m_focused_item = it2->m_id;
                    onChangeFocus(m_focused_item);
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }
    }

    // ensure visible
    CRect rc(0, 0, 0, 0);
    if (m_focused_section != -1 && FindSection(m_focused_section)) rc = FindSection(m_focused_section)->m_rcTitle;
    else if (m_focused_item != -1 && FindItem(m_focused_item)) rc = FindItem(m_focused_item)->m_rcName;
    if (!rc.IsRectEmpty())
    {
        CRect rect;
        GetClientRect(&rect);
        rect.IntersectRect(rc, rect);
        if (rect.Height() != m_line_height)
            OnVScroll(SB_THUMBPOSITION, rc.top, &m_scrollbar);
    }

    // done
    Invalidate();
}

void CPropertyGrid::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == '*')
    {
        ExpandAll(true);
    }
    else if (nChar == '/')
    {
        ExpandAll(false);
    }
    else if (nChar == '+' || nChar == '-')
    {
        if (m_focused_section != -1)
        {
            CSection* pSection = FindSection(m_focused_section);
            if (pSection) pSection->m_collapsed = (nChar == '-');
            RecalcLayout();
        }
    }

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CPropertyGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_DOWN)
    {
        FocusNextItem();
    }
    else if (nChar == VK_UP)
    {
        FocusPrevItem();
    }
    else if (nChar == VK_LEFT)
    {
        if (m_focused_section != -1 && FindSection(m_focused_section) && FindSection(m_focused_section)->m_collapsed == false)
        {
            ExpandSection(m_focused_section, false);
        }
        else
        {
            FocusPrevItem();
        }
    }
    else if (nChar == VK_RIGHT)
    {
        if (m_focused_section != -1 && FindSection(m_focused_section) && FindSection(m_focused_section)->m_collapsed == true)
        {
            ExpandSection(m_focused_section, true);
        }
        else
        {
            FocusNextItem();
        }
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPropertyGrid::RecalcOff()
{
    m_recalc_on = false;
}


void CPropertyGrid::RecalcOn()
{
    m_recalc_on = true;

    RecalcLayout();

}


//
// scrolling
//

void CPropertyGrid::RecalcLayout()
{
    if (!m_recalc_on) return;

    // save current scroll offset
    int offset = GetScrollOffset();

    // total height
    int height = 0;
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        if (m_display_mode == DM_CATEGORIZED) {
            height += m_line_height;
        }
        if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED) {
            height += int(it->m_items.size() / m_cols) * m_line_height;
        }
    }

    // client rect
    CRect rc;
    GetClientRect(&rc);
    if (height < rc.Height())
    {
        if (m_scrollbar.GetSafeHwnd() != NULL)
        {
            m_scrollbar.EnableScrollBar(ESB_DISABLE_BOTH);
            m_scrollbar.ShowScrollBar(FALSE);
        }
        m_scroll_enabled = false;
    }
    else
    {
        if (m_scrollbar.GetSafeHwnd() == NULL)
        {
            CRect rect = rc;
            rect.left = rect.right - GetSystemMetrics(SM_CXVSCROLL);
            m_scrollbar.Create(WS_CHILD | SBS_VERT, rect, this, 1000);
        }

        m_scrollbar.EnableScrollBar(ESB_ENABLE_BOTH);

        SCROLLINFO info;
        info.cbSize = sizeof(SCROLLINFO);
        info.fMask = SIF_ALL;
        info.nMin = 0;
        info.nMax = height;
        info.nPage = rc.Height();
        info.nPos = min(offset, height);
        info.nTrackPos = 2;
        m_scrollbar.SetScrollInfo(&info);

        m_scrollbar.ShowScrollBar();
        m_scroll_enabled = true;
    }

    if (GetSafeHwnd())
        Invalidate();
}

int CPropertyGrid::GetScrollOffset()
{
    if (m_scrollbar && m_scrollbar.IsWindowEnabled() == TRUE)
        return m_scrollbar.GetScrollPos();
    return 0;
}

void CPropertyGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // check
    if (!m_scroll_enabled) return;
    if (pScrollBar != &m_scrollbar) return;
    if (nSBCode == SB_ENDSCROLL) return;

    // set focus to us
    SetFocus();

    // get the scroll info
    SCROLLINFO info;
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    m_scrollbar.GetScrollInfo(&info);
    int min = info.nMin;
    int pos = info.nPos;
    int max = info.nMax - info.nPage;

    // the entire rect
    CRect rect;
    GetClientRect(&rect);
    int h = rect.Height();

    // the rect without the scrollbar
    CRect rc(0, 0, rect.right - GetSystemMetrics(SM_CXVSCROLL), rect.bottom);

    switch (nSBCode)
    {
    case SB_TOP:
        pScrollBar->SetScrollPos(min);
        break;

    case SB_BOTTOM:
        pScrollBar->SetScrollPos(max);
        break;

    case SB_LINEDOWN:
        if (pos + m_line_height >= max) pScrollBar->SetScrollPos(max);
        else pScrollBar->SetScrollPos(pos + m_line_height);
        break;

    case SB_LINEUP:
        if (pos - m_line_height <= min) pScrollBar->SetScrollPos(min);
        else pScrollBar->SetScrollPos(pos - m_line_height);
        break;

    case SB_PAGEDOWN:
        if (pos + h >= max) pScrollBar->SetScrollPos(max);
        else pScrollBar->SetScrollPos(pos + h);
        break;

    case SB_PAGEUP:
        if (pos - h <= min) pScrollBar->SetScrollPos(min);
        else pScrollBar->SetScrollPos(pos - h);
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        int diff = nPos - pos;
        if (diff == 0) return;
        if (pos <= min && diff < 0) return;
        if (pos >= max && diff > 0) return;
        pScrollBar->SetScrollPos(nPos);
    }

    Invalidate();

    CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

}

BOOL CPropertyGrid::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    int steps = abs(zDelta) / WHEEL_DELTA;
    for (int i = 0; i < 3 * steps; i++)
    {
        if (zDelta > 0) OnVScroll(SB_LINEUP, 0, &m_scrollbar);
        if (zDelta < 0) OnVScroll(SB_LINEDOWN, 0, &m_scrollbar);
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

//
// editing
//

CPropertyGrid::EEditMode CPropertyGrid::GetEditMode(CItem& item)
{
    if (item.m_is_lookup) return EM_MODAL;
    //    if (item.m_is_fun) return EM_INPLACE;

    switch (item.m_type)
    {
    case IT_CUSTOM:
        return item.m_pCustom->GetEditMode();

    case IT_STRING:
    case IT_INTEGER:
    case IT_DOUBLE:
        return EM_INPLACE;

    case IT_COMBO:
    case IT_BOOLEAN:
        return EM_DROPDOWN;
    case IT_DATE:
        return EM_DROPDOWN2;

    case IT_TEXT:
    case IT_DATETIME:
    case IT_FILE:
    case IT_FOLDER:
    case IT_COLOR:
    case IT_FONT:
        return EM_MODAL;

    default:
        assert(false);
        return EM_CUSTOM;
    }
}

void CPropertyGrid::DeleteEditControl()
{
    // destroy edit
    if (m_control)
    {
        if (m_control->GetSafeHwnd())
            m_control->DestroyWindow();
        delete m_control;
        m_control = NULL;
    }
}

LRESULT CPropertyGrid::OnComboSelChanged(WPARAM wParam, LPARAM lParam)
{
    CItem* pItem = FindItem(m_focused_item);
    if (pItem)
    {
        if (pItem->m_type == IT_BOOLEAN)
        {
            pItem->m_bValue = (wParam == 0);
            pItem->m_undefined = false;
            pItem->m_modified = true;
            itemChanged(pItem);
            DeleteEditControl();
            Invalidate();
        }
        else if (pItem->m_type == IT_COMBO)
        {
            pItem->m_nValue = int(wParam);
            pItem->m_undefined = false;
            pItem->m_modified = true;
            itemChanged(pItem);
            DeleteEditControl();
            Invalidate();
        }
        else
        {
            assert(false);
        }
    }
    return 0;
}

LRESULT CPropertyGrid::OnEditChanged(WPARAM wParam, LPARAM lParam)
{
    CItem* pItem = FindItem(m_focused_item);

    CString strValue = CString((char*)wParam);

    if (!isCanMod(m_focused_item, strValue)) return 0;

    CString msg, msg_notnull, msg_sezon;
    int moveID;

    if (strValue != "" && !check_column(pItem, strValue, msg, msg_notnull, msg_sezon, moveID)) {
        if (msg != "") {
            FocusItem(moveID);
            AfxMessageBox(msg);
            return 0;
        }
    }

    if (pItem)
    {
        if (pItem->m_type == IT_STRING)
        {
            pItem->m_strValue = strValue;
            pItem->m_undefined = false;
            pItem->m_modified = true;
            itemChanged(pItem);
            DeleteEditControl();
            Invalidate();
        }
        else if (pItem->m_type == IT_INTEGER)
        {
            if (strlen((char*)wParam))
            {
                pItem->m_nValue = atoi((char*)wParam);
                pItem->m_undefined = false;
                pItem->m_modified = true;
                itemChanged(pItem);
            }
            else {
                pItem->m_undefined = true;
                pItem->m_modified = true;
                itemChanged(pItem);
            }
            DeleteEditControl();
            Invalidate();
        }
        else if (pItem->m_type == IT_DOUBLE)
        {
            if (strlen((char*)wParam))
            {
                CString ss = (char*)wParam;
                ss.Replace(",", ".");
                int n = sscanf(ss, "%lg", &pItem->m_dValue);
                if (n == 1) {
                    pItem->m_undefined = false;
                }
                else {
                    pItem->m_undefined = true;
                }

                pItem->m_modified = true;
                itemChanged(pItem);
            }
            else {
                pItem->m_undefined = true;
                pItem->m_modified = true;
                itemChanged(pItem);
            }
            DeleteEditControl();
            Invalidate();
        }
        else if (pItem->m_type == IT_CUSTOM)
        {
            if (pItem->m_pCustom->OnItemEdited(CString((char*)wParam))) {
                pItem->m_modified = true;
                itemChanged(pItem);
            }
            DeleteEditControl();
            Invalidate();
        }
        else if (pItem->m_type == IT_DATE)
        {
            if (strlen((char*)wParam))
            {
                CString ss = (char*)wParam;
                int d, m, y;
                int n = sscanf(ss, "%d.%d.%d", &d, &m, &y);
                if (n == 3) {
                    pItem->m_undefined = false;
                }
                else {
                    pItem->m_undefined = true;
                }

                pItem->m_dtValue = COleDateTime(y, m, d, 0, 0, 0);
                pItem->m_modified = true;

                itemChanged(pItem);
            }
            else {
                bool old_undefined = pItem->m_undefined;

                pItem->m_undefined = true;
                if (old_undefined != pItem->m_undefined) itemChanged(pItem);
            }
            DeleteEditControl();
            Invalidate();
        }
        else
        {
            assert(false);
        }
    }
    return 0;
}

LRESULT CPropertyGrid::OnDateChanged(WPARAM wParam, LPARAM lParam)
{
    CItem* pItem = FindItem(m_focused_item);

    if (!isCanMod(m_focused_item, "1")) return 0;


    if (pItem)
    {
        if (pItem->m_type == IT_DATE)
        {
            CPropertyGridMonthCalCtrl* mc = (CPropertyGridMonthCalCtrl*)m_control;
            mc->GetCurSel(pItem->m_dtValue);

//            CString strValue = dateToString(pItem->m_dtValue);
            CString strValue = pItem->m_dtValue.Format("%d.%m.%Y");

                
            CString msg, msg_notnull, msg_sezon;
            int moveID;

            if (strValue != "" && !check_column(pItem, strValue, msg, msg_notnull, msg_sezon, moveID)) {
                if (msg != "") {
                    FocusItem(moveID);
                    AfxMessageBox(msg);
                    return 0;
                }
            }



            pItem->m_undefined = false;
            itemChanged(pItem);
            DeleteEditControl();
            Invalidate();

        }
        else
        {
            assert(false);
        }
    }
    return 0;
}

bool viewFile(CWnd* wnd, CString ext, CString filter, CString path, CString strValue, CString& new_value);
CString TrimPath(CString val, CString path);


LRESULT CPropertyGrid::OnMessage(WPARAM wParam, LPARAM lParam)
{
    CItem* pItem = FindItem(m_focused_item);
    if (pItem)
    {

        bool funFileInfo(CString fun, CString & ext, CString & filter, CString & path);
        CString ext, filter, path;

        CString fun = pItem->m_strFun;


        if (funFileInfo(fun, ext, filter, path)) {
            CString path0 = path;

            //             CString p0 = GetPath(pItem->m_strValue);

            //             if (p0 != "") path0 = p0;


            CString strValue;
            GetItemValue(pItem->m_id, strValue);
            //              if (IsFile(path+strValue)) {
            path0 = GetPath(path + strValue) + "\\";
            path0 = strValue;
            //              }

            CFileDialog fd(TRUE, ext, path0, OFN_HIDEREADONLY, filter, this);

            if (fd.DoModal() == IDOK) {
                CString new_value = TrimPath(fd.GetPathName(), path);
                SetItemValue(pItem->m_id, CString(new_value));
            }
        }
    }
    return 0;
}




void CPropertyGrid::EditFocusedItem()
{
    CItem* pItem = FindItem(m_focused_item);
    if (pItem)
    {
        if (!pItem->m_editable && pItem->m_type != IT_TEXT)
            return;

        if (pItem->m_type == IT_FILE)
        {
            CFileDialog dlg(TRUE, NULL, pItem->m_strValue, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, pItem->m_options.front(), GetParent());
            if (dlg.DoModal() == IDOK)
            {
                pItem->m_strValue = dlg.GetPathName();
                pItem->m_undefined = false;
                itemChanged(pItem);
                Invalidate();
            }
        }
        else if (pItem->m_type == IT_FOLDER)
        {
            CPropertyGridDirectoryPicker::m_strTitle = pItem->m_options.front();
            if (CPropertyGridDirectoryPicker::PickDirectory(pItem->m_strValue, GetParent()->GetSafeHwnd()))
            {
                pItem->m_undefined = false;
                itemChanged(pItem);
                Invalidate();
            }
        }
        else if (pItem->m_type == IT_TEXT)
        {
            CDynDialogEx dlg(GetParent());
            dlg.SetUseSystemButtons(FALSE);
            dlg.SetWindowTitle(pItem->m_label);
            dlg.SetFont(&m_fntNormal);
            CString strValue = pItem->m_strValue;

            //      dlg.AddDlgControl("EDIT", pItem->m_strValue, STYLE_EDIT|WS_VSCROLL|WS_HSCROLL|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE|ES_WANTRETURN, EXSTYLE_EDIT, CRect(7, 7, 200, 100), (void*) &strValue);
            dlg.AddDlgControl("EDIT", pItem->m_strValue, STYLE_EDIT | WS_VSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_WANTRETURN, EXSTYLE_EDIT, CRect(7, 7, 200, 100), (void*)&strValue, IDC_EDIT);

            /*
                        dlg.AddDlgControl("BUTTON", m_strOk, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(56, 106, 106, 120), NULL, IDOK);
                        dlg.AddDlgControl("BUTTON", m_strCancel, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(110, 106, 160, 120), NULL, IDCANCEL);
                        dlg.AddDlgControl("BUTTON", "Сохранить", STYLE_BUTTON, EXSTYLE_BUTTON, CRect(164, 106, 210, 120), NULL, IDC_SAVE);
            */

            /*
                        dlg.AddDlgControl("BUTTON", m_strOk, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(56, 106, 76, 120), NULL, IDOK);
                        dlg.AddDlgControl("BUTTON", m_strCancel, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(80, 106, 100, 120), NULL, IDCANCEL);
                        dlg.AddDlgControl("BUTTON", "Сохранить", STYLE_BUTTON, EXSTYLE_BUTTON, CRect(104, 106, 124, 120), NULL, IDC_SAVE);

                        dlg.AddDlgControl("BUTTON", "Из списка", STYLE_BUTTON, EXSTYLE_BUTTON, CRect(128, 106, 148, 120), NULL, IDC_SAVE);
            */


            dlg.AddDlgControl("BUTTON", m_strOk, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(16, 106, 56, 120), NULL, IDOK);
            dlg.AddDlgControl("BUTTON", m_strCancel, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(60, 106, 100, 120), NULL, IDCANCEL);
            dlg.AddDlgControl("BUTTON", "Сохранить", STYLE_BUTTON, EXSTYLE_BUTTON, CRect(104, 106, 144, 120), NULL, IDC_SAVE);

            dlg.AddDlgControl("BUTTON", "Из списка", STYLE_BUTTON, EXSTYLE_BUTTON, CRect(148, 106, 188, 120), NULL, IDC_SAVE);



            //            dlg.AddDlgControl("BUTTON", m_strSave, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(164, 106, 160, 120), NULL, IDCANCEL);

            if (dlg.DoModal() == IDOK)
            {
                if (pItem->m_editable) {
                    pItem->m_strValue = LPCTSTR(strValue);
                    pItem->m_undefined = false;
                    itemChanged(pItem);
                    Invalidate();
                }
            }
        }
        else if (pItem->m_type == IT_DATETIME)
        {
            CDynDialogEx dlg(GetParent());
            dlg.SetUseSystemButtons(FALSE);
            dlg.SetWindowTitle(pItem->m_label);
            dlg.SetFont(&m_fntNormal);
            COleDateTime dtValueDate = COleDateTime::GetCurrentTime();

            if (pItem->m_dtValue != 0) {
                dtValueDate = pItem->m_dtValue;
            }

            CTime dtValueTime(pItem->m_dtValue.GetYear(), pItem->m_dtValue.GetMonth(), pItem->m_dtValue.GetDay(), pItem->m_dtValue.GetHour(), pItem->m_dtValue.GetMinute(), pItem->m_dtValue.GetSecond());
            dlg.AddDlgControl("STATIC", m_strDate, STYLE_STATIC, EXSTYLE_STATIC, CRect(7, 3, 60, 12));
            dlg.AddDlgControl("STATIC", m_strTime, STYLE_STATIC, EXSTYLE_STATIC, CRect(67, 3, 120, 12));
            dlg.AddDlgControl("SysDateTimePick32", "", STYLE_DATETIMEPICKER | DTS_SHORTDATEFORMAT, EXSTYLE_DATETIMEPICKER, CRect(7, 13, 60, 26), (void*)&dtValueDate);
            dlg.AddDlgControl("SysDateTimePick32", "", STYLE_DATETIMEPICKER | DTS_TIMEFORMAT, EXSTYLE_DATETIMEPICKER, CRect(67, 13, 120, 26), (void*)&dtValueTime);
            dlg.AddDlgControl("BUTTON", m_strOk, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(7, 37, 60, 51), NULL, IDOK);
            dlg.AddDlgControl("BUTTON", m_strCancel, STYLE_BUTTON, EXSTYLE_BUTTON, CRect(67, 37, 120, 51), NULL, IDCANCEL);

            if (dlg.DoModal() == IDOK)
            {
                pItem->m_dtValue.SetDateTime(dtValueDate.GetYear(), dtValueDate.GetMonth(), dtValueDate.GetDay(),
                    dtValueTime.GetHour(), dtValueTime.GetMinute(), dtValueTime.GetSecond());
                pItem->m_undefined = false;
                itemChanged(pItem);
                Invalidate();
            }
        }
        else if (pItem->m_type == IT_COLOR)
        {
            CColorDialog dlg(pItem->m_clrValue, 0, GetParent());
            if (dlg.DoModal() == IDOK)
            {
                pItem->m_clrValue = dlg.GetColor();
                pItem->m_undefined = false;
                itemChanged(pItem);
                Invalidate();
            }
        }
        ///////////
        else if (pItem->m_type == IT_FONT)
        {
            CFontDialog dlg(&pItem->m_lfValue, CF_EFFECTS | CF_SCREENFONTS, NULL, GetParent());
            if (dlg.DoModal() == IDOK)
            {
                memcpy(&pItem->m_lfValue, dlg.m_cf.lpLogFont, sizeof(LOGFONT));
                pItem->m_undefined = false;
                itemChanged(pItem);
                Invalidate();
            }
        }
        else if (pItem->m_type == IT_CUSTOM)
        {
            if (pItem->m_pCustom->OnEditItem())
            {
                itemChanged(pItem);
                Invalidate();
            }
        }
        else
        {
            //      assert(false);
        }
    }
}

void CPropertyGrid::SetItemTable(HITEM item, const CString schema, const CString table, int id)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_schema = schema;
        pItem->m_table = table;
        pItem->m_id_bd = id;
    }
}

void CPropertyGrid::SetItemHelp(HITEM item, const CString help)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_help = help;
    }
}


void CPropertyGrid::SetItemLength(HITEM item, int length)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        if (pItem->m_type == IT_STRING) {
            pItem->m_length = length;
        }
    }
}

void CPropertyGrid::SetItemReadOnly(HITEM item, bool readonly)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_editable = !readonly;
    }
}

void CPropertyGrid::SetItemNoEdit(HITEM item)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_noedit = true;
    }
}



void CPropertyGrid::SetItemLookup(HITEM item, const CString strValue, int index)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_is_lookup = true;
        pItem->m_index = index;
        pItem->m_strLookup = strValue;

        pItem->m_index_old = index;
        m_old_items[pItem->m_id] = *pItem;

        RecalcLayout();
    }
}

void CPropertyGrid::SetItemFile(HITEM item, const CString filter)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_type = IT_FILE;
        pItem->m_options.push_back(filter);
        RecalcLayout();
    }
}

void CPropertyGrid::SetItemFun(HITEM item, const CString fun)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_is_fun = true;
        pItem->m_strFun = fun;

        if (pItem->m_type == IT_TEXT) {
            pItem->m_type = IT_STRING;
        }

        if (fun == "Hidden") {
            pItem->m_is_hidden = true;
        }

        RecalcLayout();
    }
}

void CPropertyGrid::SetItemValidate(HITEM item, const CString validate)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_is_validate = true;
        pItem->m_strValidate = validate;

        RecalcLayout();
    }
}

void CPropertyGrid::SetItemHide(HITEM item)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_is_hidden = true;
        RecalcLayout();
    }
}




void CPropertyGrid::SetItemAfter(HITEM item, const CString fun)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_is_after = true;
        pItem->m_strAfter = fun;

        if (pItem->m_type == IT_TEXT) {
            pItem->m_type = IT_STRING;
        }

        RecalcLayout();
    }
}

void CPropertyGrid::SetItemBefore(HITEM item, const CString fun)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_is_before = true;
        pItem->m_strBefore = fun;

        if (pItem->m_type == IT_TEXT) {
            pItem->m_type = IT_STRING;
        }
        RecalcLayout();
    }
}

void CPropertyGrid::SetItemFunDisable(HITEM item, const CString fun, const map<int, list<CString> >& map_disable, const map <CString, map<int, list<CString> > >& map_disable0)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        //        pItem->m_is_fun = true;
        pItem->m_is_fun_disable = true;
        //        pItem->m_strFun = fun;

        pItem->m_map_disable = map_disable;

        if (pItem->m_type == IT_TEXT) {
            pItem->m_type = IT_STRING;
        }

        RecalcLayout();
    }
}

void CPropertyGrid::SetItemButtom(HITEM item)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_buttom = true;
        RecalcLayout();
    }
}

void CPropertyGrid::SetItemName(HITEM item, const CString name)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        pItem->m_name = name;
    }
}

void addCItemChanged(list<CItemChanged>& lst, CItemChanged ic0, CString name, double d)
{
    CString s;
    s.Format("%lg", d);

    ic0.sql_value = s;
    ic0.m_name = name;
    lst.push_back(ic0);
}


bool is_US_PTS(CString table, CString fn)
{
    if (table.CompareNoCase("nodes") == 0) {
        if (fn.CompareNoCase("nodeName") == 0) {
            return true;
        }
        if (fn.CompareNoCase("nodeTypeID") == 0) {
            return true;
        }
    }
    return false;
}

bool CPropertyGrid::getChanges(list<CItemChanged>& lst)
{
    CItemChanged ic0;

    lst.clear();

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2) {
            if (it2->m_modified || is_US_PTS(it2->m_table, it2->m_name)) {
      
                CItemChanged ic;
                ic.m_is_lookup = it2->m_is_lookup;

                ic.m_type = it2->m_type;
                ic.id = it2->m_id_bd;
                ic.m_undefined = it2->m_undefined;
                //    EItemType m_type;
                ic.m_name = it2->m_name;
                ic.m_schema = it2->m_schema;
                ic.m_table = it2->m_table;

                ic.m_nValue = it2->m_nValue;
                ic.m_dValue = it2->m_dValue;
                ic.m_strValue = it2->m_strValue;
                ic.m_bValue = it2->m_bValue;
                ic.m_dtValue = it2->m_dtValue;
                ic.m_clrValue = it2->m_clrValue;
                ic.m_lfValue = it2->m_lfValue;
                ic.sql_value = getSQLStr(*it2);

                //        COleVariant var;

                ic.index = it2->m_index;

                auto it3 = m_old_items.find(it2->m_id);
                if (it3 != m_old_items.end()) {
                    CItem pItem_old = it3->second;
                    ic.sql_value_old = getSQLStr(pItem_old);
                    ic.index_old = pItem_old.m_index;
                    ic.m_undefined_old = pItem_old.m_undefined;
                }

                lst.push_back(ic);

                if (is_potr_real_save && ic.m_table == "realConsumers") {
                    ic0 = ic;
                }
            }
        }
    }

    if (is_potr_real_save && 0) {
        CString strValue;
        GetNameValue("schemeNum", strValue);
        bool get_cxema(CString nomer, int& gvpr, int& gvsm, int& gvps, int& gvpw, int& rez, int& gvop, int& gvoo, int& otopln, int& otoplz, int& ventil, int& elevat);

        int gvpr2, gvsm2, gvps2, gvpw2, rez2, gvop2, gvoo2, otopln2, otoplz2, ventil2, elevat2;
        bool ret2 = get_cxema(strValue, gvpr2, gvsm2, gvps2, gvpw2, rez2, gvop2, gvoo2, otopln2, otoplz2, ventil2, elevat2);


        m_otopl;
        m_gv;
        m_ventil;
        m_rez;

        //////////////////

        ///////////

        ic0.m_is_lookup = false;
        ic0.index = -1;

        addCItemChanged(lst, ic0, "calcHLindep", otopln2 != 0 ? m_otopl : 0);
        addCItemChanged(lst, ic0, "calcHLdep", otoplz2 != 0 ? m_otopl : 0);

        addCItemChanged(lst, ic0, "avgHLGVScloseParall", gvpr2 != 0 ? m_gv : 0);
        addCItemChanged(lst, ic0, "avgHLGVScloseMix", gvsm2 != 0 ? m_gv : 0);
        addCItemChanged(lst, ic0, "avgHLGVScloseConseq", gvps2 != 0 ? m_gv : 0);
        addCItemChanged(lst, ic0, "avgHLGVSclosePreON", gvpw2 != 0 ? m_gv : 0);

        addCItemChanged(lst, ic0, "avgHLGVSopenFlow", gvop2 != 0 ? m_gv : 0);
        addCItemChanged(lst, ic0, "avgHLGVSopenRet", gvoo2 != 0 ? m_gv : 0);

        addCItemChanged(lst, ic0, "circHLosOpen", rez2 != 0 ? m_rez : 0);

        addCItemChanged(lst, ic0, "calcHLventil", ventil2 != 0 ? m_ventil : 0);
    }

    return lst.size() > 0;
}



void CPropertyGrid::SetAddCol(int col)
{
    m_col = col;
    if (m_col + 1 > m_cols) m_cols = m_col + 1;
}


//CString CPropertyGrid::getVal(vector<CItem>::iterator& it)
CString CPropertyGrid::getVal(CItem* it) const
{
    if (it->m_undefined) return "";


    CString strValue = it->m_strValue;



    switch (it->m_type)
    {
    case IT_TEXT:
    {
//        size_t j;
        //            for (; (j = strValue.find("\r\n")) != CString::npos;)
        //                strValue.replace(j, 2, "¶");
        strValue.Replace("\r\n", "¶");
        break;
    }

    case IT_INTEGER:
    {
        CString strTemp;
        CString strFormat = "%d";
        if (it->m_options.size() && !it->m_options.front().IsEmpty()) strFormat = it->m_options.front();
        strTemp.Format(strFormat, it->m_nValue);
        strValue = LPCTSTR(strTemp);
        break;
    }

    case IT_DOUBLE:
    {
        CString strTemp, strTempOld;
        CString strFormat = "%lg";
        if (it->m_options.size() && !it->m_options.front().IsEmpty()) strFormat = it->m_options.front();
        strTemp.Format(strFormat, it->m_dValue);
        strTempOld.Format(strFormat, it->m_dValue_old);
        strValue = LPCTSTR(strTemp);

        break;
    }

    case IT_DATE:
    {
        CString strTemp;
        if (it->m_undefined) {
            strValue = m_strUndefined;
        }
        else {
            if (it->m_options.size() && !it->m_options.front().IsEmpty()) strTemp = it->m_dtValue.Format(it->m_options.front());
            else strTemp = it->m_dtValue.Format(VAR_DATEVALUEONLY);
            strValue = LPCTSTR(strTemp);
        }
        break;
    }

    case IT_DATETIME:
    {
        CString strTemp;
        if (it->m_options.size() && !it->m_options.front().IsEmpty()) strTemp = it->m_dtValue.Format(it->m_options.front());
        else strTemp = it->m_dtValue.Format();
        strValue = LPCTSTR(strTemp);
        break;
    }

    case IT_BOOLEAN:
    {
        strValue = it->m_bValue ? m_strTrue : m_strFalse;
        break;
    }

    case IT_COMBO:
    {
        if (it->m_nValue >= 0 && it->m_nValue<int(it->m_options.size()))
            strValue = it->m_options[it->m_nValue];
        break;
    }

    case IT_FILE:
    case IT_FOLDER:
    {
        TCHAR szBuffer[1024];
        strncpy(szBuffer, strValue, 1024);
        strValue = szBuffer;
        break;
    }
    }
    return strValue;
}


bool writeTextClipboard(CWnd* wnd, CString buf)
{
    if (!wnd->OpenClipboard())
    {
        AfxMessageBox(_T("Невозможно открыть буфер обмена"), MB_OK | MB_ICONINFORMATION);
        return false;
    }

    if (!::EmptyClipboard())
    {
        AfxMessageBox(_T("Невозможно очистить буфер обмена"), MB_OK | MB_ICONINFORMATION);
        ::CloseClipboard();
        return false;
    }

    // allocate a (shared) global memory block for the data
    HGLOBAL hTextData = ::GlobalAlloc(GMEM_DDESHARE, buf.GetLength() * sizeof(TCHAR));
    if (hTextData == NULL) return false;

    // now copy the CStrings, terminate each with CR/LF
    LPTSTR lpOut = (LPTSTR)::GlobalLock(hTextData);
    ASSERT(lpOut != NULL);

    lstrcpy(lpOut, buf);

    if (hTextData == NULL || ::SetClipboardData(CF_TEXT, hTextData) == NULL)
    {
        AfxMessageBox(_T("Failed to set clipboard data"), MB_OK | MB_ICONINFORMATION);
        ::CloseClipboard();
        return false;
    }

    if (!::CloseClipboard()) {
        AfxMessageBox(_T("Failed to close clipboard"), MB_OK | MB_ICONINFORMATION);
        return false;
    }

    return true;
}



void CPropertyGrid::Print()
{
    CString buf = "";

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        CString s1 = it->m_title;
        buf += s1;
        buf += "\n";

        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
        {
            CString s2 = it2->m_label;
            CString s3 = getVal(&*it2);

            buf += s2;
            buf += "\t";
            buf += s3;
            buf += "\n";
        }
    }

    writeTextClipboard(this, buf);
}


void CPropertyGrid::Print_struct()
{
    CString buf = "";

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        CString s1 = it->m_title;
        buf += s1;
        buf += "\n";

        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
        {
            CString s2 = it2->m_name;
            CString s3 = it2->m_label;
            //            CString s3 = getVal(&*it2);

            buf += "\"";
            buf += s2;
            buf += "\",\"";
            buf += s3;
            buf += "\"";
            buf += "\n";
        }
    }

    writeTextClipboard(this, buf);
}

void CPropertyGrid::SetName(const char* name)
{
    m_table = name;
    m_szSection.Format("PropertyGrid\\%s", name);
}

bool CPropertyGrid::check_open(CSection* pSection, bool msg)
{
    if (isTU()) {
        return check_openTU(pSection, msg);
    }

    return true;
}


bool CPropertyGrid::isCanMod(HITEM item, CString s_new)
{
    CItem* pItem = FindItem(item);
    if (pItem) {
        if (pItem->m_is_before) {

            CString fun = pItem->m_strBefore;

            if (fun.Find("NotNull") >= 0) {
                vector<CString> match;
                int l = regex_match(".*NotNull\\s+([^ ]+)\\s*$", fun, match);

                if (l > 1) {
                    list<CString> fn = readArgs(match[1]);
                    CString str, ss = "";

                    for (auto &it : fn) {
                        CItem* pItem2 = FindItemName(it);
                        if (pItem2) {
                            CString str, str1, str2;
                            GetItemValue(pItem2->m_id, str2);
                            if (str2 == "" && s_new != "") {
                                if (ss == "") FocusItem(pItem2->m_id);
                                if (ss != "") ss += "\n";
                                ss += pItem2->m_label;
                            }
                        }
                    }

                    if (ss != "") {
                        str.Format("Перед тем как начать редактировать поле \"%s\", должно быть заполнены поля:\n%s", pItem->m_label, ss);
                        AfxMessageBox(str);
                        return false;
                    }
                }
            }
        }
    }

    if (isTU()) {
        return isCanModTU(item);
    }

    return true;
}

bool ind_save(CPropertyGrid* wnd);
bool check_korrozia(CPropertyGrid* wnd);

int regex_match(const char* p, const char* buf, vector<CString>& match);

bool check_unique(CString table, CString fn, int id, CString val);


bool CPropertyGrid::check_NotNull(CString fn)
{
    list<CString> lst = readArgs(fn);

    CString msg = "Заполните все обязательные данные:";

    bool y = false;
    for (auto & it : lst) {
        CItem* item = FindItemName(it);

        if (item) {
            CString value;
            GetItemValue(item->m_id, value);
            if (value == "") {
                msg += "\n";
                msg += item->m_label;
                FocusItem(item->m_id);
                y = true;
            }
        }
    }

    if (y) {
        AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
        return false;
    }

/*

    CItem* item = FindItemName(fn);

    if (item) {
        CString s, value;
        GetItemValue(item->m_id, value);
        if (value == "") {
            s.Format("Поле \"%s\" должно быть заполнено", item->m_label);
            AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
            FocusItem(item->m_id);
            return false;
        }
    }
*/
    return true;
}



bool CPropertyGrid::check_column(CItem* item, CString v, CString &msg, CString &msg_notnull, CString &msg_sezon, int &moveID)
{
    msg = "";
    msg_notnull = "";
    msg_sezon = "";

    if (item->m_is_validate && !item->m_is_hidden) {
        CString s;
        CString strV = item->m_strValidate;

        bool is_null = false;

        if (v == "") is_null = true;
        if (item->m_is_lookup && item->m_index < 0) is_null = true;

        if (item->m_type == IT_DATE && (item->m_dtValue.GetStatus() != 0 || item->m_dtValue == 0)) is_null = true;


        if (is_null) {
            if (strV.Find("NotNIf") >= 0) {

                CString s = item->m_strValidate;

                vector<CString> match;
                int l = regex_match(".*NotNIf\\s+([^ ]+)\\s+([0-9]+)\\s*$", s, match);

                if (l > 1) {
                    CString fn2 = match[1];
                    int id2 = atoi(match[2]);
                    int id3;

                    GetNameValue(CString(fn2), id3);
                    if (id2 == id3) {
//                        msg.Format("Поле \"%s\" должно быть заполнено", item->m_label);
                        msg_notnull = item->m_label;
//                        AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
//                        if (mov) FocusItem(item->m_id);
                        moveID = item->m_id;
                        return false;
                    }
                }
            }
            else if (strV.Find("NotNull") >= 0) {
                msg_notnull = item->m_label;
//                msg.Format("Поле \"%s\" должно быть заполнено", item->m_label);
//                AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
//                if (mov) FocusItem(item->m_id);
                moveID = item->m_id;
                return false;
            }
        }
        if (strV.Find("After") >= 0 && !is_null) {
             vector<CString> match;
             int l = regex_match(".*After\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 CString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      COleDateTime dt1, dt2;
//                      bool is_d1 = GetNameValue(item->m_name, dt1);

                      bool is_d1 = StringToDate(v, dt1);

                      bool is_d2 = GetNameValue(item2->m_name, dt2);
                      if (dt2 > dt1) {
                          msg.Format("\"%s\" должно быть после \"%s\"", item->m_label , item2->m_label);
//                          AfxMessageBox(msg);
//                          if (mov) FocusItem(item->m_id);
                          moveID = item->m_id;
                          return false;
                      }
                 }
             }
        }

#if 0

bool read3double(CString s, double &e1, double &e2, double &e3);

        if (strV.Find("indikator_ves2") >= 0 && !is_null) {
            CString s1, s2;

            double b1, b2, b3;
            double e1, e2, e3;
            CString f1 = "spisok_plastin_pri_ustanovke";
            CString f2 = "spisok_plastin_posle_ispytaniy";

            bool is_d1 = GetNameValue(f1, s1);
            bool is_d2 = GetNameValue(f2, s2);

            CItem* item1 = FindItemName(f1);
            CItem* item2 = FindItemName(f2);

            if (is_d1 && is_d2) {
                read3double(s, b1, b2, b3);
                read3double(s, e1, e2, e3);
                if (e1 < b1 || e2 < b2 || e3 < b3) {
                    msg.Format("\"%s\" должно быть меньше или равно \"%s\"", item1->m_label , item2->m_label);
                }
            }
        }
#endif
        
        if (strV.Find("Less_or_equal") >= 0 && !is_null) {
             vector<CString> match;
             int l = regex_match(".*Less_or_equal\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 CString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      double d1, d2;
                      d1 = atof(v);

                      bool is_d2 = GetNameValue(item2->m_name, d2);

                      if (is_d2 && d2 < d1) {
                          msg.Format("\"%s\" должно быть меньше или равно \"%s\"", item->m_label , item2->m_label);
//                          AfxMessageBox(msg);
//                          if (mov) FocusItem(item->m_id);
                          moveID = item->m_id;
                          return false;
                      }
                 }
             }
        }



        if (strV.Find("Exists") >= 0 && !is_null) {
             vector<CString> match;
             int l = regex_match(".*Exists\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 CString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      CString ss;
                      bool is = GetNameValue(fn, ss);
                      if (!is || ss == "") {
                          msg_notnull = item2->m_label;
//                          msg.Format("Заполните поле \"\"%s\"\"", item2->m_label);
//                          msg.Format("Поле \"%s\" должно быть заполнено перед заполнением поля \"%s\"",  item2->m_label, item->m_label);
//                          AfxMessageBox(msg);
//                          if (mov) FocusItem(item2->m_id);
                          moveID = item2->m_id;
                          return false;
                      }
                 }
             }
        }

        if (strV.Find("NotNIfExists") >= 0 && !is_null) {
             vector<CString> match;
             int l = regex_match(".*NotNIfExists\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 CString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      CString ss;
                      bool is = GetNameValue(fn, ss);
                      if (!is || ss == "") {
                          msg_notnull = item2->m_label;
                          moveID = item2->m_id;
                          return false;
                      }
                 }
             }
        }

        
        if (!is_null && (strV.Find("CurrentSeason") >= 0 || strV.Find("CurrentAndNextSeason") >= 0)) {
            COleDateTime dt1;
            bool is_d1 = GetNameValue(item->m_name, dt1);

            int y1, y2;
            COleDateTime date1, date2;
            get_season(y1, y2, date1, date2);

            if (y1 < 0) {
                msg = "Необходимо выбрать отопительный сезон";
                moveID = item->m_id;
                return false;
            }

            if (!check_season(dt1) && strV.Find("CurrentSeason") >= 0) {
                msg_sezon = item->m_label;
                moveID = item->m_id;
                return false;
            }

        }

        bool CheckFaktoryRiskaForRemont(int id);

        if (!is_null && strV.Find("CheckFaktoryRiskaForRemont") >= 0) {
            if (!CheckFaktoryRiskaForRemont(item->m_id_bd)) {
                msg = "Заполните все обязательные данные \"Список участков для ремонта\"";
//                AfxMessageBox("Заполните все обязательные данные \"Список участков для ремонта\"");
                CItem* item2 = FindItemName("Список участков для ремонта");
                if (item2) {
//                    if (mov) FocusItem(item->m_id);
                    moveID = item->m_id;
                }
                return false;
            }
        }

        if (!is_null && strV.Find("Unique") >= 0) {
            if (!check_unique(item->m_table, item->m_name, item->m_id_bd, v)) {
                msg.Format("Поле \"%s\" должно быть уникальным", item->m_label);
//                AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
//                if (mov) FocusItem(item->m_id);
                moveID = item->m_id;
                return false;
            }
        }

        CString sp = "Pattern ";

        if (v != "" && strV.Find(sp) == 0) {
            CString s = item->m_strValidate;

            vector<CString> match;
            int l = regex_match(".*Pattern \"(.+)\" \"(.+)\".*", s, match);

            if (l > 1) {
                CString pat = match[1];
                CString txt = match[2];

                int l = regex_match(pat, v, match);
                if (l < 2) {
                    msg = txt;
    //                AfxMessageBox(txt, MB_OK | MB_ICONINFORMATION);
    //                if (mov) FocusItem(item->m_id);
                    moveID = item->m_id;
                    return false;
                }
            }
        }


        if (strV.Find("noNodeDuble") >= 0) {
            CString kod, name, s;
            int externalCodeID;

            CItem* item1 = FindItemName("externalNodeName");

            if (item1) {
                GetNameValue("externalCodeID", externalCodeID);
                GetNameValue("externalNodeName", name);
                GetNameValue("externalCodeID", kod);

                bool isDublNode0(int id, int externalCodeID, CString externalNodeName);

                if (isDublNode0(item1->m_id_bd, externalCodeID, name)) {
                    msg.Format("Узел %s %s уже существует", kod, name);
//                    AfxMessageBox(s, MB_OK | MB_ICONINFORMATION);
                    moveID = item->m_id;
                    return false;
                }
            }
        }
    }
    return true;
}


bool CPropertyGrid::required(CItem* item)
{
    CString strV = item->m_strValidate;

    if (item->m_is_validate && !item->m_is_hidden) {
        CString s;

        if (strV.Find("NotNull") >= 0) {
            return true;
        }
        if (strV.Find("Unique") >= 0) {
            return true;
        }

        if (strV.Find("NotNIfExists") >= 0) {
             vector<CString> match;
             int l = regex_match(".*NotNIfExists\\s+([^ ]+)", strV, match);

             if (l > 1) {
                 CString fn = match[1];
                 CItem* item2 = FindItemName(fn);
                 if (item2) {
                      CString ss;
                      bool is = GetNameValue(fn, ss);
                      if (ss != "") {
                          return true;
                      }
                 }
             }
        }
    }

    return false;
}

BOOL isEditGeo(const char* tn, bool isMessage = true);


bool CPropertyGrid::check(bool modified, bool cancel)
{
    bool error = false;

    CString msg = "", msg_notnull = "", msg_sezon = "";

    bool first = true;
    int moveID;

    if (!isEditGeo(m_table, true)) return false;

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        vector<CItem>::iterator it2 = it->m_items.begin();
        for (; it2 != it->m_items.end(); it2++)
        {
            if (it2->m_is_validate) {
                CString v;
                GetItemValue(it2->m_id, v);
                CString msg1, msg_notnull1, msg_sezon1;

                if (!check_column(&*it2, v, msg1, msg_notnull1, msg_sezon1, moveID)) {
                    if (first) {
                        FocusItem(moveID);
                        first = false;
                    }

                    if (msg1 != "") {
                        msg += msg1 + "\n";
                    }
                    if (msg_notnull1 != "") {
                        msg_notnull += CString("\"") + msg_notnull1 += CString("\"\n");
                    }
                    if (msg_sezon1 != "") {
                        msg_sezon += CString("\"") + msg_sezon1 += CString("\"\n");
                    }
                    error = true;
//                    return false;
                }
            }
        }
    }
    if (error) {
        CString s = "";
        if (msg_notnull != "") {
            s += "Заполните обязательные поля:\n";
            s += msg_notnull;
        }

        if (msg_sezon != "") {
            CString ss;
            int y1, y2;
            COleDateTime date1, date2;
            get_season(y1, y2, date1, date2);

            ss.Format("Даты должны быть в отопительном сезоне %d-%d\nв интервале %s - %s:\n", 
                y1, y2, dateToStringV(date1), dateToStringV(date2));

            if (s != "") s += "\n";
            s += ss;
            s += msg_sezon;
        }

        if (msg != "") {
            if (s != "") s += "\n"; 
            s += msg;
        }

        AfxMessageBox(s);
        return false;
    }

    if (m_table == "remont2") {
        CString s1, s2, s3, s4, str;
        int stateID;
        GetNameValue("data_nachala_remonta", s1);
        GetNameValue("data_zaversheniya_remonta", s2);
        GetNameValue("nomer_prikaza", s3);
        GetNameValue("data_prikaza_vvoda_v_ekspluataciyu", s4);
        GetNameValue("stateID", stateID);

        if (s1 != "" && s2 != "" && s3 != "" && s4 != "" && stateID == 2) {
            str.Format("Завершить Ремонт?");
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                SetNameValue("stateID", 3);
            }
        }
    }

    if (m_table == "opres") {
        CString s1, s2, s3, s4, str;
        int stateID;
        GetNameValue("date_opres", s1);
        GetNameValue("vremya_provedeniya_opressovki", s2);
        GetNameValue("prodolzhitelnost_opressovki", s3);
        GetNameValue("reshenie_komissii", s4);
//        GetNameValue("", s5);

        GetNameValue("sostoyanie_opresID", stateID);

        if (s1 != "" && s2 != "" && s3 != "" && s4 != "" && stateID == 2) {
            str.Format("Завершить Опрессовку?");
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                SetNameValue("sostoyanie_opresID", 3);
            }
        }
    }

    if (m_table == "defect") {
        CString s2, s3, s4, s5;
        int stateID;
        
        GetNameValue("stateID", stateID);
        GetNameValue("data_nachala_remonta", s2);
        GetNameValue("nomer_prikaza", s3);
        GetNameValue("data_prikaza_vvoda_v_ekspluataciyu", s4);
        GetNameValue("data_zaversheniya_remonta", s5);

        if (stateID != 2 && stateID != 3 && s2 != "") {
//            str.Format("?");
//            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                SetNameValue("stateID", 2);
//            }
        }

        if (stateID == 2 && s3 != "" && s4 != "" && s5 != "") {
            CString str;
            str.Format("Завершить Ремонт?");
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                SetNameValue("stateID", 3);
            }
        }
    }


    if (m_table == "shurfy") {
        CString s2, s3, s4, s5;
        int stateID;
        
        GetNameValue("sostoyanie_shurfaID", stateID);
        GetNameValue("data_nachala", s2);

        if (stateID != 2 && stateID != 3 && s2 != "") {
//            str.Format("?");
//            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                SetNameValue("sostoyanie_shurfaID", 2);
//            }
        }

        GetNameValue("data_okonchaniya", s3);
        GetNameValue("rezultaty_osmotra", s4);
        GetNameValue("namechennye_meropriyatiya", s5);
        
        if (stateID == 2 && s3 != "" && s4 != "" && s5 != "") {
            CString str;
            str.Format("Завершить Шурфовку?");
            if (AfxMessageBox(str, MB_YESNO) == IDYES) {
                SetNameValue("sostoyanie_shurfaID", 3);
            }
        }
    }



    if (!cancel) {
        CItem* pItem = FindItemName("data_izvlecheniya");
        if (pItem) {
            CString table = pItem->m_table;
            if (table.CompareNoCase(REM_INDICATOR) == 0) {
                COleDateTime dt1, dt2;

                int sostoyanie;
                GetNameValue(CString("sostoyanie"), sostoyanie);
                bool is_d1 = GetNameValue(CString("data_ustanovki"), dt1);
                bool is_d2 = GetNameValue(CString("data_izvlecheniya"), dt2);

                if (!check_korrozia(this)) return false;

                ind_save(this);
                return true;
            }
        }
    }

    if (!modified) return true;

    if (isTU()) return checkTU();


    return true;
}



bool CPropertyGrid::check_exists(CString fn)
{
    CItem* item = FindItemName(fn);
    if (!item) return true;


    CString s;
    bool is_d = GetNameValue(fn, s);
    if (!is_d || s == "") {
        CString ss;
        ss.Format(_TR("Необходимо заполнить поле \"%s\""), item->m_label);
        AfxMessageBox(ss, MB_OK | MB_ICONINFORMATION);
        FocusName(fn);
        return false;
    }
    return true;
}
