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

BSTR getWideString(COleVariant v);
static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
using pugi::xml_node;
bool checkFileOpen(CString doc_name);


void AktRasledovania2(CWnd* wnd, int id) {

    COleVariant v;
    CString ss;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Акт расследования ТН.docx"));
    CString fname;

    if (!checkFileOpen(doc_name))
        return;

    fname.Format("%s\\excel2\\docx\\empty_doc.docx", argpath());

    CopyFile(fname, doc_name, 0);

    CString q, fStr;
    
    CAdoFile* ado = getAdo(getPsAdoName());
//    fStr.Format("%ssql\\docxsql\\act_defects_new_fio.sql", argpath());

    fStr.Format("%ssql\\objects\\defect\\defects_all.sql", argpath());

    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;
     
//        myId.Format("%d", id);
        myId.Format("d.id=%d AND ", id);
//        q.Replace("$fileID$",myId);

        q.Replace("$state_cond$", myId);
        q.Replace("$season_condition$", " IS NULL OR 1=1 ");
        q.Replace("$and_condition2$", "");
        

    CInputDialog inD(AfxGetMainWnd(), "", "?", q);
    inD.DoModal();
        

        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }

        duckx::Document doc(doc_name.GetString());

        doc.open();

        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
        
        CString nomer_akta = "", data_sostavleniya_akta = "";
//        wnd->GetNameValue("nomer_akta", nomer_akta);
//        wnd->GetNameValue("data_sostavleniya_akta", data_sostavleniya_akta);

        nomer_akta = ado->readStr("Номер акта");
        data_sostavleniya_akta = ado->readStr("Дата составления акта");



        ss.Format("Акт №%s", nomer_akta);
 

        doc.paragraphs().insert_paragraph(ss.AllocSysString(), fBold);
        doc.paragraphs().insert_paragraph(L"расследования технологического нарушения", fBold); 

        
        ss.Format("дата составления: %s", data_sostavleniya_akta);
        doc.paragraphs().insert_paragraph(ss.AllocSysString(), fBold);//
        doc.paragraphs().insert_paragraph(L"", fBold);
        

        duckx::formatting_flag fNorm = duckx::align_left | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::size_11 | duckx::tnr_style | duckx::bold;
        //Table1
        duckx::Table table1 = doc.tables().insert_table(duckx::align_center);

        duckx::TableRow rowT1 = table1.insert_row();
        v = ado->read("Район эксплуатации");
        duckx::TableCell cell1T1 = rowT1.insert_cell(L"Район эксплуатации:", fNorm);//
        duckx::Paragraph p1 = cell1T1.paragraphs().insert_paragraph(L"\"", fBoldLeft);
        p1.add_run(getWideString(v), fBoldLeft);
        p1.add_run(L"\"", fBoldLeft);

        duckx::TableCell cell2T1 = rowT1.insert_cell(L"Магистраль:", fNorm);//
        duckx::Paragraph p2 = cell2T1.paragraphs().insert_paragraph(L"\"", fBoldLeft);
        CString val, val1, val2, val3;
//        wnd->GetNameValue("naimenovanie_magistrali", val1);
        val1 = ado->readStr("Магистраль");


        p2.add_run(val1.AllocSysString(), fBoldLeft);
        p2.add_run(L"\"", fBoldLeft);
        

        duckx::TableRow row2T1 = table1.insert_row();
        v = ado->read("Наименование участка");
        duckx::TableCell cell3T2 = row2T1.insert_cell(L"Участок эксплуатации:", fNorm);//
        duckx::Paragraph p3 = cell3T2.paragraphs().insert_paragraph(L"\"", fBoldLeft);
        p3.add_run(getWideString(v), fBoldLeft);
        p3.add_run(L"\"", fBoldLeft);

        v = ado->read("Начальник участка");
        duckx::TableCell cell2T2 = row2T1.insert_cell(L"Начальник участка:", fNorm);//
        duckx::Paragraph p4 = cell2T2.paragraphs().insert_paragraph(L"\"", fBoldLeft);
        p4.add_run(getWideString(v), fBoldLeft);
        p4.add_run(L"\"", fBoldLeft);

        doc.paragraphs().insert_paragraph(L"", fBold);//

         //Table2
        //row 1
        duckx::Table table2 = doc.tables().insert_table(duckx::align_center);

        duckx::TableRow row1T2 = table2.insert_row();
        row1T2.insert_cell(L"1.", fNorm);
        row1T2.insert_cell(L"Наименование участка:", fNorm);//
        val1 = getWideString(ado->read("Код расчетной схемы начального узла"));
        val2 = getWideString(ado->read("Наименование начального узла"));
