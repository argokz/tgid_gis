#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "win.h"
#include "geofile.h"

#include "maptabs.h"

#include "math.h"


#include "ado.h"


//#include "grid/msword8.h"
#include <fstream>

#include "InputDia.h"

#include <duckx.hpp>

CString sezon_name(int y);

bool checkFileOpen(CString doc_name);

const duckx::formatting_flag fNorm = duckx::align_left | duckx::size_8 | duckx::tnr_style;
//const duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
const duckx::formatting_flag fBoldCenter = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
const duckx::formatting_flag fItalicRight = duckx::align_right | duckx::tnr_style | duckx::italic | duckx::size_8;
const duckx::formatting_flag fNormCenter = duckx::align_center | duckx::size_8 | duckx::tnr_style;


bool write_table(duckx::Table &table1, CAdoFile *ado, CString q, int ncols, const int* cols, bool numb)
{
    bool ret = ado->openTable0(q);
    if (!ret)
    {
        AfxMessageBox("Ошибка в запросе");
        return false;
    }

    CString str;
    int nn = 1;

    while (!ado->isEOF()) {
        duckx::TableRow row = table1.insert_row();

        if (cols) {
            if (numb) {
                str.Format("%d", nn++);
            }
            for (long i = 0; i < ncols; i++) {
                CString str = ado->readStr(cols[i]);
                row.insert_cell(str.AllocSysString(), fNorm);
            }
        }
        else {
            for (long i = 0; i < ado->NFlds(); i++) {
                str = ado->readStr(i);
                if (numb && i == 0) {
                    str.Format("%d", nn++);
                }
                row.insert_cell(str.AllocSysString(), fNorm);
            }
        }
        ado->MoveNext();
    }
    ado->closeTable();

    return true;
}


duckx::Document & open_docx(CString doc_name)
{
    duckx::Document* doc = nullptr;

    if (!checkFileOpen(doc_name))
        return *doc;

    CString fname;

    fname.Format("%s\\excel2\\docx\\empty_doc_album.docx", argpath());

    CopyFile(fname, doc_name, 0);


    doc = new duckx::Document(doc_name.GetString());

    if (doc) {
        doc->open();
    }

    return *doc;
}

void show_docx(CWnd* wnd, duckx::Document& doc)
{
    if (!&doc) return;

    doc.save();

    HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc.get_file().c_str(), NULL, NULL, SW_SHOWMAXIMIZED);
}

void Remont_docx1(CWnd* wnd, CAdoFile* ado, CString q, int m_sezon_korrozia)
{
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("График выполнения ремонтов.docx"));
    CString fname;

    duckx::Document &doc = open_docx(doc_name);
    if (!&doc) return;

    COleDateTime v = COleDateTime::GetCurrentTime();

    CString  s;

//    s.Format("отопительный сезон/период\nна «%s»", v.Format("%d.%m.%Y"));   
    s.Format("%s\nна «%s»", sezon_name(m_sezon_korrozia), v.Format("%d.%m.%Y"));   

    CString energosist = readTableValue(ado, "SELECT TOP 1 kratkoe_naimenovanie_es FROM energosistemy");
    doc.paragraphs().insert_paragraph(energosist.AllocSysString(), fItalicRight);
     

//    doc.paragraphs().insert_paragraph(L"Наименование теплоснабжающей организации организации", fItalicRight);
    doc.paragraphs().insert_paragraph(L"", fBoldCenter);
    doc.paragraphs().insert_paragraph(L"График выполнения ремонтов", fBoldCenter);
    doc.paragraphs().insert_paragraph(s.AllocSysString(), fNormCenter);
    doc.paragraphs().insert_paragraph(L"", fBoldCenter);

    duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
    duckx::TableRow row = table1.insert_row();

    row.insert_cell(L"п/п", fBoldCenter, 1, 1);//
    row.insert_cell(L"Контур ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Дата начала ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Дата окончания ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Вид ремонта", fBoldCenter, 1, 1);//
    row.insert_cell(L"Тепловая сеть", fBoldCenter, 1, 1);//
//    row.insert_cell(L"Объем средств, тыс. тг", fBoldCenter, 3, 1);//
//    row.insert_cell(L"Ремонтный персонал, чел.", fBoldCenter, 3, 1);//
    row.insert_cell(L"Проложено труб, м.п", fBoldCenter, 3, 1);//
    row.insert_cell(L"Восстановление тепловой изоляции, м2", fBoldCenter, 3, 1);//
    row.insert_cell(L"Восстановление канальной прокладки, м", fBoldCenter, 3, 1);//
    row.insert_cell(L"Асфальтирование, м2", fBoldCenter, 3, 1);//
    row.insert_cell(L"Ответственный за ремонт", fBoldCenter, 1, 1);//
    row.insert_cell(L"Подразделение производившее ремонт", fBoldCenter, 1, 1);//

    row = table1.insert_row();

    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

/*
    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

*/
    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"план", fBoldCenter, 1);//
    row.insert_cell(L"факт", fBoldCenter, 1);//
    row.insert_cell(L"%", fBoldCenter, 1);//

    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    row.insert_cell(L"", fBoldCenter, 1, 2)                    ; 

    write_table(table1, ado, q, 0, 0, true);

    show_docx(wnd, doc);
}
