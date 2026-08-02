#pragma once

#include "clip/vectmatr.h"
#include <cxema/graph2.h>
#include <gidview/Scroll.h>
#include <cxema/cxema1.h>
#include <cxema/graph2.h>

//#include "Okno.h"

class QPrinter;
class CPjezoTable;
class MyMain;

struct PjezoAction
{
QAction *aListPjezo; // Список направлений...
QAction *aFilePrint; // Печать...\tCtrl+P
QAction *aFilePrintPreview; // Предварительный просмотр
QAction *aFilePrintSetup; // Параметры страницы...
QAction *aAppExit; // Выход
QAction *aEditCopy2; // Копировать\tCtrl+C
QAction *aPlus; // Увеличить
QAction *aMinus; // Уменьшить
QAction *aOkno; // Информация
QAction *aPodacha; // Подающий
QAction *aObratka; // Обратный
QAction *aRealP; // Факт
QAction *aStat; // Статика
QAction *aNapor; // Напоры
QAction *aOtmet; // Отметки
QAction *aVysot; // Здания
QAction *aExcel; // Таблицы Excel..
QAction *aTechinfo; // Тех.информация
QAction *aSavepjezo; // Сохранить направление
QAction *aDouble; // Двойной пьезометр
QAction *aViewStatusBar; // Строка статуса
QAction *aInf; // Отображаемая информация...
QAction *aFont; // Шрифт
QAction *aHelpFinder; // Содержание
QAction *aUpdate; // Последнее обновление
QAction *aUpdateSetup; // Настройка обновлений
QAction *aAppAbout; // О программе...

QAction *aMasall;  //
QAction *aMoveTo;  //

};




/////////////////////////////////////////////////////////////////////////////
// CPjezo view

struct CPNode1 {
  CNode2 *n;
  double pihP1, pihO1;
  double pihP2, pihO2;
  double pP_fact, pO_fact;
  double pP_old, pO_old;

  double pihP1_old, pihO1_old;
  double pihP2_old, pihO2_old;

  double tP, tO;
  double h;
  double geodz;
  double hz;
  double len, lenO, vO, lenP, vP;

  CPNode1() : pihP1(0), pihO1(0), pihP2(0), pihO2(0), h(0), geodz(0), hz(0), n(NULL), pP_fact(0), pO_fact(0)
  {}
};

struct CPLine1 {
  CLine2 *l;
  double len;
  double diam;
  double r;
  double g; // Расход
  double w; // Скорость
  double a14;  //  Удельные линейные потери напора на участке
  double a15;  //  Линейные потери напора на участке
  double a16;  //  Местные потери напора на участке
  double a17;  //  потери напора на участке

  CPLine1() : len(0), diam(0), r(0), g(0), l(NULL)
  {}
};

struct CPNode {
    CPNode1 rn[2];
};

struct CPLine {
    CPLine1 rl[2];
};




class CPjezo : public Scroll
{
    Q_OBJECT

friend CPjezoTable;

public:
  CPjezo(CCxema *_cxema, QWidget *parent, GidWidget *_gid);
  virtual ~CPjezo();

// Attributes
public:
  CCxema *m_cxema;
  CGraph2 *m_graph;
  CGraph2 *m_graph2;
//  CGidFile *m_gid_file;
  double m_len;
  double min_pih;
  double max_pih;
  int m_mas;
  int m_n;

  bool m_napor, m_otmet, m_vysot, m_okno, m_stat, m_real, m_dbl;
  bool m_podacha, m_obratka, m_fact;


// Operations
public:

  void initPjezo();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPjezo)
  public:
//  virtual void OnInitialUpdate();
  protected:
//  virtual void OnDraw(QPainter* pDC);      // overridden to draw this view
//  virtual void OnPrint(QPainter* pDC, CPrintInfo* pInfo);
//  virtual bool OnPreparePrinting(CPrintInfo* pInfo);
  //}}AFX_VIRTUAL



  // Generated message map functions
protected:
    int nkx, nky;
    int m_x1, m_y1, m_x2, m_y2;
    int width1, height1;
    int w, h;
    double h_max;

    double m_coef = 1.;

    QFont m_font;

    QFont fontH, fontV;
    deque<CNode2> list_pjezo;
    bool m_bIsInfo[5];

    int m_calculationID;
    int m_fileID;

    QMessageBox *okno;
  //  COkno okno;

    vector<CPLine> m_vlineP;
    vector<CPLine> m_vlineO;
    vector<CPNode> m_vnode;
  
    double getPih(int i, int typ, bool first, int nras);
    void redrawPaint(QPainter *);
    void drawGr(QPainter *, int color, int typ, int nras);
    void drawSetka(QPainter *);
    void drawOkno(QPainter *);
    void House(QPainter *, int x, int y, int h);
    void readUS_OUT(CGraph2 *m_graph, int num, int nras);
    void CopyBlockP(CFRect rect);
    void saveSettings();

//    void closeEvent(QCloseEvent *event);
//    void OnHide();

  void createMenu(MyMain *main_window);
  void createActions();
  void createToolBar(MyMain *main_window);

    void draw_rect(QPainter *painter, int width, int height, int mas);
    void draw(QPainter *painter);

    void keyPressEvent(QKeyEvent *event);
    void hideEvent(QHideEvent *event);


private:
    PjezoAction pjezoAction;

public slots:
   void onToggleValue(bool on);
  

  void onPlus();
  void onMinus();
  void onFont();
  void onMasall();
//  void onKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//  void onUpdateNapor(CCmdUI* pCmdUI);
//  void onNapr();

    void onOkno();
  //  void onUpdateOkno(CCmdUI* pCmdUI);
    void onDouble();
  //  void onUpdateDouble(CCmdUI* pCmdUI);
    void onEditCopy();
  //  void onUpdateStat(CCmdUI* pCmdUI);
    void onTechinfo();
    void onExcel();
  //  void onUpdateRealP(CCmdUI* pCmdUI);
    void onSavePjezo();
    void onSavePjezo2();
    void onMoveTo();

    void onFilePrint();
    void onFilePrintPreview();
    void printPreview(QPrinter *printer);

    void closeTable();


    void onListPjezo();

  //  bool OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  //  void onMButtonDown(UINT nFlags, CPoint point);
    void onInf();

private:
    GidWidget *gid;
};