//        wnd->GetNameValue("name1", val2);
        val2 = ado->readStr("Наименование начального узла");
        val3 = "\"" + val1 + "\", " + "\"" + val2 + "\"";
        row1T2.insert_cell(val3.AllocSysString(), fBoldLeft);//
        
        row1T2.insert_cell(L"-", fBoldLeft);
        val1 = getWideString(ado->read("Код расчетной схемы конечного узла"));
        val2 = getWideString(ado->read("Наименование конечного узла"));
        val3 = "\"" + val1 + "\", " + "\"" + val2 + "\"";
        row1T2.insert_cell(val3.AllocSysString(), fBoldLeft);

        //row 2
        duckx::TableRow row2T2 = table2.insert_row();
        row2T2.insert_cell(L"2.", fNorm);
        row2T2.insert_cell(L"Дата и время обнаружения технологического нарушения:", fNorm);//
//        wnd->GetNameValue("data_osmotra", val3);
        val3 = ado->readStr("Дата обнаружения нарушения");
        row2T2.insert_cell(val3.AllocSysString(), fNorm);//

        row2T2.insert_cell(L",", fNorm);
//        wnd->GetNameValue("vremya_osmotra", val3);
        val3 = ado->readStr("Время обнаружения повреждения");
        row2T2.insert_cell(val3.AllocSysString(), fNorm);

        //row 3
        duckx::TableRow row3T2 = table2.insert_row();
        row3T2.insert_cell(L"3.", fNorm);
        row3T2.insert_cell(L"Адрес:", fNorm);//
        val3 = ado->readStr("Адрес");
        row3T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 3.1
        duckx::TableRow row3_1T2 = table2.insert_row();
        row3_1T2.insert_cell(L"4.", fNorm);
        row3_1T2.insert_cell(L"Ближайшая камера и расстояние  до неё:", fNorm);//
        val1 = ado->readStr("Наименование ближайшей камеры");
        val2.Format("%.2fм",ado->read_double("Расстояние до нарушения от ближайшей камеры, м"));
        if (!val1.IsEmpty()) {
            val3 = val1;
            if (!val2.IsEmpty()) {
                val3 = val3 + ", " + val2;
            }
        }
        row3_1T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 4
        duckx::TableRow row4T2 = table2.insert_row();
        row4T2.insert_cell(L"5.", fNorm);
        row4T2.insert_cell(L"Поврежденный трубопровод:", fNorm);//
        val3 = ado->readStr("Поврежденный трубопровод");
        row4T2.insert_cell(val3.AllocSysString(), fNorm,3);

        //row 5
        duckx::TableRow row5T2 = table2.insert_row();
        row5T2.insert_cell(L"6.", fNorm);
        row5T2.insert_cell(L"Вид нарушения:", fNorm);//
        val3 = ado->readStr("Вид нарушения");
        row5T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 6
        duckx::TableRow row6T2 = table2.insert_row();
        row6T2.insert_cell(L"7.", fNorm);
        row6T2.insert_cell(L"Категория нарушения:", fNorm);//
        val3 = ado->readStr("Категория нарушения");
        row6T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 7
        duckx::TableRow row7T2 = table2.insert_row();
        row7T2.insert_cell(L"8.", fNorm);
        row7T2.insert_cell(L"Поврежденный элемент:", fNorm);//
        val3 = ado->readStr("Поврежденный элемент");
        row7T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 8
        duckx::TableRow row8T2 = table2.insert_row();
        row8T2.insert_cell(L"9.", fNorm);
        row8T2.insert_cell(L"Характер повреждения:", fNorm);//
        val3 = ado->readStr("Характер повреждения");
        row8T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 9
        duckx::TableRow row9T2 = table2.insert_row();
        row9T2.insert_cell(L"10.", fNorm);
        row9T2.insert_cell(L"Место расположения центра повреждения на трубопроводе,  часов:", fNorm);//
        val3 = ado->readStr("Место расположения центра повреждения на трубопроводе,  часов");
        row9T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 10
        duckx::TableRow row10T2 = table2.insert_row();
        row10T2.insert_cell(L"11.", fNorm);
        row10T2.insert_cell(L"Размер повреждения:", fNorm);//
        val3 = ado->readStr("Размер повреждения");
        row10T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 11
        duckx::TableRow row11T2 = table2.insert_row();
        row11T2.insert_cell(L"12.", fNorm);
        row11T2.insert_cell(L"Причины нарушения технические:", fNorm);//
        val3 = ado->readStr("Причины нарушения (технические)");
        row11T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 12
        duckx::TableRow row12T2 = table2.insert_row();
        row12T2.insert_cell(L"13.", fNorm);
        row12T2.insert_cell(L"Причины нарушения организационные:", fNorm);//
        val3 = ado->readStr("Причины нарушения (организационные)");
        row12T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 13
        duckx::TableRow row13T2 = table2.insert_row();
        row13T2.insert_cell(L"14.", fNorm);
        row13T2.insert_cell(L"Сопутствующие причины:", fNorm);//
        val3 = ado->readStr("Сопутствующие причины");
        row13T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 14
        duckx::TableRow row14T2 = table2.insert_row();
        row14T2.insert_cell(L"15.", fNorm);
        row14T2.insert_cell(L"Состояние конструкций канала/камеры:", fNorm);//
        val3 = ado->readStr("Состояние конструкции камеры");
        row14T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 15
        duckx::TableRow row15T2 = table2.insert_row();
        row15T2.insert_cell(L"16.", fNorm);
        row15T2.insert_cell(L"Состояние теплоизоляции теплопроводов:", fNorm);//
        val3 = ado->readStr("Состояние теплоизоляции теплопроводов");
        row15T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 16
        duckx::TableRow row16T2 = table2.insert_row();
        row16T2.insert_cell(L"17.", fNorm);
        row16T2.insert_cell(L"Состояние наружной поверхности теплопроводов:", fNorm);//
        val3 = ado->readStr("Состояние наружной поверхности теплопроводов");
        row16T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 17
        duckx::TableRow row17T2 = table2.insert_row();
        row17T2.insert_cell(L"18.", fNorm);
        row17T2.insert_cell(L"Способ ликвидации нарушения:", fNorm);//
        val3 = ado->readStr("Способ ликвидации нарушения");
        row17T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 18
        duckx::TableRow row18T2 = table2.insert_row();
        row18T2.insert_cell(L"19.", fNorm);
        row18T2.insert_cell(L"Количество отключенных потребителей:", fNorm);//
        val3 = ado->readStr("Количество отключенных потребителей");
        row18T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 19
        duckx::TableRow row19T2 = table2.insert_row();
        row19T2.insert_cell(L"20.", fNorm);
