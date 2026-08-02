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

BSTR getWideString(COleVariant v);

void addParagraph(duckx::Paragraph& ps, BSTR main, vector<BSTR> stringArray, duckx::formatting_flag f) {
    duckx::Paragraph p = ps.insert_paragraph(main, f);
    for (int i = 0; i < stringArray.size(); i++) {
        p.add_run(stringArray[i], f);
    }
};

void addParagraph2(duckx::Paragraph& ps, BSTR main, vector<pair<BSTR, bool>> stringArray, duckx::formatting_flag f) {
    duckx::Paragraph p = ps.insert_paragraph(main, f);
    for (int i = 0; i < stringArray.size(); i++) {
        if (stringArray[i].second)
            p.add_run(stringArray[i].first, f | duckx::bold);
        else
            p.add_run(stringArray[i].first, f);
    }
};
bool checkFileOpen(CString doc_name);
#include "PropertyGrid\test\PropGridDlg.h"
void AktShurfovki(CPropertyGrid* wnd, int id) {

    COleVariant v;
    CString ss;
    CString doc_name;
    doc_name.Format("%s\\%s", getenv("TMP"), _TR("Акт шурфовки.docx"));
    CString fname;

    if (!checkFileOpen(doc_name))
        return;

    fname.Format("%s\\excel2\\docx\\empty_doc.docx", argpath());

    CopyFile(fname, doc_name, 0);

    CString q, fStr;

    CAdoFile* ado = getAdo(getPsAdoName());
    fStr.Format("%ssql\\docxsql\\act_shufovki.sql", argpath());
    ifstream f(fStr);
    if (f.good())
    {
        q = readFile(fStr);
        CString myId;

        myId.Format("%i", id);
        q.Replace("$fileID$", myId);
        bool ret = ado->openTable0(q);
        if (!ret)
        {
            AfxMessageBox("Ошибка в запросе");
            return;
        }

        duckx::Document doc(doc_name.GetString());

        doc.open();

        duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;
        duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_12 | duckx::tnr_style;
        duckx::formatting_flag fNorm = duckx::align_center | duckx::size_12 | duckx::tnr_style;
        duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;



        doc.paragraphs().insert_paragraph(L"Утверждаю", fNormRight);
        addParagraph(doc.paragraphs(), L"",
            { getWideString(ado->read("Должность утверждающего")),L", ", getWideString(ado->read("Служба утверждающего"))
            },
            fNormRight);
        doc.paragraphs().insert_paragraph(L"АО «Астана-Теплотранзит»", fNormRight);
        addParagraph(doc.paragraphs(), getWideString(ado->read("ФИО утверждающего")),
            {
                L" _____________________"
            },
            fNormRight);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        duckx::Paragraph p = doc.paragraphs().insert_paragraph(L"Район теплосети: ", fNormLeft);

        v = ado->read("Район эксплуатации");
        p.add_run(getWideString(v), fBoldLeft);
        p.add_run(L"; ", fBoldLeft);
        v = ado->read("Участок эксплуатации");
        CString area(v.bstrVal);
        int bIndex = area.Find("участок");
        if (bIndex != -1)
            area = area.Mid(0, bIndex + 7);
        p.add_run(L"Участок эксплуатации: ", fNormLeft);
        p.add_run(area.AllocSysString(), fBoldLeft);

        /*p.add_run(L" ТЭЦ ", fNormLeft);
        v = ado->read("Источник теплоснабжения");
        p.add_run(getWideString(v), fNormLeft);*/

        v = ado->read("Дата окончания шурфовки");
        doc.paragraphs().insert_paragraph(L"Дата: ", fNormLeft).add_run(getWideString(v), fBoldLeft);

        v = ado->read("Номер акта");
    
        doc.paragraphs().insert_paragraph(L"АКТ №", fBold).add_run(getWideString(v), fBold);
        doc.paragraphs().insert_paragraph(L"на осмотр  тепломагистрали в шурфе", fBold);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);

        addParagraph2(doc.paragraphs(), L"Наименование или № магистрали: ",
            { make_pair(getWideString(ado->read("Магистраль")),true)
            },
            fNormLeft);


        addParagraph2(doc.paragraphs(), L"Место шурфовки: между камерами (узлами): ",
            {
                make_pair(getWideString(ado->read("Наименование начального узла")), true),
                make_pair(SysAllocString(L" - "),true),
                make_pair(getWideString(ado->read("Наименование конечного узла")),true),
                make_pair(SysAllocString(L", "), true),
                make_pair(getWideString(ado->read("Признак участка трубопровода")),true)
            },
            fNormLeft);

        p = doc.paragraphs().insert_paragraph(L"на расстоянии ", fNormLeft);

        ss.Format("%.2f м ", ado->read("Расстояние до ближайшей камеры, м").dblVal);
        p.add_run(ss.AllocSysString(), fBoldLeft);
        p.add_run(L"от камеры (узла) ", fNormLeft);
        v = ado->read("Наименование ближайшей камеры");
        p.add_run(getWideString(v), fBoldLeft);

        v = ado->read("Длина осмотра, м");
        p = doc.paragraphs().insert_paragraph(L"на длине ", fNormLeft);
        ss.Format("%.2f м.", ado->read("Длина осмотра, м").dblVal);
        p.add_run(ss.AllocSysString(), fBoldLeft);


        COleDateTime dNow = COleDateTime::GetCurrentTime();
        COleDateTime d1 = ado->read_date("Дата первичного ввода в эксплуатацию");
        COleDateTime d2 = ado->read_date("Дата последней перекладки");
        COleDateTime d3;
        if (d2.m_dt == 0.0)
            d3 = d1;
        else
            d3 = d2;
        COleDateTimeSpan d4 = dNow - d3;
        int years = trunc(d4.GetTotalDays() / 365);
        p = doc.paragraphs().insert_paragraph(L"Год строительства/последней перекладки участка теплосети: ", fNormLeft);
        p.add_run(d3.Format(_T("%d.%m.%Y")).AllocSysString(), fBoldLeft);

        p = doc.paragraphs().insert_paragraph(L"Длительность эксплуатации: ", fNormLeft);
        ss.Format("%i", years);
        p.add_run(ss.AllocSysString(), fBoldLeft);
        p.add_run(L" лет.", fBoldLeft);

        p = doc.paragraphs().insert_paragraph(L"Тип прокладки: ", fNormLeft);
        p.add_run(getWideString(ado->read("Тип прокладки")), fBoldLeft);

        doc.paragraphs().insert_paragraph(L"", fNormLeft);


        int extSign;
        double diam = ado->read_double("Диаметр наружный, мм");
        ss = ado->readStr("Признак участка трубопровода");

        duckx::Paragraph pDiametr = doc.paragraphs().insert_paragraph(L"Диаметр труб: ");
        if (strcmp(ss, "общий") == 0)
        {
            extSign = 1;
            ss.Format("%.2f", diam);
            pDiametr.add_run(L"подающий ", fNormLeft);
            pDiametr.add_run(ss.AllocSysString(), fBoldLeft);
            pDiametr.add_run(L" мм", fBoldLeft);
            pDiametr.add_run(L", обратной ", fNormLeft);
            pDiametr.add_run(ss.AllocSysString(), fBoldLeft);
            pDiametr.add_run(L" мм", fBoldLeft);
            
        }
        else if (strcmp(ss, "подающий") == 0)
        {
            extSign = 2;
            ss.Format("%.2f", diam);
            pDiametr.add_run(L"подающий ", fNormLeft);
            pDiametr.add_run(ss.AllocSysString(), fBoldLeft);
            pDiametr.add_run(L" мм", fBoldLeft);
        }
        else
        {
            extSign = 3;
            ss.Format("%.2f", diam);
            pDiametr.add_run(L"обратной ", fNormLeft);
            pDiametr.add_run(ss.AllocSysString(), fBoldLeft);
            pDiametr.add_run(L" мм", fBoldLeft);
        }
        

        doc.paragraphs().insert_paragraph(L"", fNormLeft);

        doc.paragraphs().insert_paragraph(L"Результаты осмотра", fBold);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);

        addParagraph(doc.paragraphs(), L"1 Характеристика участка сети:", {}, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – назначение вскрытия: ",
            { make_pair(getWideString(ado->read("Назначение вскрытия")),true)
            },
            fNormLeft);
        addParagraph2(doc.paragraphs(), L" – место проведения (адрес) вскрытия: ",
            { make_pair(getWideString(ado->read("Адрес")),true)
            },
            fNormLeft);


        addParagraph(doc.paragraphs(), L"2 Характеристика наружного покрытия над прокладкой теплопровода", {}, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – прокладка трубопроводов под пешеходными зонами: ", { make_pair(getWideString(ado->read("Прокладка трубопроводов под пешеходными зонами")), true) }, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – прокладка трубопроводов под авто и ж/д дорогами: ", { make_pair(getWideString(ado->read("Прокладка трубопроводов под авто и ж/д дорогами")), true) }, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – прокладка трубопровода в местах массового скопления жителей города: ", { make_pair(getWideString(ado->read("Прокладка трубопровода в местах массового скопления жителей города")), true) }, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – поверхность над трассой: ", { make_pair(getWideString(ado->read("Поверхность над трассой")),true) }, fNormLeft);


        addParagraph(doc.paragraphs(), L"3 Характеристика грунта", {}, fNormLeft);
        addParagraph2(doc.paragraphs(), getWideString(ado->read("Грунт в месте прокладки")), {}, fBoldLeft);

        addParagraph(doc.paragraphs(), L"4 Уровень грунтовых вод ", {}, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – интенсивность подтопления грунтовыми водами: ", { make_pair(getWideString(ado->read("Интенсивность подтопления грунтовыми водами")), true) }, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – подтопление до трубы: ", { make_pair(getWideString(ado->read("Подтопление до трубы")), true) }, fNormLeft);

        ss.Format("%.2f м", ado->read("Глубина заложения прокладки, м").fltVal);
        addParagraph2(doc.paragraphs(), L"5 Глубина заложения прокладки: ", {
            make_pair(ss.AllocSysString(), true)
            }, fNormLeft);

        addParagraph(doc.paragraphs(), L"6 Наличие дренажного устройства, его конструкция, состояние и работоспособность ", {}, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – конструкция дренажного устройства: ", { make_pair(getWideString(ado->read("Конструкция дренажного устройства")), true) }, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – водоотведение на участке: ", { make_pair(getWideString(ado->read("Водоотведение на участке")), true) }, fNormLeft);

        addParagraph(doc.paragraphs(), L"7 Гидроизоляция канала ", {}, fNormLeft);

        addParagraph(doc.paragraphs(), L"8 Характеристика и состояние строительных конструкций ", {}, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – строительные конструкции канала: ", { make_pair(getWideString(ado->read("Строительные конструкции канала")), true) }, fNormLeft);
        addParagraph2(doc.paragraphs(), L" – состояние строительных конструкций канала: ", { make_pair(getWideString(ado->read("Состояние строительных конструкций канала")), true) }, fNormLeft);

        addParagraph2(doc.paragraphs(), L"9 Внутреннее состояние канала: ", {
            make_pair(getWideString(ado->read("Внутреннее состояние канала")), true)
            }, fNormLeft);

        addParagraph(doc.paragraphs(), L"10 Покровный слой (материалы, состояние): ", {}, fNormLeft);

        CString materialP, materialO, condP, condO;

        duckx::Table table1 = doc.tables().insert_table(duckx::align_left);
        duckx::TableRow row1 = table1.insert_row();
        row1.insert_cell(L"", fNormLeft);
        row1.insert_cell(L"подающая труба:", fNormLeft);
        row1.insert_cell(L"обратная труба:", fNormLeft);
        duckx::TableRow row2 = table1.insert_row();

        if (extSign == 1 || extSign == 2) {
            materialP = getWideString(ado->read("Материал наружного покрытия"));
            condP = getWideString(ado->read("Состояние наружного покрытия (подающий трубопровод)"));
        }
        if (extSign == 1 || extSign == 3) {
            materialO = getWideString(ado->read("Материал наружного покрытия"));
            condO = getWideString(ado->read("Состояние наружного покрытия (обратный трубопровод)"));
        }

        row2.insert_cell(L"– материал:\n", fNormLeft);
        row2.insert_cell(materialP.AllocSysString(), fNormLeft);
        row2.insert_cell(materialO.AllocSysString(), fNormLeft);

        table1.insert_row().insert_cell(L"", fNormLeft, 3);

        duckx::TableRow row3 = table1.insert_row();
        row3.insert_cell(L"– состояние:\n", fNormLeft);
        row3.insert_cell(condP.AllocSysString(), fNormLeft);
        row3.insert_cell(condO.AllocSysString(), fNormLeft);

        doc.paragraphs().insert_paragraph(L"");


        addParagraph(doc.paragraphs(), L"11 Тепловая изоляция (материалы, состояние):  ", {}, fNormLeft);

        materialP = "";
        materialO = "";
        condP = "";
        condO = "";

        table1 = doc.tables().insert_table(duckx::align_left);
        row1 = table1.insert_row();
        row1.insert_cell(L"", fNormLeft);
        row1.insert_cell(L"подающая труба:", fNormLeft);
        row1.insert_cell(L"обратная труба:", fNormLeft);

        
        if (extSign == 1 || extSign == 2) {
            
            materialP = CCrack::strVARIANT_UTF(ado->read("Материал тепловой изоляции")) + " " + CCrack::strVARIANT_UTF(ado->read("Тип изоляции ППУ"));
            condP = getWideString(ado->read("Состояние тепловой изоляции (подающий трубопровод)"));
        }
        if (extSign == 1 || extSign == 3) {
            materialO = CCrack::strVARIANT_UTF(ado->read("Материал тепловой изоляции")) + " " + CCrack::strVARIANT_UTF(ado->read("Тип изоляции ППУ"));
            condO = getWideString(ado->read("Состояние тепловой изоляции (обратный трубопровод)"));
        }

        row2 = table1.insert_row();
        row2.insert_cell(L"– материал:\n", fNormLeft);
        row2.insert_cell(materialP.AllocSysString(), fNormLeft);
        row2.insert_cell(materialO.AllocSysString(), fNormLeft);

        table1.insert_row().insert_cell(L"", fNormLeft, 3);

        row3 = table1.insert_row();
        row3.insert_cell(L"– состояние:\n", fNormLeft);
        row3.insert_cell(condP.AllocSysString(), fNormLeft);
        row3.insert_cell(condO.AllocSysString(), fNormLeft);

        doc.paragraphs().insert_paragraph(L"");

        

        addParagraph(doc.paragraphs(), L"12 Антикоррозионное покрытие труб, его состояние: ", {}, fNormLeft);

        materialP = "";
        materialO = "";
        condP = "";
        condO = "";

        table1 = doc.tables().insert_table(duckx::align_left);
        row1 = table1.insert_row();
        row1.insert_cell(L"", fNormLeft);
        row1.insert_cell(L"подающая труба:", fNormLeft);
        row1.insert_cell(L"обратная труба:", fNormLeft);


        if (extSign == 1 || extSign == 2) {
            materialP = getWideString(ado->read("Материал антикоррозийного покрытия"));
            condP = getWideString(ado->read("Состояние противокоррозионного покрытия (подающий трубопровод)"));
        }
        if (extSign == 1 || extSign == 3) {
            materialO = getWideString(ado->read("Материал антикоррозийного покрытия"));
            condO = getWideString(ado->read("Состояние противокоррозионного покрытия (обратный трубопровод)"));
        }

        row2 = table1.insert_row();
        row2.insert_cell(L"– материал:\n", fNormLeft);
        row2.insert_cell(materialP.AllocSysString(), fNormLeft);
        row2.insert_cell(materialO.AllocSysString(), fNormLeft);

        table1.insert_row().insert_cell(L"", fNormLeft, 3);

        row3 = table1.insert_row();
        row3.insert_cell(L"– состояние:\n", fNormLeft);
        row3.insert_cell(condP.AllocSysString(), fNormLeft);
        row3.insert_cell(condO.AllocSysString(), fNormLeft);

        doc.paragraphs().insert_paragraph(L"");


        addParagraph(doc.paragraphs(), L"13 Наличие наружной коррозии, ее характер и интенсивность, толщина коррозионной пленки, диаметр и глубина каверн, местонахождение по оси трубы: ", {}, fNormLeft);
        if (extSign == 1 || extSign == 2) {
            addParagraph2(doc.paragraphs(), L" – подающая труба: ", {
                make_pair(getWideString(ado->read("Наличие коррозии (подающий трубопровод)")), true)

                }, fNormLeft);

        }
        if (extSign == 1 || extSign == 3) {
            addParagraph2(doc.paragraphs(), L" – обратная труба: ", {
                make_pair(getWideString(ado->read("Наличие коррозии (обратный трубопровод)")), true)
                }, fNormLeft);
        }

        addParagraph(doc.paragraphs(), L"14. Результаты вырезки (коррозийного процесса):", {}, fNormLeft);

            addParagraph2(doc.paragraphs(), L" – состояние металла трубопровода: ", {
                make_pair(getWideString(ado->read("Состояние металла трубопровода")), true)
                }, fNormLeft);

            addParagraph2(doc.paragraphs(), L" – степень внешней коррозии: ", {
                make_pair(getWideString(ado->read("Степень внешней коррозии")), true)
                }, fNormLeft);

            addParagraph2(doc.paragraphs(), L" – степень внутренней коррозии: ", {
                make_pair(getWideString(ado->read("Степень внутренней коррозии")), true)
                }, fNormLeft);




/*
        addParagraph2(doc.paragraphs(), L"14 Наличие электрифицированного транспорта и расстояние до ближайших рельсов: ", {
             make_pair(getWideString(ado->read("Наличие электрифицированного транспорта на расстоянии")), true)
            }, fNormLeft);
*/

        addParagraph2(doc.paragraphs(), L"15 Наличие электрифицированного транспорта и расстояние до ближайших рельсов: ", {
             make_pair(getWideString(ado->read("Наличие электрифицированного транспорта на расстоянии")), true)
            }, fNormLeft);

        addParagraph2(doc.paragraphs(), L"16 Наличие вблизи теплотрассы других подземных коммуникаций (кабель, газопровод, канализации): ", {
            make_pair(getWideString(ado->read("Вблизи лежащие коммуникации")), true)
            }, fNormLeft);

        addParagraph2(doc.paragraphs(), L"17 Наличие на смежных подземных коммуникациях электрозащитных установок: ", {
            make_pair(getWideString(ado->read("Наличие эл.коммуникаций, электрохимзащиты газопровода")), true)
            }, fNormLeft);

        addParagraph2(doc.paragraphs(), L"18 Предполагаемые причины нарушения теплоизоляционной конструкции и наружной коррозии трубопроводов: ", {
            make_pair(getWideString(ado->read("Предполагаемые причины разрушения изоляции, коррозии")), true)
            }, fNormLeft);

        addParagraph2(doc.paragraphs(), L"19 Намечаемые мероприятия по устранению причин дефектов: ", {
            make_pair(getWideString(ado->read("Намеченные мероприятия")), true)
            }, fNormLeft);

        addParagraph2(doc.paragraphs(), L"20 Описание работ по восстановлению прокладки в месте шурфовки; дата восстановления: ", {
            make_pair(getWideString(ado->read("Мероприятия по восстановлению прокладки")), true)
            }, fNormLeft);

        addParagraph2(doc.paragraphs(), L"21 Дополнительные данные: ", {
            make_pair(getWideString(ado->read("Результаты осмотра")), true)
            }, fNormLeft);


        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        doc.paragraphs().insert_paragraph(L"Члены Комиссии", fNormLeft);

        duckx::Table table = doc.tables().insert_table(duckx::align_center);
        duckx::TableRow row1T1 = table.insert_row();
        ss.Format("%s ______________", ado->readStr("Должность 1 члена"));
        row1T1.insert_cell(ss.AllocSysString(), fNormLeft);
        row1T1.insert_cell(getWideString(ado->read("ФИО 1 члена")), fNormLeft);
        duckx::TableRow row2T1 = table.insert_row();
        ss.Format("%s ______________", ado->readStr("Должность 2 члена"));
        row2T1.insert_cell(ss.AllocSysString(), fNormLeft);
        row2T1.insert_cell(getWideString(ado->read("ФИО 2 члена")), fNormLeft);
        duckx::TableRow row3T1 = table.insert_row();
        ss.Format("%s ______________", ado->readStr("Должность визирующего"));
        row3T1.insert_cell(ss.AllocSysString(), fNormLeft);
        row3T1.insert_cell(getWideString(ado->read("ФИО визирующего")), fNormLeft);

        doc.paragraphs().insert_paragraph(L"", fNormLeft);
        doc.paragraphs().insert_paragraph(L"", fNormLeft);

        doc.paragraphs().insert_paragraph(getWideString(ado->read("data_utverzhdenija_akta")), fNormRight);
        //doc.paragraphs().insert_paragraph(L"«______»_______________20__год", fNormRight);
        ///////////////////////////////////////////

        doc.save();

        HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
    }
}