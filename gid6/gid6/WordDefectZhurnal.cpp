#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "win.h"
#include "geofile.h"

#include "maptabs.h"

#include "math.h"


#include "ado.h"
#include "TaskProgressDlg.h"


#include "grid/msword8.h"

static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
static COleVariant covTrue((short)TRUE), covFalse((short)FALSE);


bool closeWord(const char* doc_name);

Range SetText(Table& oTbl, int row, int col, LPCTSTR str);
Range SetTextBorder(Table& oTbl, int row, int col, LPCTSTR str);
void SetAlign(Range& oRange, long align);
Range AppendText(Range& range, LPCTSTR str);



CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);

#include "PropertyGrid\test\PropGridDlg.h"

Range AddField(CPropertyGrid* wnd, Range& range, CString title, CString fn)
{
    range = AppendText(range, title);

    CString val;
    wnd->GetNameValue(fn, val);
    range.SetBold(0);
    range = AppendText(range, "\t");
    range = AppendText(range, val);
    range.SetBold(1);
    range = AppendText(range, "\n");

    return range;
}


Range WriteAktRassledovania(CPropertyGrid* wnd, Range range)
{
    range = AddField(wnd, range, "Акт расследования технологического нарушения\n№___\n", "");


    range = AddField(wnd, range, "Район эксплуатации:", "");
    range = AddField(wnd, range, "Магистраль:", "");
    range = AddField(wnd, range, "Участок эксплуатации:", "");
    range = AddField(wnd, range, "Начальник участка", "");


    range = AddField(wnd, range, "1.\tНаименование участка:", "");
    range = AddField(wnd, range, "2.\tДата и время обнаружения технологического нарушения:", "");
    range = AddField(wnd, range, "3.\tАдрес:", "");
    range = AddField(wnd, range, "4.\tПоврежденный трубопровод:", "");
    range = AddField(wnd, range, "5.\tВид нарушения:", "");
    range = AddField(wnd, range, "6.\tКатегория нарушения:", "remontCatID");
    range = AddField(wnd, range, "7.\tПоврежденный элемент:", "");
    range = AddField(wnd, range, "8.\tХарактер повреждения", "tipPovrezhdeniaID");
    range = AddField(wnd, range, "9.\tМесто расположения центра повреждения на трубопроводе, часов:", "tsentrPovrezhdenia");
    range = AddField(wnd, range, "10.\tРазмер повреждения:  Высота, мм: Ширина, мм: Площадь, кв.\tмм", "");
    range = AddField(wnd, range, "11.\tПричины нарушения технические:", "");
    range = AddField(wnd, range, "12.\tПричины нарушения организационные:", "");
    range = AddField(wnd, range, "13.\tСопутствующие причины:", "");
    range = AddField(wnd, range, "14.\tСостояние конструкций канала/камеры:", "");
    range = AddField(wnd, range, "15.\tСостояние теплоизоляции теплопроводов:", "");
    range = AddField(wnd, range, "16.\tСостояние наружной поверхности теплопроводов:", "");
    range = AddField(wnd, range, "17.\tСпособ ликвидации нарушения:", "");
    range = AddField(wnd, range, "18.\tКоличество отключенных потребителей:", "");
    range = AddField(wnd, range, "19.\tВремя начала ремонтных работ:    Время завершения  ремонтных работ:", "");
    range = AddField(wnd, range, "20.\tКоличество недоотпущенной тепловой энергии, Гкал:", "");
    range = AddField(wnd, range, "21.\tЗатраты на восстановление, тг:", "");
    range = AddField(wnd, range, "22.\tИные социальные последствия:", "");

    range = AddField(wnd, range, "Комиссия расследования", "");

    return range;
}


#include <duckx.hpp>

static duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
static duckx::formatting_flag fNormCenter = duckx::align_center | duckx::size_8 | duckx::tnr_style;
static duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_8;
static duckx::formatting_flag fBoldCenter = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;

