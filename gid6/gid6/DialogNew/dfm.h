#if !defined(_DFM_H_)
#define _DFM_H_


enum {
  ID_TDbTable,
  ID_TStringGrid,
  ID_TCheckBox,
  ID_TEdit,
  ID_TPanel,
  ID_TButton,
  ID_TEditFile,
  ID_TEditFilePic,
  ID_TEditFileDoc,
  ID_TEditDir,
  ID_TEditPicture,
  ID_TComboEdit,
  ID_TForm1,
  ID_TGroupBox,
  ID_TLabel
};

enum {
ID_Caption,
ID_Color,
ID_Cursor,
ID_Font_Charset,
ID_Font_Color,
ID_Font_Height,
ID_Font_Name,
ID_Font_Style,
ID_Height,
ID_Left,
ID_OldCreateOrder,
ID_ParentColor,
ID_ParentFont,
ID_PixelsPerInch,
ID_TabOrder,
ID_Text,
ID_TextHeight,
ID_Top,
ID_Width
};

enum {
ID_align,
ID_body,
ID_border,
ID_charset,
ID_col,
ID_colspan,
ID_content,
ID_head,
ID_height,
ID_html,
ID_http_equiv,
ID_meta,
ID_rowspan,
ID_span,
ID_table,
ID_td,
ID_tr,
ID_width
};


enum {
  ID2_DbQuery,
  ID2_DbTable,
  ID2_EditCall,
  ID2_Call,
  ID2_Call_Table,
  ID2_Call_Table_MDB,
  ID2_Call_Table_Table,
  ID2_Call_Table_ID,
  ID2_Call_Table_Name,

  ID2_Table,
  ID2_If,
  ID2_EndIf,
  ID2_LoopTable,
  ID2_Loop,
  ID2_EndLoop,
  ID2_NextRow,
  ID2_FirstRow,
  ID2_Picture,
  ID2_File,
  ID2_FileM,
  ID2_SetFile,
  ID2_Dir,
  ID2_DirM,
  ID2_Font,
  ID2_Edit,
  ID2_ReadOnly,
  ID2_ComboEdit,
  ID2_ComboEdit2,
  ID2_Label,
  ID2_Button,
  ID2_Set,
  ID2_Cols,
  ID2_Rows,
  ID2_Width,
  ID2_Height,
  ID2_BkColor,
  ID2_Bold,
  ID2_Italic,
  ID2_Normal,
  ID2_Enter,
  ID2_Buffer,
  ID2_Pattern,
  ID2_CopyDoc,

  ID2_ItIsRow,


  ID2_Title,
  ID2_FileExt,
  ID2_FileFilter,
  ID2_FilePath,
  ID2_FilePath1,
  ID2_FilePath2,
  ID2_FilePath3,
  ID2_FilePath4,
  ID2_FilePath5,
  ID2_FilePath6
};


struct STR
{
  char *str;
  int cod;
};


struct OBJ
{
  int ID;
  CString name;

  CString Field;
  CString Caption;
  int Color;
  int Cursor;
  int Font_Charset;
  int Font_Color;
  int Font_Height;
  int Font_Name;
  int Font_Style;
  int Height;
  int Left;
  int OldCreateOrder;
  int ParentColor;
  int ParentFont;
  int PixelsPerInch;
  int TabOrder;
  int Text;
  int TextHeight;
  int Top;
  int Width;
};


struct OBJ2
{
  int ID;
  CString name;
  CString text;
  int row, col;
  int width, height;

  OBJ2()
  {
    text = "";
  }

};




#endif