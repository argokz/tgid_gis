#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>

#include <property/PropertyDial.h>
#include <db/db.h>
#include <fstream>

#include <lib/DuckX/src/duckx.hpp>

//static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
//using pugi::xml_node;
//bool checkFileOpen(QString doc_name);

//BSTR COleVariant v.toString().toStdWString().c_str();

//double getDouble(COleVariant v);

duckx::Document * open_docx(QString doc_name);
void show_docx(QWidget* wnd, duckx::Document *doc);
bool CopyFile(QString fn1, QString fn2, bool bFailIfExists);
bool checkFileOpen(QString doc_name);


void KartaPovrezhdaemosti2(QWidget* wnd, QSqlDatabase &db, int id)
{
//    QString doc_name = QString("%1/%2").arg(QDir::tempPath(), QObject::tr("Карта нарушения.docx"));
    QString doc_name = QString("%1/%2").arg(QDir::tempPath(), QObject::tr("defect_map.docx"));


//    if (!checkFileOpen(doc_name))
//        return;

    QString fname = QString("%1/excel2/docx/empty_doc.docx").arg(argpath());

    CopyFile(fname, doc_name, 0);


    QString nomer_akta = "", data_sostavleniya_akta = "";
//    wnd->GetNameValue("nomer_akta", nomer_akta);
//    wnd->GetNameValue("data_sostavleniya_akta", data_sostavleniya_akta);


    QString q, fStr;

//    fStr.Format("%ssql/docxsql/map_defects.sql", argpath());
    fStr = QString("sql/objects/defect/defects_all.sql");
//    ifstream f(fStr);
//    if (f.good())
//    {
        q = readFile(fStr);
        QString myId;

//        myId.Format("%d", id);
//        q.Replace("$fileID$", myId);

//        myId.Format("%d", id);
        myId = QString("d.id=%1 AND ").arg(id);
//        q.Replace("$fileID$",myId);

        q.replace("$state_cond$", myId);
        q.replace("$season_condition$", " IS NULL OR 1=1 ");
        q.replace("$and_condition2$", "");


//    CInputDialog inD(AfxGetMainWnd(), "", "?", q);
//    inD.DoModal();


        QSqlQuery query(db);
        query.setForwardOnly(true);

        bool ret = query_exec(db, query, q);

        if (!ret)
        {
            QMessageBox::warning(0, "", "Ошибка в запросе");
            return;
        }

        nomer_akta = query.value("Номер акта").toString();
        data_sostavleniya_akta = query.value("Дата составления акта").toString();

        duckx::Document *doc = open_docx(doc_name);
        if (!doc) return;

        QVariant v;
        QString var;

        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_11 | duckx::tnr_style;
        duckx::formatting_flag fNormCenter = duckx::align_center | duckx::size_11 | duckx::tnr_style;

        doc->paragraphs().insert_paragraph(L"КАРТА НАРУШЕНИЯ", fBold);
        doc->paragraphs().insert_paragraph(L"Заявка №________", fBoldRight);//
        doc->paragraphs().insert_paragraph(L"Дата заявки №________", fBoldRight);
        duckx::Table table1 = doc->tables().insert_table(duckx::align_center | duckx::border);






        //row1
        duckx::TableRow rowT1 = table1.insert_row();
        v = query.value("Дата обнаружения нарушения");
        rowT1.insert_cell(L"1. Дата обнаружения повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Район эксплуатации");
        rowT1.insert_cell(L"2. Район:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Наименование участка");
        rowT1.insert_cell(L"3. Фрагмент тепловой сети:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        //row2
        duckx::TableRow row2T1 = table1.insert_row();
        v = query.value("Признак сети");
        row2T1.insert_cell(L"4. Признак т/сети:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Наименование начального узла");
        row2T1.insert_cell(L"5. Начальная камера (точка) участка повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Наименование конечного узла");
        row2T1.insert_cell(L"6. Конечная камера (точка) участка повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        
        //row3
        duckx::TableRow row3T1 = table1.insert_row();
        v = query.value("Расстояние до нарушения от ближайшей камеры, м");
        var = QString("%1 м").arg(v.toInt());
        row3T1.insert_cell(L"7. Расстояние до повреждения от начальной камеры:", fBoldLeft, 4).paragraphs().insert_paragraph(var.toStdWString().c_str(), fNormLeft);
        v = query.value("Тип прокладки");
        row3T1.insert_cell(L"8. Тип прокладки тр-да в месте повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Поверхность в месте раскопки");
        row3T1.insert_cell(L"9. Поверхность в месте раскопки:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        //row4
        duckx::TableRow row4T1 = table1.insert_row();
        v = query.value("Поврежденный трубопровод");
        row4T1.insert_cell(L"10. Поврежденный трубопровод:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Диаметр наружный, мм");
        var = QString("%1 мм").arg(v.toDouble());
        duckx::TableCell tC1 = row4T1.insert_cell(L"11. Наружный диаметр:", fBoldLeft, 4);
        tC1.paragraphs().insert_paragraph(var.toStdWString().c_str(), fNormLeft);
        tC1.paragraphs().insert_paragraph(L"Толщина стенки", fNormLeft);
        v = query.value("Толщина стенки, мм");
        var = QString("%1 мм").arg(v.toDouble());
        tC1.paragraphs().insert_paragraph(var.toStdWString().c_str(), fNormLeft);
        v = query.value("Режим");
        row4T1.insert_cell(L"12. Режим:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        //row5
        duckx::TableRow row5T1 = table1.insert_row();
        v = query.value("Дренируемый трубопровод");
        row5T1.insert_cell(L"13. Дренируемый трубопровод:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Поврежденный элемент");
        row5T1.insert_cell(L"14. Поврежденный элемент трубопровода:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Характер повреждения");
        row5T1.insert_cell(L"15. Тип повреждения:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);


        //row6
        duckx::TableRow row6T1 = table1.insert_row();
        v = query.value("Место расположения центра повреждения на трубопроводе, часов");
        row6T1.insert_cell(L"16. Место расположения центра повреждения на трубопроводе:", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        
        tC1 = row6T1.insert_cell(L"17. Размер повреждения:", fBoldLeft, 4);
        v = query.value("Ширина повреждения");
        var = QString("ширина %1 мм").arg(v.toDouble());
        tC1.paragraphs().insert_paragraph(var.toStdWString().c_str(), fNormLeft);
        v = query.value("Высота повреждения");
        var = QString("высота %1 мм").arg(v.toDouble());
        tC1.paragraphs().insert_paragraph(var.toStdWString().c_str(), fNormLeft);
        v = query.value("Площадь повреждения");
        var = QString("площадь %1 кв.мм").arg(v.toDouble());
        tC1.paragraphs().insert_paragraph(var.toStdWString().c_str(), fNormLeft);
        v = query.value("Причины нарушения (технические)");
        row6T1.insert_cell(L"18. Причины повреждения (техническая):", fBoldLeft, 4).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        //row7
        duckx::TableRow row7T1 = table1.insert_row();
        v = query.value("Причины нарушения (организационные)");
        row7T1.insert_cell(L"19. Организационные причины:", fBoldLeft, 6).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Сопутствующие причины");
        row7T1.insert_cell(L"20. Сопутствующие причины:", fBoldLeft, 6).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        //row8
        duckx::TableRow row8T1 = table1.insert_row();
        row8T1.insert_cell(L"Описание ремонтных работ:", fBold, 12);

        //row9
        duckx::TableRow row9T1 = table1.insert_row();
        v = query.value("Ремонт трубопровода и элементов");
        row9T1.insert_cell(L"21. Ремонт трубопровода и элементов:", fBoldLeft, 12).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        //row10
        duckx::TableRow row10T1 = table1.insert_row();
        v = query.value("Размер заплатки");
        tC1 = row10T1.insert_cell(L"22. Размер заплатки", fBoldLeft, 12);
        tC1.paragraphs().add_run(v.toString().toStdWString().c_str(), fBoldLeft);

        v = query.value("Длина заменённой трубы, м");
        var = QString(". Длина замененного уч-ка: %1 м").arg(v.toDouble());
        tC1.paragraphs().add_run(var.toStdWString().c_str(), fBoldLeft);

        //row11
        duckx::TableRow row11T1 = table1.insert_row();
        v = query.value("Ремонт изоляционной конструкции");
        row11T1.insert_cell(L"23. Ремонт изоляционной конструкции тр-да:", fBoldLeft, 12).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        
        //row12
        duckx::TableRow row12T1 = table1.insert_row();
        v = query.value("Ремонт канала");
        row12T1.insert_cell(L"24. Ремонт канала:", fBoldLeft, 12).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        //row13
        duckx::TableRow row13T1 = table1.insert_row();
        row13T1.insert_cell(L"Дополнительные сведения (к описанию повреждения):", fBold, 12);

        //row14
        duckx::TableRow row14T1 = table1.insert_row();
        
        tC1 = row14T1.insert_cell(L"25. Состояние конструкций канала (камеры):", fBoldLeft, 12);
        v = query.value("Состояние конструкции канала");
        tC1.paragraphs().insert_paragraph(L"Состояние конструкции канала:", fNormLeft);
        tC1.paragraphs().add_run(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Состояние конструкции камеры");
        tC1.paragraphs().insert_paragraph(L"Состояние конструкции камеры:", fNormLeft);
        tC1.paragraphs().add_run(v.toString().toStdWString().c_str(), fNormLeft);

        
        //row15
        duckx::TableRow row15T1 = table1.insert_row();
        v = query.value("Состояние теплоизоляции теплопроводов");
        row15T1.insert_cell(L"Состояние теплоизоляции теплопроводов (в целом на участке):", fBold, 12).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);;
        
        /*
        //row16
        duckx::TableRow row16T1 = table1.insert_row();
        v = query.value("Дата обнаружения повреждения");
        row16T1.insert_cell(L"25. По подающему тр-ду:", fBoldLeft).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNorm);
        v = query.value("Район");
        row16T1.insert_cell(L"26. По обратному тр-ду:", fBoldLeft).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNorm);
        */

        //row16
        duckx::TableRow row16T1 = table1.insert_row();
        v = query.value("Состояние наружной поверхности теплопроводов");
        row16T1.insert_cell(L"Состояние наружной поверхности теплопроводов (в месте осмотра):", fBold, 12).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);;


        //row17
        duckx::TableRow row17T1 = table1.insert_row();
        v = query.value("Состояние внутренней поверхности поврежденной трубы");
        row17T1.insert_cell(L"Состояние внутренней поверхности поврежденной трубы:", fBold, 12).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);;

        //row18
        duckx::TableRow row18T1 = table1.insert_row();
       
        tC1 = row18T1.insert_cell(L"26. Начальная камера (точка) участка отключения от ", fBoldLeft, 12);
        v = query.value("Начальная камера отключения от сети");
        tC1.paragraphs().add_run(v.toString().toStdWString().c_str(), fBoldLeft);
        tC1.paragraphs().insert_paragraph(L"Конечная камера (точка) участка до", fBoldLeft);
        v = query.value("Конечная камера отключения от сети");
        tC1.paragraphs().add_run(v.toString().toStdWString().c_str(), fBoldLeft);

        //row19
        duckx::TableRow row19T1 = table1.insert_row();
        v = query.value("Начала ремонта");
        row19T1.insert_cell(L"27. Начала ремонта:", fBoldLeft, 3).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Окончание ремонта");
        row19T1.insert_cell(L"28. Окончание ремонта:", fBoldLeft, 3).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        v = query.value("Трудозатраты на ремонт");
        row19T1.insert_cell(L"29. Трудозатраты на ремонт:", fBoldLeft, 3).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Стоимость ремонтных работ, тг");
        row19T1.insert_cell(L"30. Стоимость ремонтных работ:", fBoldLeft, 3).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);

        duckx::TableRow row19_1T1 = table1.insert_row();
        v = query.value("Причины нарушения (организационные)");
        row19_1T1.insert_cell(L"31. Вид ремонта:", fBoldLeft,12).paragraphs().insert_paragraph(v.toString().toStdWString().c_str(), fNormLeft);


        //row20
        duckx::TableRow row20T1 = table1.insert_row();
        tC1 = row20T1.insert_cell(L"Примечание. Схема. Пояснения", fBold, 12);
        for (int i = 0; i < 24; i++) {
            tC1.paragraphs().insert_paragraph(L"", fNormLeft);
        }

        doc->paragraphs().insert_paragraph(L"Карту заполнил", fNormCenter);
        duckx::Table table2 = doc->tables().insert_table(duckx::align_center);
        duckx::TableRow row1T2 = table2.insert_row();
        v = query.value("Должность");
        row1T2.insert_cell(L"Должность ", fNormLeft).paragraphs().add_run(v.toString().toStdWString().c_str(), fNormLeft);
        v = query.value("Начальник участка");
        row1T2.insert_cell(L"Ф.И.О. ", fNormLeft).paragraphs().add_run(v.toString().toStdWString().c_str(), fNormLeft);
        duckx::TableRow row2T2 = table2.insert_row();
        row2T2.insert_cell(L"«____»________________________г.", fNormLeft);
        row2T2.insert_cell(L"Подпись_______________________", fNormLeft);
        doc->save();

//        HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);

        show_docx(wnd, doc);

        return;
       
    //}
}