//        wnd->GetNameValue("data_nachala_remonta", val1);
        val1 = ado->readStr("Дата начала ремонтных работ");

//        wnd->GetNameValue("vremiaNachalaRemonta", val2);
        val2 = ado->readStr("Время начала ремонтных работ");

        row19T2.insert_cell(L"Время начала/завершение ремонтных работ ", fNorm);//
        val3 = "Начало: " + val1 + " " + val2;
        row19T2.insert_cell(val3.AllocSysString(), fNorm);//
        
//        wnd->GetNameValue("data_zaversheniya_remonta", val1);
        val1 = ado->readStr("Дата завершения ремонтных работ");


//        wnd->GetNameValue("vremiaZaversheniaRemonta", val2);
        val2 = ado->readStr("Время завершения  ремонтных работ");


        val3 = "Завершение: " + val1 + " " + val2;
        row19T2.insert_cell(val3.AllocSysString(), fNorm, 3);//

        //row 20
        duckx::TableRow row20T2 = table2.insert_row();
        row20T2.insert_cell(L"21.", fNorm);
        row20T2.insert_cell(L"Количество недоотпущенной тепловой энергии, Гкал:", fNorm);//
        val3 = ado->readStr("Количество недоотпущенной тепловой энергии");

        row20T2.insert_cell(val3.AllocSysString(), fNorm, 3);

        //row 21
        duckx::TableRow row21T2 = table2.insert_row();
        row21T2.insert_cell(L"22.", fNorm);
        row21T2.insert_cell(L"Затраты на восстановление, тг:", fNorm);//
        val1 = ado->readStr("Затраты на восстановление");
        row21T2.insert_cell(val1.AllocSysString(), fNorm, 3);

        //row 22
        duckx::TableRow row22T2 = table2.insert_row();
        row22T2.insert_cell(L"23.", fNorm);
        row22T2.insert_cell(L"Иные социальные последствия:", fNorm);//
        val3 = ado->readStr("Иные социальные факторы");
        row22T2.insert_cell(val3.AllocSysString(), fNorm, 3);
        //Table3
       
        duckx::formatting_flag fNorm2 = duckx::align_left | duckx::size_12 | duckx::tnr_style;
        doc.paragraphs().insert_paragraph(L"", fNorm2);
        doc.paragraphs().insert_paragraph(L"Комиссия расследования", fNorm2);
        doc.paragraphs().insert_paragraph(L"", fNorm2);
        duckx::Table table3 = doc.tables().insert_table(duckx::align_center);
        
        
        duckx::TableRow row1T3 = table3.insert_row();
