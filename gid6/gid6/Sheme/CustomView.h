#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCustomView view

//для CView для отображения
#define interval 70
#define deep 30
#define grid 1


#include "temlpwnd.h"
#include "ConsrtuctorDoc.h"

#define  ELEM_KOL 15

class CCustomView : public CTemplWnd
{
friend class CConsrtuctorDoc; //Дружественный класс документа -- для доступа к защищенным переменным
public:
	CCustomView();
	~CCustomView();

	bool SwitchShowFont();				//перекидывает (показывает/прячет) все надписи на схеме
	void ShowFont(bool show=true);		//показывает/прячет все надписи на схеме
	void ShowBalloons(bool show=true);	//показывает/прячет течение воды на схеме
	
	
	int element;  //Переменная, обозачающая разновидность вставляемого элемента схемы. (если равна -1, то прорисовка останавливается в ConsructorView)
	CDuo duo_selected;// Переменная, сопоставляемая с выделенным элементом (указывает тип элемента и положение в массиве)
	long type_selected;// Переменная, сопоставляемая с выделенным элементом
	HCURSOR old;	//Указатель на курсор "обычный"
	HCURSOR cr;		//Указатель на курсор "рука"
	HCURSOR zoom_in,zoom_out;//Указатель на курсор "лупа+","лупа-"
	HCURSOR cross;//Указатель на курсор "крест"
	int listik; //номер скомпилированного множества объектов отображения

	
	void StartMovie(); //Начинает циклическую прорисовку
	void StopMovie();	//Останавливает циклическую прорисовку	
	virtual void CompileList(){}; //Компиляция статичниго изображения
	virtual void CopyScreen(CCustomView * TemplView);//Копирование настроек экрана

	void OnFont();  //Вызывает диалог об изменении размера шрифта надписей

	//{{AFX_VIRTUAL(CCustomView)
	public:
	virtual void OnInitialUpdate(); //Установка размеров для полос прокрутки окна при его создании
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);//Подготовка окна к прорисовке (проскролливание) 
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs); //Расставляет необходимые параметры для OpenGL при создании окна
	virtual void OnDraw(CDC* pDC) {};      //Организует действия при прорисовке
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE); //Убирает все режимы при попытке проскролиивания	
	//}}AFX_VIRTUAL
protected:	
/////////////////////////////////////////////////////////////////////////////
// CCustomView -- диагностика 	
#ifdef _DEBUG //
	virtual void AssertValid() const; //Диагностика 
	virtual void Dump(CDumpContext& dc) const;//Диагностика 
#endif
// CCustomView -- диагностика 
/////////////////////////////////////////////////////////////////////////////
	HGLRC hGLRC; //Указатель на device context OpenGL для rendering-a
public:
	CClientDC *pdc; //Указатель на device context окна 
protected:
	int SetWindowPixelFormat(HDC); //Функция подготовки окна для отображения OpenGL объектов
	
	CPoint origin,tempOrigin;	//точки сдвига левого верхнего угла окна при проскролливании (два для дубляжа)
	double dx,dy;				//сдвиг мирового окна
	CRect rectClient;		//Возвращает оконные координаты для прорисовки

	int uIDEvent;// Переменная, сопоставляемая с номером таймера

	virtual bool SendData(long param){return false;};// Посылка данных родительскому окну
	template <class V> void AnimElem(V * vect); //Прорисовывает анимацию объектов
	void display(); //Прорисовывает все окно OpenGL
/////////////////////////////////////////////////////////////////////////////
// CCustomView -- преобразование координат 		
	double getGLX(long x);	//Возвращает мировую координату X по оконной координате X
	double getGLY(long y);	//Возвращает мировую координату Y по оконной координате Y
	long getX(double x);	//Возвращает оконную координату X по мировой координате X
	long getY(double y);	//Возвращает оконную координату Y по мировой координате Y