void WordDefectZhurnalHead(duckx::Table &table1)
{
    duckx::TableRow rowHeader1 = table1.insert_row().set_child(L"w:tblHeader");

    rowHeader1.insert_cell(L"№\nТН\nп.п", fBoldCenter, 1, 1);
//    \u00AD

    rowHeader1.insert_cell(L"Дата возник- новения ТН\nВремя возник- новения ТН", fBoldCenter, 1, 1);
    rowHeader1.insert_cell(L"Описание технологического нарушения (ТН)", fBoldCenter, 6, 1);
    rowHeader1.insert_cell(L"Дата ликвидации ТН\nВремя ликвидации ТН", fBoldCenter, 1, 1);
    rowHeader1.insert_cell(L"Примечания (№ акта расследования ТН;\n№ приказа ввода в экслуатацию)", fBoldCenter, 1, 1);


    duckx::TableRow rowHeader2 = table1.insert_row().set_child(L"w:tblHeader");
    rowHeader2.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    rowHeader2.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    rowHeader2.insert_cell(L"Местоположение ТН", fBoldCenter, 1, 1)   ; 
    rowHeader2.insert_cell(L"Вид ТН", fBoldCenter, 1, 1)              ; 
    rowHeader2.insert_cell(L"Характеристика ТН", fBoldCenter, 1, 1)   ; 
    rowHeader2.insert_cell(L"Причина ТН", fBoldCenter, 1, 1)          ; 
    rowHeader2.insert_cell(L"Последствия ТН", fBoldCenter, 1, 1)      ; 
    rowHeader2.insert_cell(L"Способ ликвидации ТН", fBoldCenter, 1, 1); 
    rowHeader2.insert_cell(L"", fBoldCenter, 1, 2)                    ; 
    rowHeader2.insert_cell(L"", fBoldCenter, 1, 2)                    ; 


    duckx::TableRow rowHeader3 = table1.insert_row().set_child(L"w:tblHeader");

    rowHeader3.insert_cell(L"", fBoldCenter, 1, 2)                                                                                                     ; 
    rowHeader3.insert_cell(L"", fBoldCenter, 1, 2)                                                                                                     ; 
    rowHeader3.insert_cell(L"(участок трубоп-да, адрес, ближайшая камера – расстояние до неё)", fBoldLeft, 1)                                          ; 
    rowHeader3.insert_cell(L"(авария - А;\nинцидент:\nтехнологический отказ - ТО;\nфункциональный отказ - ФО)", fBoldLeft, 1)                          ; 
    rowHeader3.insert_cell(L"(конкретная неисправность\nотказавшего оборудования;\nнарушенная функция)", fBoldLeft, 1)                                 ; 
    rowHeader3.insert_cell(L"(установленная и (или) предполагаемая причина ТН)", fBoldLeft, 1)                                                         ; 
    rowHeader3.insert_cell(L"(кол-во  откл. потребителей, недоотпущенной тепл.энергии, Гкал; затраты на восстан-е, тг; иные социальн-е последствия", fBoldLeft, 1); 
    rowHeader3.insert_cell(L"(ремонт:\nТР – текущий,\nКР – капитал-й,\nИ – инвестиц-й,\nО – опрессовка;\nвыполненные работы)", fBoldLeft, 1)           ; 
    rowHeader3.insert_cell(L"", fBoldCenter, 1, 2)                                                                                                     ; 
    rowHeader3.insert_cell(L"", fBoldCenter, 1, 2)                                                                                                     ; 

    duckx::TableRow rowHeader4 = table1.insert_row(); //  .set_child(L"w:tblHeader");


    rowHeader4.insert_cell(L"1", fBoldCenter, 1).set_width(154);
    rowHeader4.insert_cell(L"2", fBoldCenter, 1).set_width(283);
    rowHeader4.insert_cell(L"3", fBoldCenter, 1).set_width(632);
    rowHeader4.insert_cell(L"4", fBoldCenter, 1).set_width(390);
    rowHeader4.insert_cell(L"5", fBoldCenter, 1).set_width(632);
    rowHeader4.insert_cell(L"6", fBoldCenter, 1).set_width(682);
    rowHeader4.insert_cell(L"7", fBoldCenter, 1).set_width(729);
    rowHeader4.insert_cell(L"8", fBoldCenter, 1).set_width(683);
    rowHeader4.insert_cell(L"9", fBoldCenter, 1).set_width(347);
    rowHeader4.insert_cell(L"10", fBoldCenter,1).set_width(468);
}

void split_sql_expressions(CString text, std::list<CString> &list_q);

CString split_query(CAdoFile* ado, CString q) 
{

    std::list<CString> list_q;

    split_sql_expressions(q, list_q);

    if (list_q.size() > 1) {
        q = list_q.back();
        list_q.pop_back();

        for (auto& qq : list_q) {
            ado->Execute(qq);
        }
    }
    return q;
}