//        val = ado->readStr("Должность председателя");
        val = ado->readStr("dolzhnost_predsedatelya");
        row1T3.insert_cell(val.AllocSysString(), fNorm2);
        row1T3.insert_cell(L"_____________________", fNorm2);
//        wnd->GetNameValue("predsedatel", val);
        val = ado->readStr("predsedatel");


        row1T3.insert_cell(val.AllocSysString(), fNorm2);
        duckx::TableRow row2T3 = table3.insert_row();
        row2T3.insert_cell(L"", fNorm2);
        row2T3.insert_cell(L"(подпись)", fNorm2);
        row2T3.insert_cell(L"", fNorm2);

       
        
        duckx::TableRow row3T3 = table3.insert_row();
        val = ado->readStr("Должность члена 1");
        row3T3.insert_cell(val.AllocSysString(), fNorm2);
        row3T3.insert_cell(L"_____________________", fNorm2);
//        wnd->GetNameValue("chlen_tn_1", val);
        val = ado->readStr("chlen_tn_1");
        row3T3.insert_cell(val.AllocSysString(), fNorm2);

        duckx::TableRow row4T3 = table3.insert_row();
        row4T3.insert_cell(L"", fNorm2);
        row4T3.insert_cell(L"(подпись)", fNorm2);
        row4T3.insert_cell(L"", fNorm2);



        duckx::TableRow row5T3 = table3.insert_row();
        val = ado->readStr("Должность члена 2");
        row5T3.insert_cell(val.AllocSysString(), fNorm2);
        row5T3.insert_cell(L"_____________________", fNorm2);
//        wnd->GetNameValue("chlen_tn_2", val);
        val = ado->readStr("chlen_tn_2");
        row5T3.insert_cell(val.AllocSysString(), fNorm2);
        duckx::TableRow row6T3 = table3.insert_row();
        row6T3.insert_cell(L"", fNorm2);
        row6T3.insert_cell(L"(подпись)", fNorm2);
        row6T3.insert_cell(L"", fNorm2);

        duckx::TableRow row7T3 = table3.insert_row();
        val = ado->readStr("Должность члена 3");
        row7T3.insert_cell(val.AllocSysString(), fNorm2);
        row7T3.insert_cell(L"_____________________", fNorm2);
//        wnd->GetNameValue("chlen_tn_3", val);
        val = ado->readStr("chlen_tn_3");
        row7T3.insert_cell(val.AllocSysString(), fNorm2);
        duckx::TableRow row8T3 = table3.insert_row();
        row8T3.insert_cell(L"", fNorm2);
        row8T3.insert_cell(L"(подпись)", fNorm2);
        row8T3.insert_cell(L"", fNorm2);
        
        doc.paragraphs().insert_paragraph(L"", fNorm2);

//        doc.paragraphs().insert_paragraph(L"Акт составлен \"____\" _________ 20____ года.", fNorm2);
        
        doc.paragraphs().insert_paragraph(L"", fNorm2);
        doc.save();

        HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open",  doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
    }
}