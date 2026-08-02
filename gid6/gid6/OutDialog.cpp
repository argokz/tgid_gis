// gidrView.cpp : implementation of the CGidrView class
//

#include "stdafx.h"
#include "gid6.h"

#include "MainFrm.h"
#include "gid6Doc.h"

#include "ado.h"

#include "mmenu.h"
#include <atlstr.h>
#include "Ultimate Grid\FormDialog.h" 

int getOutID(CAdoFile *ado, int fileID);
CString getIdQ(const char *q);


int regex_match(const char *p, const char *buf, vector<CString> & match);

void TitleRazmer(CString t1, CString &t, CString &r)
{
    t = t1;
    r = "";

    vector<CString> match;

    int l = regex_match("^(.+),\\s*([^,]+)\\s*$", t1, match);

    if (l > 0) {

        set<CString> set_l = {"%","Вт/(м*К)","Гкал/ч","Град.С","°C","°С","кВ","кВт","кг","м","м.вод.ст","м.вод.ст. или расход","м/c","м^2","м^3","м^3/ч","мм","мм.вод.ст./м","об/мин","раз","сут","т/ч","час","штук(и)"};

        auto it = set_l.find(match[2]);

        if (it != set_l.end()) {
            t = match[1];
            r = match[2];
        }
    }
}

CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, const char* fileID);


void CGidrView::OnAnalizRezhima()
{
    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    struct MenuItemOut {
      char *name;
      char *out;
      char *query;
      int node_line;
    };

    MenuItemOut menudoc[] = {

        "Узлы, подающий трубопровод, Анализ напора", "UT_OUT", "1", 2,
        "Узлы, обратный трубопровод, Анализ напора", "UT_OUT", "2", 2,
        "Потребители, Анализ располагаемого напора", "UT_OUT", "3", 2,
        "Потребители, Анализ располагаемого напора на выходе потребителя", "UT_OUT", "4", 2,
        "Потребители, Анализ теплообеспеченности", "UT_OUT", "5", 2,
        "Потребители, Анализ теплового режима", "UT_OUT", "6", 2,
        "Обобщенные потребители, Анализ теплообеспеченности", "UT_OUT", "7", 2,
        "Обобщенные потребители, Анализ располагаемого напора", "UT_OUT", "8", 2,
        "Обобщенные потребители, Анализ теплового режима", "UT_OUT", "9", 2,
        "Участки трубопроводов, Анализ потерь напора", "UT_OUT", "10", 3,
    NULL, NULL, NULL, 0,
    };
    CString columnMark[] = {
        "Режим","Режим","Режим","Режим напора потребителя", "Режим (отд. потреб.)", "Анализ режима", "Режим", "", "Анализ режима", ""
    };
    CMMenu menu(this, IDD_MENU6, _TR("Выберите объект для анализа режима"));

    for (int i = 0; menudoc[i].name; i++) menu.lst.Add(menudoc[i].name);

    if (menu.DoModal() == IDOK) {
        CString tn = menudoc[menu.m_Index].out;
        CString fn, sql = menudoc[menu.m_Index].query;
        fn.Format("admissibilityAnalysis_ms\\%s", menudoc[menu.m_Index].query);

        CAdoFile* ado = m_cxema->m_ado;
        int fileID = m_cxema->m_cur_file->id;
        VIRT_DATA vd;
        if (menudoc[menu.m_Index].node_line == 2)
            vd = virt_data_node_noedit;
        else if (menudoc[menu.m_Index].node_line == 3)
            vd = virt_data_line_noedit;
        else
            vd = virt_data_unknown;
        sql = readQ(ado, fn, "", fileID);
        RunTableDB(ado->m_schema, tn, sql, -1, vd, columnMark[menu.m_Index], menudoc[menu.m_Index].name);
    }
}


