#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QProgressDialog>
#include <QFile>
#include <QRegularExpression>
#include <QMessageBox>

#include <QInputDialog>

#include <mystd.h>

#include <iostream>
#include <cxema/Graph.h>
#include <cxema/graph2.h>
#include <cxema/cxema1.h>

#include <db/db.h>


//static map<int, int> map_CurrentExternalCode;
const double coef_xy = 1.;

long sizeOfQ(QSqlDatabase& db, const QString & q);

void setCurrentExternalCode(int fileID, int kod);
//{
//    map_CurrentExternalCode[fileID] = kod;
//}

void setNodeCxema(CNode2* node2, const QString & _schemeNum)
{
    if (_schemeNum == "" || !node2) return;

//    char* schemeNum = _schemeNum.toLatin1().data();
//    int n1, n2;

    static QRegularExpression re("^([0-9]+)\\.([0-9]+)$");
    QRegularExpressionMatch match = re.match(_schemeNum);
    if (match.hasMatch()) {
//         int n1 = match.captured(1).toInt();
        int n2 = match.captured(2).toInt();

//    if (sscanf_s(schemeNum, "%d.%d", &n1, &n2) == 2) {
        node2->node.isEl = false;
        node2->node.isNez = false;
        if (n2 == 5 || n2 == 6 || n2 == 7 || n2 == 8 || _schemeNum == "1.3" || _schemeNum == "1.4") {
            node2->node.isEl = true;
        }

        if (n2 == 9 || n2 == 10 || n2 == 11 || n2 == 12 ||  _schemeNum == "1.5" || _schemeNum == "1.6") {
            node2->node.isNez = true;
            node2->node.isEl = false;
        }
    }
}