// CCustomView -- преобразование координат 
/////////////////////////////////////////////////////////////////////////////
	void DrawRect(CRect r); //Отрисовка прямоугольника выделения поверх окна
/////////////////////////////////////////////////////////////////////////////
// CCustomView -- Движение мыши
	MOUSEXY m;	
// CCustomView -- Движение мыши
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCustomView -- zooming
	CRect RubberRect;	//"резиновый" прямоугольник выделения в оконных координатах
	WRECT RubberGL;		//"резиновый" прямоугольник выделения в мировых координатах
	bool Zooming;		//1 - идет ZoomIn  0 - ZoomOut 
	bool ZoomPrepared;	//1 - подготовлен ZoomIn 0 - нет
	void DrawRubberRect(CRect RubberRect); //прорисовка "резинового" прямоугольника
	STACK Zoom_stack;	//Стек приближения
	void ZoomToStepPoint(long step,CPoint point=0);	//Приближение с шагом step в точку point
	void ZoomIn(const CPoint point);	//увеличение в точке point
	void ZoomOut(const CPoint point);	//ументшение в точке point
	void CorrectRect(CRect &Rectangle);	//корректировка размеров Rectangle	
	void OnZoomInMouseUP();				//Вызывает окончание растягивания "резиного" прямоугольника выделения ZoomIn
	void OnZoomInMouseMove(CPoint point); //Вызывает процесс растягивания "резиного" прямоугольника выделения ZoomIn
	CPen * DotPen;	
public:
	unsigned char DebugMode;						//режим работы 0 - обычный, 1 - ZoomIn,2 - ZoomOut
	void ChangeDebugMode(unsigned char newDebugMode);
	////////////вызов снаружи
	void ZoomCustom();					//приближение с вызовом диалога 
	void FitToScreen(); //Выбирает масштаб для вписывания схемы в размер экрана
	///////////////
protected:
// CCustomView -- zooming
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCustomView -- перетаскивание под-элементов
	PLACE TxtElemPlace;		//структура, содержащая сведения о элементе, подэлемент которого перетаскивается
	double TxtShift[2];		//сдвиг точки ухвата мыши от левого верхнего угла границы перетаскиваемого подэлемента
	CRect Txtrect;			//граница перетаскиваемого подэлемента
	BOOL DraggingText;		//идет перетаскивание
	bool MouseOnSub(bool with_switch=true);	//проверка и начало перетаскивания(если вернулась 1). если with_switch=1, то можно перетаскивать и кнопки, иначе  
	void StartDragText(CTempl * loop);		//Начало перетаскивания объекта "loop"
	void MoveDrag(CPoint point);			//Процесс перетаскивания объекта "loop"
	void EndDragText();						//Конец процесса перетаскивания объекта "loop"
	template <class V> bool MouseOnSubDevice(V * vect,CTempl * &found); //Проверка нахождения мыши на объекте для перетаскивания(елси да, то "found"- это и есть объект, возвращает 1)
// CCustomView -- перетаскивание под-элементов
/////////////////////////////////////////////////////////////////////////////

	
	template <class V> void ShowFont(V * vect,bool show); //скрывает или повазывает надписи в указанном массиве


	void InitDisplay();		//Инициализация мирового окна при инзменении окна просмотра
	void UpdateScrollSizes();
	
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
	CConsrtuctorDoc * m_pDocument;
	CConsrtuctorDoc* GetDocument(); //Получение документа для вида
/////////////////////////////////////////////////////////////////////////////

	//{{AFX_MSG(CCustomView)
	virtual afx_msg void OnDestroy(); //Уничтожает ненужные переменные при уничтожении окна
	virtual afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); //Настраивает окно для отображения OpenGL
	virtual afx_msg void OnTimer(UINT nIDEvent); //Функция, вызываемая по таймеру для прорисовки
	afx_msg void OnSize(UINT nType, int cx, int cy); //Происходит при изменении размеров окна и производит синхронизацию мирового окна и окна просмора
	virtual afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

