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


bool checkFileOpen(CString doc_name);

const duckx::formatting_flag fNorm = duckx::align_left | duckx::size_8 | duckx::tnr_style;
//const duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
const duckx::formatting_flag fBoldCenter = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
const duckx::formatting_flag fItalicRight = duckx::align_right | duckx::tnr_style | duckx::italic | duckx::size_8;
const duckx::formatting_flag fNormCenter = duckx::align_center | duckx::size_8 | duckx::tnr_style;

CString sezon_name(int y);


bool write_table(duckx::Table &table1, CAdoFile *ado, CString q, int ncols, const int* cols, bool numb);

duckx::Document & open_docx(CString doc_name);
void show_docx(CWnd* wnd, duckx::Document& doc);

void Remont_docx2(CWnd* wnd, CAdoFile* ado, CString q, int m_sezon_korrozia)
{
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("План ремонтов (контуров) по сезону_все сезоны.docx"));
    CString fname;

    duckx::Document &doc = open_docx(doc_name);
    if (!&doc) return;

    COleDateTime v = COleDateTime::GetCurrentTime();


    CString energosist = readTableValue(ado, "SELECT TOP 1 kratkoe_naimenovanie_es FROM energosistemy");

    CString  s;
    s.Format("%s\nдата утверждения %s", energosist, v.Format("%d.%m.%Y"));   

    doc.paragraphs().insert_paragraph(s.AllocSysString(), fItalicRight);
     

//    doc.paragraphs().insert_paragraph(L"Наименование теплоснабжающей организации организации", fItalicRight);
    doc.paragraphs().insert_paragraph(L"", fBoldCenter);
//    doc.paragraphs().insert_paragraph(L"График выполнения ремонтов", fBoldCenter);
    doc.paragraphs().insert_paragraph(L"ПЛАН\nкапитальных/инвестиционных ремонтов", fBoldCenter);

    s = sezon_name(m_sezon_korrozia);

    doc.paragraphs().insert_paragraph(s.AllocSysString(), fNormCenter);
    doc.paragraphs().insert_paragraph(L"", fBoldCenter);

    duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
    duckx::TableRow row = table1.insert_row();


    row.insert_cell(L"п/п", fBoldCenter);
    row.insert_cell(L"Контур ремонта", fBoldCenter);
    row.insert_cell(L"Дата начала ремонта", fBoldCenter);
    row.insert_cell(L"Дата окончания ремонта", fBoldCenter);
    row.insert_cell(L"Вид ремонта", fBoldCenter);
    row.insert_cell(L"Тепловая сеть", fBoldCenter);
    row.insert_cell(L"Описание работ", fBoldCenter);
    row.insert_cell(L"Объем выделенных средств, тыс. тг с НДС", fBoldCenter);
    row.insert_cell(L"Ремонтный персонал, планиру-емый, чел.", fBoldCenter);
    row.insert_cell(L"Ответственный за ремонт", fBoldCenter);
    row.insert_cell(L"Подразделение производившее ремонт", fBoldCenter);


    write_table(table1, ado, q, 0, 0, true);

    show_docx(wnd, doc);
}