void CGidrView::OnAnal()
{
    if (!curFile()) {
        AfxMessageBox(_TR("Выберите активный фрагмент"), MB_OK|MB_ICONINFORMATION);
        return;
    }

    struct MenuItemOut {
      char *name;
      char *out;
      char *query;
      int node_line;
    };

    MenuItemOut menudoc[] = {
      "Участки теплопроводов", "UT_OUT", NULL, 2,
      "Потребители", "PT_OUT", NULL, 1,
      "Насосные агрегаты", "NS_OUT", NULL, 2,
      "Сетевые регуляторы", "RS_OUT", NULL, 2,
      "Байпасы наружных теплопроводов", "BP_OUT", NULL, 2,
      "Задвижки", "ZD_OUT", NULL, 2,
      "Регулирующая арматура", "ZD2_OUT", NULL, 2,
      "Узлы", "US_OUT", NULL, 1,
    NULL, NULL, NULL, 0,
    };

    CMMenu menu(this, IDD_MENU6, _TR("Выберите объект для анализа режима"));

    for (int i = 0; menudoc[i].name; i++) menu.lst.Add(menudoc[i].name);

    if (menu.DoModal() == IDOK) {

        CAdoFile *ado = m_cxema->m_ado;
        CString q;
        CString tn = menudoc[menu.m_Index].out;
        q.Format("SELECT * FROM %s LIMIT 1", tn);

        if (ado->m_type_of_net == TYPE_OF_NET_MSSQL) {
            q.Format("SELECT TOP 1 * FROM %s", tn);
        }


        CFormDialog dlg(this, menudoc[menu.m_Index].name);

        if (ado->openTable0(q)) {
            int nFlds = ado->NFlds();

            CString qc;
            CAdoField *adoField = new CAdoField[nFlds + 1];

            for (int i = 0; i < nFlds; i++) {
                CAdoField f = ado->Field(i);
                adoField[i] = f;
            }
            ado->closeTable();

            CString ss = "";


            for (int i = 0; i < nFlds; i++) {
                CAdoField f = adoField[i];

                CString name = f.name;

                if (name.CompareNoCase("id") != 0 && name.CompareNoCase("nodeID") != 0 && name.CompareNoCase("lineID") != 0 && name.CompareNoCase("calculationID") != 0 
                    && name.CompareNoCase("externalSignLineID") != 0 && name.CompareNoCase("externalSign") != 0
                    && f.typ == adDouble
                    ) {

                    CString title = f.title;
                    CString f1 = f.title;

                    MySQLColumns *col = ado->findMySQLColumns(tn, name);
                    MySQLColumns2 *col2 = ado->findMySQLColumns2(tn, name);
                    MySQLColumns2 *colOld = ado->findMySQLOldColumns(tn, name);

                    if (col) {
                        if (col->strCOLUMN_COMMENT != "") {
                            title = col->strCOLUMN_COMMENT;
                            f1 = col->name_full;
                        }
                        //           dg.NumericScale = (short)col->lNUMERIC_SCALE;
                    }
                    CString razmer = "";

                    if (colOld) {
                        if (colOld->hlp != "")  title = colOld->hlp;
                        if (colOld->helpshiftf1 != "" && f1 == f.name)  f1 = colOld->helpshiftf1;

                        razmer = colOld->stand_razmer;

                    }
                    else if (col2) {
                        if (col2->hlp != "")  title = col2->hlp;
                        if (col2->helpshiftf1 != "" && f1 == f.name) f1 = col2->helpshiftf1;

                        razmer = col2->stand_razmer;
                    }

                    TitleRazmer(title, title, razmer);

                    if (title.Left(2) != "b1") {
                        if (ss != "") ss += ",";
                        ss += f.name;
                        dlg.addRow(name, title, razmer);
                    }
                }
            }

            if (tn == "UT_OUT") {
                dlg.addRow("tubeRoughness", "Шероховатость", "мм");
            }

            if (dlg.DoModal() == IDOK && dlg.m_q != "") {
                CString q, qid;

                int fileID = m_cxema->m_cur_file->id;
                int calculationID = getOutID(ado, fileID);

                set<int> set_id;

                if (menudoc[menu.m_Index].node_line == 1) {
                    //                    q.Format("SELECT n.id, n.externalNodeName, %s FROM nodes n JOIN %s n2 ON n2.nodeID=n.id WHERE %s AND n2.calculationID=%d ",  ss, tn, dlg.m_q, calculationID);
                    q.Format("SELECT n.id, n.externalNodeName as 'Наименование узла', %s FROM %s n2\n"
                        " JOIN nodes n ON n2.nodeID=n.id WHERE %s AND n2.calculationID=%d ",
                        ss, tn, dlg.m_q, calculationID);
                }
                else if (menudoc[menu.m_Index].node_line == 2) {
                    //                  q.Format("SELECT l.id, ec1.name AS kod1 ,n1.externalNodeName AS uzel1,ec2.name AS kod2 ,n1.externalNodeName AS uzel2, esl.name AS pr, %s FROM linesobj l JOIN %s l2 ON l2.lineID=l.id "
                    q.Format("SELECT l.id, ec1.name AS kod1 ,n1.externalNodeName AS uzel1,ec2.name AS kod2 ,n2.externalNodeName AS uzel2, esl.name AS pr, %s FROM %s l2 JOIN linesobj l ON l2.lineID=l.id "
                        " JOIN nodes n1 ON n1.id=l.nodeID1 "
                        " JOIN nodes n2 ON n2.id=l.nodeID2 "
                        " join externalCodes ec1 on ec1.id = n1.externalCodeID "
                        " join externalCodes ec2 on ec2.id = n2.externalCodeID "
                        " join externalSignLine esl on esl.id=l2.externalSignLineID "
                        " WHERE (%s) AND l.removed = 0 AND l2.calculationID=%d ORDER BY l.id,l2.externalSignLineID ", ss, tn, dlg.m_q, calculationID);
                }

                if (menudoc[menu.m_Index].node_line && tn == "UT_OUT") {
                        q.Format("SELECT l.id, ec1.name AS kod1 ,n1.externalNodeName AS uzel1,ec2.name AS kod2 ,n2.externalNodeName AS uzel2, esl.name AS pr, %s, tubeRoughness as 'Шероховатость' "
                        " FROM %s l2"
                        " JOIN linesobj l ON l2.lineID=l.id "
                        " join heatPipeSections hps on hps.lineID = l.id "
                        " JOIN nodes n1 ON n1.id=l.nodeID1 "
                        " JOIN nodes n2 ON n2.id=l.nodeID2 "
                        " join externalCodes ec1 on ec1.id = n1.externalCodeID "
                        " join externalCodes ec2 on ec2.id = n2.externalCodeID "
                        " join externalSignLine esl on esl.id=l2.externalSignLineID "
                        " WHERE (%s) AND l.removed = 0 AND l2.calculationID=%d ORDER BY l.id,l2.externalSignLineID ", ss, tn, dlg.m_q, calculationID);
                }

                qid = getIdQ(q);

                    //              qid.Format("SELECT id FROM (%s\n) __T", q);
                if (ado->openTable0(qid)) {
                    while (!ado->isEOF()) {
                        int id = ado->read_long("id");
                        set_id.insert(id);
                        ado->MoveNext();
                    }
                    ado->closeTable();
                }

              m_cxema->m_graph->vyd_lines(set_id, true);

              RunTable(tn, q, -1, menudoc[menu.m_Index].node_line == 1 ? virt_data_node_out : virt_data_line_out, "", "");
            }
        }
    }
}
