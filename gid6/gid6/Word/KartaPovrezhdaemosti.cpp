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


#include <duckx.hpp>


static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
using pugi::xml_node;
bool checkFileOpen(CString doc_name);

BSTR getWideString(COleVariant v) {
    return CCrack::strVARIANT_UTF(v).AllocSysString();
}

double getDouble(COleVariant v) {
    if (v.vt == 1)
        return 0.0;
    return v.dblVal;
}

#include "PropertyGrid\test\PropGridDlg.h"
void KartaPovrezhdaemosti(CPropertyGrid* wnd, int id) {

    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Карта повреждаемости.docx"));
    CString fname;

    if (!checkFileOpen(doc_name))
        return;

    fname.Format("%s\\excel2\\docx\\empty_doc.docx", argpath());

    CopyFile(fname, doc_name, 0);


    CString nomer_akta, data_sostavleniya_akta;
    wnd->GetNameValue("nomer_akta", nomer_akta);
    wnd->GetNameValue("data_sostavleniya_akta", data_sostavleniya_akta);


    CString q, fStr;

    CAdoFile* ado = getAdo(getPsAdoName());
    fStr.Format("%ssql\\docxsql\\map_defects.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%d", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }




        duckx::Document doc(doc_name.GetString());

        doc.open();
        COleVariant v;
        CString var;

        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormCenter = duckx::align_center | duckx::size_11 | duckx::tnr_style;

        doc.paragraphs().insert_paragraph(L"КАРТА НАРУШЕНИЯ", fBold);
        doc.paragraphs().insert_paragraph(L"Заявка №________", fBoldRight);//
        doc.paragraphs().insert_paragraph(L"Дата заявки №________", fBoldRight);
        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);

        //row1
        duckx::TableRow rowT1 = table1.insert_row();
        v = ado->read("Дата обнаружения повреждения");
        rowT1.insert_cell(L"1. Дата обнаружения повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Район");
        rowT1.insert_cell(L"2. Район:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("№ т/м присоединения");
        rowT1.insert_cell(L"3. Фрагмент тепловой сети:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        //row2
        duckx::TableRow row2T1 = table1.insert_row();
        v = ado->read("Признак сети");
        row2T1.insert_cell(L"4. Признак т/сети:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Начальная точка");
        row2T1.insert_cell(L"5. Начальная камера (точка) участка повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Конечная точка");
        row2T1.insert_cell(L"6. Конечная камера (точка) участка повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        
        //row3
        duckx::TableRow row3T1 = table1.insert_row();
        v = ado->read("Расстояние до нарушения от ближайшей камеры, м");
//        var.Format("%d м", v.intVal);
        var.Format("%.2f м", v.dblVal);
        row3T1.insert_cell(L"7. Расстояние до повреждения от начальной камеры:", fBoldLeft, 4).paragraphs().insert_paragraph(var.AllocSysString(), fNormLeft);
        v = ado->read("Тип прокладки");
        row3T1.insert_cell(L"8. Тип прокладки тр-да в месте повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Поверхность в месте раскопки");
        row3T1.insert_cell(L"9. Поверхность в месте раскопки:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        //row4
        duckx::TableRow row4T1 = table1.insert_row();
        v = ado->read("Поврежденный трубопровод");
        row4T1.insert_cell(L"10. Поврежденный трубопровод:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Диаметр наружний");
        var.Format("%.2f мм", v.dblVal);
        duckx::TableCell tC1 = row4T1.insert_cell(L"11. Наружный диаметр:", fBoldLeft, 4);
        tC1.paragraphs().insert_paragraph(var.AllocSysString(), fNormLeft);
        tC1.paragraphs().insert_paragraph(L"Толщина стенки", fNormLeft);
        v = ado->read("Толщина стенки");
        var.Format("%.2f мм", v.dblVal);
        tC1.paragraphs().insert_paragraph(var.AllocSysString(), fNormLeft);
        v = ado->read("Режим");
        row4T1.insert_cell(L"12. Режим:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        //row5
        duckx::TableRow row5T1 = table1.insert_row();
        v = ado->read("Дренируемый трубопровод");
        row5T1.insert_cell(L"13. Дренируемый трубопровод:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Поврежденный элемент");
        row5T1.insert_cell(L"14. Поврежденный элемент трубопровода:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Тип повреждения");
        row5T1.insert_cell(L"15. Тип повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);


        //row6
        duckx::TableRow row6T1 = table1.insert_row();
        v = ado->read("Место расположения центра повреждения на трубопроводе,  часов");
        row6T1.insert_cell(L"16. Место расположения центра повреждения на трубопроводе:", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        
        tC1 = row6T1.insert_cell(L"17. Размер повреждения:", fBoldLeft, 4);
        v = ado->read("Ширина повреждения");
        var.Format("ширина %.2f мм", v.dblVal);
        tC1.paragraphs().insert_paragraph(var.AllocSysString(), fNormLeft);
        v = ado->read("Высота повреждения");
        var.Format("высота %.2f мм", v.dblVal);
        tC1.paragraphs().insert_paragraph(var.AllocSysString(), fNormLeft);
        v = ado->read("Площадь повреждения");
        var.Format("площадь %.2f кв.мм", v.dblVal);
        tC1.paragraphs().insert_paragraph(var.AllocSysString(), fNormLeft);
        v = ado->read("Причины повреждения");
        row6T1.insert_cell(L"18. Причины повреждения (техническая):", fBoldLeft, 4).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        //row7
        duckx::TableRow row7T1 = table1.insert_row();
        v = ado->read("Организационные причины");
        row7T1.insert_cell(L"19. Организационные причины:", fBoldLeft, 6).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Сопутствующие причины");
        row7T1.insert_cell(L"20. Сопутствующие причины:", fBoldLeft, 6).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        //row8
        duckx::TableRow row8T1 = table1.insert_row();
        row8T1.insert_cell(L"Описание ремонтных работ:", fBold, 12);

        //row9
        duckx::TableRow row9T1 = table1.insert_row();
        v = ado->read("Ремонт трубопровода и элементов");
        row9T1.insert_cell(L"21. Ремонт трубопровода и элементов:", fBoldLeft, 12).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        //row10
        duckx::TableRow row10T1 = table1.insert_row();
        v = ado->read("Размер заплатки");
        tC1 = row10T1.insert_cell(L"22. Размер заплатки", fBoldLeft, 12);
        tC1.paragraphs().add_run(getWideString(v), fBoldLeft);

        v = ado->read("Длина заменённой трубы, м");
        var.Format(". Длина замененного уч-ка: %.2f м", getDouble(v));
        tC1.paragraphs().add_run(var.AllocSysString(), fBoldLeft);

        //row11
        duckx::TableRow row11T1 = table1.insert_row();
        v = ado->read("Ремонт изоляционной конструкции");
        row11T1.insert_cell(L"23. Ремонт изоляционной конструкции тр-да:", fBoldLeft, 12).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        
        //row12
        duckx::TableRow row12T1 = table1.insert_row();
        v = ado->read("Ремонт канала");
        row12T1.insert_cell(L"24. Ремонт канала:", fBoldLeft, 12).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        //row13
        duckx::TableRow row13T1 = table1.insert_row();
        row13T1.insert_cell(L"Дополнительные сведения (к описанию повреждения):", fBold, 12);

        //row14
        duckx::TableRow row14T1 = table1.insert_row();
        
        tC1 = row14T1.insert_cell(L"25. Состояние конструкций канала (камеры):", fBoldLeft, 12);
        v = ado->read("Состояние конструкции канала");
        tC1.paragraphs().insert_paragraph(L"Состояние конструкции канала:", fNormLeft);
        tC1.paragraphs().add_run(getWideString(v), fNormLeft);
        v = ado->read("Состояние конструкции камеры");
        tC1.paragraphs().insert_paragraph(L"Состояние конструкции камеры:", fNormLeft);
        tC1.paragraphs().add_run(getWideString(v), fNormLeft);

        
        //row15
        duckx::TableRow row15T1 = table1.insert_row();
        v = ado->read("Состояние теплоизоляции теплопроводов");
        row15T1.insert_cell(L"Состояние теплоизоляции теплопроводов (в целом на участке):", fBold, 12).paragraphs().insert_paragraph(getWideString(v), fNormLeft);;
        
        /*
        //row16
        duckx::TableRow row16T1 = table1.insert_row();
        v = ado->read("Дата обнаружения повреждения");
        row16T1.insert_cell(L"25. По подающему тр-ду:", fBoldLeft).paragraphs().insert_paragraph(getWideString(v), fNorm);
        v = ado->read("Район");
        row16T1.insert_cell(L"26. По обратному тр-ду:", fBoldLeft).paragraphs().insert_paragraph(getWideString(v), fNorm);
        */

        //row16
        duckx::TableRow row16T1 = table1.insert_row();
        v = ado->read("Состояние наружной поверхности теплопроводов");
        row16T1.insert_cell(L"Состояние наружной поверхности теплопроводов (в месте осмотра):", fBold, 12).paragraphs().insert_paragraph(getWideString(v), fNormLeft);;


        //row17
        duckx::TableRow row17T1 = table1.insert_row();
        v = ado->read("Состояние внутренней поверхности поврежденной трубы");
        row17T1.insert_cell(L"Состояние внутренней поверхности поврежденной трубы:", fBold, 12).paragraphs().insert_paragraph(getWideString(v), fNormLeft);;

        //row18
        duckx::TableRow row18T1 = table1.insert_row();
       
        tC1 = row18T1.insert_cell(L"26. Начальная камера (точка) участка отключения от ", fBoldLeft, 12);
        v = ado->read("Начальная камера отключения от сети");
        tC1.paragraphs().add_run(getWideString(v), fBoldLeft);
        tC1.paragraphs().insert_paragraph(L"Конечная камера (точка) участка до", fBoldLeft);
        v = ado->read("Конечная камера отключения от сети");
        tC1.paragraphs().add_run(getWideString(v), fBoldLeft);

        //row19
        duckx::TableRow row19T1 = table1.insert_row();
        v = ado->read("Начала ремонта");
        row19T1.insert_cell(L"27. Начала ремонта:", fBoldLeft, 3).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Окончание ремонта");
        row19T1.insert_cell(L"28. Окончание ремонта:", fBoldLeft, 3).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        v = ado->read("Трудозатраты на ремонт");
        row19T1.insert_cell(L"29. Трудозатраты на ремонт:", fBoldLeft, 3).paragraphs().insert_paragraph(getWideString(v), fNormLeft);
        v = ado->read("Стоимость ремонтных работ, тг");
        row19T1.insert_cell(L"30. Стоимость ремонтных работ:", fBoldLeft, 3).paragraphs().insert_paragraph(getWideString(v), fNormLeft);

        duckx::TableRow row19_1T1 = table1.insert_row();
        v = ado->read("Организационные причины");
        row19_1T1.insert_cell(L"31. Вид ремонта:", fBoldLeft,12).paragraphs().insert_paragraph(getWideString(v), fNormLeft);


        //row20
        duckx::TableRow row20T1 = table1.insert_row();
        tC1 = row20T1.insert_cell(L"Примечание. Схема. Пояснения", fBold, 12);
        for (int i = 0; i < 24; i++) {
            tC1.paragraphs().insert_paragraph(L"", fNormLeft);
        }

        doc.paragraphs().insert_paragraph(L"Карту заполнил", fNormCenter);
        duckx::Table table2 = doc.tables().insert_table(duckx::align_center);
        duckx::TableRow row1T2 = table2.insert_row();
        v = ado->read("Должность");
        row1T2.insert_cell(L"Должность ", fNormLeft).paragraphs().add_run(getWideString(v), fNormLeft);
        v = ado->read("Начальник участка");
        row1T2.insert_cell(L"Ф.И.О. ", fNormLeft).paragraphs().add_run(getWideString(v), fNormLeft);
        duckx::TableRow row2T2 = table2.insert_row();
        row2T2.insert_cell(L"«____»________________________г.", fNormLeft);
        row2T2.insert_cell(L"Подпись_______________________", fNormLeft);
        doc.save();

        HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
        return;
       
    }
}