int WordDefectZhurnal(CString doc_name, CString q, int id)
{
    //    Percent2 pc;

    CString fname;
    fname.Format("%s\\excel2\\docx\\empty_doc_album.docx", argpath());

    CopyFile(fname, doc_name, 0);

    duckx::Document doc(doc_name.GetString());

    doc.set_size(18);

    doc.open();

    doc.paragraphs().insert_paragraph(L"Журнал нарушений", fBoldCenter);

    duckx::Table table1;

    CAdoFile* ado = getAdo(getPsAdoName());
    if (!ado) return 0;

    CString ss;

    int nn = 1;

    duckx::TableCell cell;
    duckx::TableRow rowT1;

    {

        ////////////////////
        doc.paragraphs().insert_paragraph(L"Сведения по участку тепловой сети", fBoldCenter);
      COleDateTime v = COleDateTime::GetCurrentTime();
      CString date_str = v.Format("%d.%m.%Y");

        doc.paragraphs().insert_paragraph(date_str.AllocSysString(), fNormLeft);
//        doc.paragraphs().insert_paragraph(L"");

        duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
        WordDefectZhurnalHead(table1);
    }
///////////////



    CString s;

    CString nach_old = "!!!!!!!!!";
    CString uch_old = "!!!!!!!!!";

    q = split_query(ado, q);

    {
        if (ado->openTable0(q)) {
            while (!ado->isEOF()) {

                CString d1 = ado->readStr("Дата обнаружения нарушения");
                CString d2 = ado->readStr("Время обнаружения повреждения");
                CString s3 = ado->readStr("Вид нарушения");
                CString d_zav = ado->readStr("Дата завершения ремонтных работ");
                CString t_zav = ado->readStr("Время завершения ремонтных работ");


                CString str0 = ado->readStr("Начальник участка");
                CString str1 = ado->readStr("Наименование начального узла");
                CString str2 = ado->readStr("Код расчетной схемы начального узла");
                CString str3 = ado->readStr("Наименование конечного узла");
                CString str4 = ado->readStr("Код расчетной схемы конечного узла");

                CString str5 = ado->readStr("Наименование участка");
                CString str6 = ado->readStr("Ближайшая камера");
                CString str7 = ado->readStr("Расстояние до нарушения от ближайшей камеры, м");

                CString povr_elem = ado->readStr("Поврежденный элемент");
                
                //CString str8 = ado->readStr("Владелец участка теплопровода");


                if (str0 != nach_old || str5 != uch_old) {

                    table1 = doc.tables().insert_table(duckx::align_center | duckx::border);

                    if (nach_old != str0) {
                        duckx::TableRow row = table1.insert_row();
                        row.insert_cell(str0.AllocSysString(), fBoldLeft, 10); 
//                        doc.paragraphs().insert_paragraph(str0.AllocSysString(), fBoldLeft);
                    }
                    if (uch_old != str5) {
                        duckx::TableRow row = table1.insert_row();
                        row.insert_cell(str5.AllocSysString(), fBoldLeft, 10); 
//                        doc.paragraphs().insert_paragraph(str5.AllocSysString(), fBoldLeft);
                    }

                    nach_old = str0;
                    uch_old = str5;
                }


                CString str8 = ado->readStr("Адрес");

                CString s5 = "";

                /**
                                if (str0 != "") {
                                    if (s5 != "") s5 += "\n";
                                    s5 += str0;
                                }
                                if (str5 != "") {
                                    if (s5 != "") s5 += "\n";
                                    s5 += str5;
                                }
                */
                if (str2 != "" || str4 != "") {
                    if (s5 != "") s5 += "\n";
                    CString s = str2 + " " + str1 + " - " + str4 + " " + str3;
                    s5 += s;
                }
                if (str7 != "") {
                    if (s5 != "") s5 += "\n";
                    s5 += str7 + " м";
                }

                if (str8 != "") {
                    if (s5 != "") s5 += "\n";
                    s5 += str8;
                }


                CString s6 = ado->readStr("Поврежденный трубопровод");
//                CString s7 = ado->readStr("Характер повреждения");
                CString s7 = ado->readStr("Код нарушения");
                CString pr1 = ado->readStr("Причины нарушения (технические)");
                CString pr2 = ado->readStr("Причины нарушения (организационные)");
                CString pr3 = ado->readStr("Сопутствующие причины");

                CString s8 = ado->readStr("Ремонт трубопровода и элементов");
                CString vid_rabot = ado->readStr("Вид работ");


                //                    Врезка элементов:
                //                    Демонтированы элементы:

                s8.Replace("врезка элементов", "<b>врезка элементов</b>");
                s8.Replace("замена элементов", "<b>замена элементов</b>");
                s8.Replace("ремонт элементов", "<b>ремонт элементов</b>");
                s8.Replace("демонтированы элементы", "<b>демонтированы элементы</b>");

                s8.Replace(":,", "\n");
                s8.Replace(":", ":\n");
                s8.Replace(",", ",\n");

                CString sn;

                sn.Format("%d", nn++);

                duckx::TableRow rowT1 = table1.insert_row().set_child(L"w:cantSplit");

                cell = rowT1.insert_cell(sn.AllocSysString(), fNormCenter);//
//                cell = rowT1.insert_cell(s5.AllocSysString(), fNormLeft);//

                ss = d1 + "\n" + d2;
                cell = rowT1.insert_cell(ss.AllocSysString(), fNormCenter);
                
                cell = rowT1.insert_cell(s5.AllocSysString(), fNormLeft);

                cell = rowT1.insert_cell(s7.AllocSysString(), fNormCenter);//
                cell = rowT1.insert_cell(povr_elem.AllocSysString(), fNormCenter);//

//                cell = rowT1.insert_cell(L"", fNormCenter);

                CString pr = "";
                if (pr1.TrimLeft() != "") {
                    if (pr != "") pr += "\n";
                    pr += "<b>Технические причины:</b>\n";
                    pr += pr1;
                }
                if (pr2.TrimLeft() != "") {
                    if (pr != "") pr += "\n";
                    pr += "<b>Организационные причины:</b>\n";
                    pr += pr2;
                }
                if (pr3.TrimLeft() != "") {
                    if (pr != "") pr += "\n";
                    pr += "<b>Сопутствующие причины:</b>\n";
                    pr += pr3;
                }
                cell = rowT1.insert_cell(pr.AllocSysString(), fNormLeft);


                CString kolichestvo_otklyuchennyh_potrebitelej = ado->readStr("Количество отключенных потребителей");
                CString kolichestvo_nedootpushchennoj_teplovoj_energii = ado->readStr("Количество недоотпущенной тепловой энергии, Гкал");
                CString zatraty_na_vosstanovlenie = ado->readStr("Затраты на восстановление, тг");
                CString inye_socialnye_posledstviya = ado->readStr("Иные социальные последствия");


                //                    wnd->GetNameValue("kolichestvo_otklyuchennyh_potrebitelej",  kolichestvo_otklyuchennyh_potrebitelej);
                //                    wnd->GetNameValue("kolichestvo_nedootpushchennoj_teplovoj_energii",  kolichestvo_nedootpushchennoj_teplovoj_energii);
                //                    wnd->GetNameValue("zatraty_na_vosstanovlenie",  zatraty_na_vosstanovlenie);
                //                    wnd->GetNameValue("inye_socialnye_posledstviya",  inye_socialnye_posledstviya);
                CString sss;

                if (atoi(kolichestvo_otklyuchennyh_potrebitelej) == 0) {
                    sss = "-";
                }
                else {
                    sss.Format("<b>отключено потребителей</b>: %s\n"
                        "<b>недоотпущенно тепловой энергии</b>: %s Гкал\n"
                        "<b>затраты на восстановление</b>: %s тг\n"
                        "%s",
                        kolichestvo_otklyuchennyh_potrebitelej,
                        kolichestvo_nedootpushchennoj_teplovoj_energii,
                        zatraty_na_vosstanovlenie,
                        inye_socialnye_posledstviya);
                }

                cell = rowT1.insert_cell(sss.AllocSysString(), fNormLeft);
                cell = rowT1.insert_cell((vid_rabot+"\n"+s8).AllocSysString(), fNormLeft);

                CString s_d_zav = d_zav + "\n" + t_zav;

                cell = rowT1.insert_cell(s_d_zav.AllocSysString(),  fNormLeft);

                CString nomer_akta = ado->readStr("Номер акта");
                CString nomer_prikaza = ado->readStr("Номер приказа");

                if (nomer_akta == "" && nomer_prikaza == "") {
                    sss = " - ";
                }
                else {
                    sss.Format("Номер акта: %s\n"
                               "Номер приказа: %s", nomer_akta, nomer_prikaza);
                }

                //                    SetTextBorder(oTbl1, r + 1, 10, sss);
                cell = rowT1.insert_cell(sss.AllocSysString(), fNormCenter);

                ado->MoveNext();
            }
            ado->closeTable();
        }
    }

    doc.save();

    HINSTANCE hRun = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);

    return 1;
}