bool readNodeNew(QSqlQuery& query, CNode2* node2, int typ, CGraph2* m_graph, bool is_out)
{
    long id = query.value("id").toInt();
    long fileID = query.value("fileID").toInt();
    long internalNodeID = query.value("internalNodeID").toInt();

    long externalCodeID = query.value("externalCodeID").toInt();
    long externalSignID = query.value("externalSignID").toInt();
    long podp = query.value("displaySign").toInt();

    setCurrentExternalCode(fileID, externalCodeID);

    long consumerStateID = query.value("consumerStateID").toInt();
    QString schemeNum = query.value("schemeNum").toString();

    long connectID = query.value("connectID").toInt();

    long nodeTypeID = query.value("nodeTypeID").toInt();

    long id2 = query.value("id2").toInt();

    QString name = query.value("externalNodeName").toString();
    QString namePS = query.value("nodeName").toString();

    double x = query.value("x").toDouble();
    double y = query.value("y").toDouble();

    x *= coef_xy;
    y *= coef_xy;

    double geoMarkTopTube = query.value("geoMarkTopTube").toDouble();
    double geoMarkNodeArea = query.value("geoMarkNodeArea").toDouble();

    double hz = query.value("hz").toDouble();

    NAGR nagr;
    memset(&nagr, 0, sizeof(NAGR));

    if (typ == TIP_PO) {
        nagr.d_otopl_zco = query.value("calcHLdep").toDouble();
        nagr.d_otopl_nco = query.value("calcHLindep").toDouble();
        nagr.d_ventil_cv = query.value("calcHLventil").toDouble();
        nagr.d_otopl_pr = query.value("calcHLparall").toDouble();
        nagr.d_gv_pr = query.value("calcHLGVSparall").toDouble();
        nagr.d_rez_pr = query.value("avgHLcompParall").toDouble();
        nagr.d_otopl_sm = query.value("calcHLmix").toDouble();
        nagr.d_gv_sm = query.value("calcHLGVSmix").toDouble();
        nagr.d_rez_sm = query.value("avgHLcompMix").toDouble();
        nagr.d_otopl_ps = query.value("calcHLconseq").toDouble();
        nagr.d_gv_ps = query.value("calcHLGVSconseq").toDouble();
        nagr.d_rez_ps = query.value("avgHLcompconseq").toDouble();
        nagr.d_otopl_pw = query.value("calcHLpreON").toDouble();
        nagr.d_gv_pw = query.value("calcHLGVSpreON").toDouble();
        nagr.d_rez_pw = query.value("avgHLcompPreON").toDouble();
        nagr.d_gv_op = query.value("avgHLGVSopenSysFlow").toDouble();
        nagr.d_gv_oo = query.value("avgHLGVSopenSysRet").toDouble();
        nagr.d_rez_op = query.value("avgHLcompOpen").toDouble();
    }

    node2->node.pribor_ucheta = 0;

    if (typ == TIP_PR) {
        node2->node.pribor_ucheta = (query.value("WEmeteringDeviceID") == 2);
        node2->node.nomDR = query.value("nomDR").toInt();

        nagr.d_otoplz = query.value("calcHLdep").toDouble();
        nagr.d_otopln = query.value("calcHLindep").toDouble();
        //        nagr.d_otn_fs = query.value("relLoadFacade").toDouble();
        nagr.d_otopl_tp = query.value("calcInternHD").toDouble();
        nagr.d_ventil = query.value("calcHLventil").toDouble();
        nagr.d_dolja_vent = query.value("expendHWpart").toDouble();
        nagr.d_kondiz = query.value("avgHLcond").toDouble();
        nagr.d_txz = query.value("avgHLcloseSys").toDouble();
        nagr.d_txop = query.value("avgHLopenSysFlow").toDouble();
        nagr.d_txoo = query.value("avgHLopenSysRet").toDouble();
        nagr.d_gvop = query.value("avgHLGVSopenFlow").toDouble();
        nagr.d_gvoo = query.value("avgHLGVSopenRet").toDouble();
        nagr.d_gvpr = query.value("avgHLGVScloseParall").toDouble();
        nagr.d_gvsm = query.value("avgHLGVScloseMix").toDouble();
        nagr.d_gvps = query.value("avgHLGVScloseConseq").toDouble();
        nagr.d_gvpw = query.value("avgHLGVSclosePreON").toDouble();
    }

    long automDegID = query.value("automDegID").toInt();

    switch (automDegID) {
    case 1:  node2->node.pr_avtomat = QChar(L' '); break;
    case 2:  node2->node.pr_avtomat = QChar(L'А'); break;
    case 3:  node2->node.pr_avtomat = QChar(L'3'); break;
    }

    node2->node.Qot = nagr.d_otoplz + nagr.d_otopln + nagr.d_otopl_zco + nagr.d_otopl_nco + nagr.d_otopl_pr + nagr.d_otopl_sm + nagr.d_otopl_ps + nagr.d_otopl_pw;
    node2->node.Qgvs = nagr.d_gvop + nagr.d_gvoo + nagr.d_gvpr + nagr.d_gvsm + nagr.d_gvps + nagr.d_gvpw + nagr.d_gv_op + nagr.d_gv_oo + nagr.d_gv_pr + nagr.d_gv_sm + nagr.d_gv_ps + nagr.d_gv_pw;
    node2->node.Qvent = nagr.d_ventil + nagr.d_ventil_cv;

    node2->node.isValid = (typ != TIP_PO && typ != TIP_PR) || node2->node.Qot != 0 || (node2->node.Qvent != 0 && schemeNum == "2.1");

    if (internalNodeID > 0) {
        CNode2* parent = m_graph->find_ins(internalNodeID);
        if (parent) {
            parent->node.isP = true;
        }
    }

    node2->node.nomP = node2->node.nomO = -1;
    if (externalSignID != SignNode_o) node2->node.nomP = id;
    if (externalSignID != SignNode_p) node2->node.nomO = id;

    node2->node.externalSignID = externalSignID;

    node2->node.id2 = id2;

    node2->node.connectID = connectID;


    if (name.length() >= MY_N10) name = name.left(MY_N10);

    node2->node.id_kod = externalCodeID;
    node2->node.node_name = CNodeName("", name, S_OBA);
    node2->node.coord.x = x;
    node2->node.coord.y = y;
    node2->node.typ = typ;
    node2->node.hz = hz;
    node2->node.internalNodeID = internalNodeID;

    node2->node._typPS = nodeTypeID;

    node2->node.schemeNum = schemeNum;

    setNodeCxema(node2, schemeNum);

    node2->node.isPodp = !podp;

    node2->node.geoMarkTopTube = geoMarkTopTube;
    node2->node.geoMarkNodeArea = geoMarkNodeArea;

    node2->node.isOtkl = consumerStateID != 1;
    node2->node.name = name;
    node2->node.namePS = namePS;

    node2->node.nomgP = id;
    node2->node.data.pihP = query.value("pihP").toDouble();
    node2->node.data.tempP = query.value("tP1").toDouble();

    node2->node.nomgO = id;
    node2->node.data.pihO = query.value("pihO").toDouble();
    node2->node.data.tempO = query.value("tO1").toDouble();

    if (typ == TIP_PR || typ == TIP_PO) {
        node2->node.qz = query.value("a15").toDouble();
        node2->node.qP = query.value("a16").toDouble();
        node2->node.qO = query.value("a17").toDouble();

        if (node2->node.qz == 0 && node2->node.qP == 0 && node2->node.qO == 0) {
            node2->node.isZakr = query.value("isZakr").toBool();
        }
        node2->node.nomPT = query.value("nomPT").toInt();
    }

    return true;
}

