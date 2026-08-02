#if !defined(TreeCtrlX_H)
#define TreeCtrlX_H


class CTreeCtrlX : public CTreeCtrl
{
private:
  DECLARE_DYNCREATE(CTreeCtrlX)

public :    
    void SetItemFont(HTREEITEM hItem, LOGFONT& logfont);
    void SetItemBold(HTREEITEM hItem, BOOL bBold);
    void SetItemColor(HTREEITEM hItem, COLORREF color);
    BOOL GetItemFont(HTREEITEM hItem, LOGFONT * plogfont);
    BOOL GetItemBold(HTREEITEM hItem);
    COLORREF GetItemColor(HTREEITEM hItem);

    protected:
      struct Color_Font
      {
        COLORREF color;
        LOGFONT  logfont;
      };
      CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont ;

    afx_msg void OnPaint() ;

  DECLARE_MESSAGE_MAP()
};



#endif // !defined(TreeCtrlX_H)