QString nodeQ()
{
    QString q =  readQ("sql3/us.sql");
    q.replace(QString("$user_gid$"), QString("Администратор"));
    return q;
}

bool CCxema::read_nodes(const QString & par, QProgressDialog* percent, bool first)
{
    QString q = nodeQ();

    if (q == "") {
        QMessageBox::warning(nullptr, "", "Не могу открыть файл sql3/us.sql. Переустановите систему");
        return false;
    }

    if (par.left(1) == "(") {
        q += QString(" AND %1").arg(par);
    }
    else {
        if (!isFragmentList(par)) {
            QMessageBox::warning(nullptr, "",
                QObject::tr("Некорректный список фрагментов: %1").arg(par));
            return false;
        }
        q += QString(" AND n.fileID IN (%1)").arg(par);
    }


/*
    if (first) {
        q += " AND n.internalNodeID IS NULL";
    }
    else {
        q += " AND NOT n.internalNodeID IS NULL";
    }
*/

//    QInputDialog::getMultiLineText(nullptr, "", "", q);

    QSqlQuery query(m_db);

    QString qq = QString("SELECT id FROM nodes n WHERE n.removed=0 AND n.fileID IN (%1)").arg(par);

    if (par.left(1) == "(") {
        qq = QString("SELECT id FROM nodes n WHERE n.removed=0 AND %1").arg(par);
    }

    long cnt = sizeOfQ(m_db, qq);

    query.setForwardOnly(true);
    query_exec(m_db, query, q);

    int i = 0;

    while (query.next()) {
        long id = query.value("id").toInt();

        long fileID = query.value("fileID").toInt();

        QString type_txt = query.value("type_txt").toString();

        int typ = getNodeTyp(type_txt);

        CNode2* node2 = m_graph->find_ins(id);

        if (node2) {
            readNodeNew(query, node2, typ, m_graph);
            node2->node.fileID = fileID;
        }

        if (percent && cnt) {
            if (i%1000 == 0) {
                percent->setValue(i * 100. / cnt / 2);
                QApplication::processEvents();
            }
        }

        i++;
    }

    if (percent && cnt) {
        percent->setValue(i * 100. / cnt / 2);
        QApplication::processEvents();
    }
    
    return true;
}
