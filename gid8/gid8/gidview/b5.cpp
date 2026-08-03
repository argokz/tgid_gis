#include <QtGui>
#include <cxema/col_index.h>
#include <QtSql>
#include <QRegularExpression>

//#include <mainwindow.h>
//#include <gidview/GidWidget.h>
#include <gidview/GidWidget.h>
#include <db/db.h>

#include "cxema/undo.h"


QString getNodeQ(int typ, const QString & tn, const QString & qq);
QString getLineQ(int typ, const QString & tn, const QString & otkr, const QString & qq);
//long addNodeUS(CAdoFile * ado, const char* tn, const char* name, int fileID, int externalSignID, int internalNodeID, int externalCodeID, CFPoint pt);
int addNodeUS(QSqlDatabase & db, const QString & tn, const QString & name, int fileID, int externalSignID, int internalNodeID, int externalCodeID, CFPoint pt, int m_user);
int addLine(QSqlDatabase & db, int typ, CNode2 *n1, CNode2 *n2, int externalSignLineID, CCoordList &cl, int &idv, int m_user);
int getLineTyp2(const QString & tn);
QString getLineOtkr(int typ);


struct NODE {
    QString uzel, pr;
    int nomer_mesto;
    double x, y;

    bool operator<(const NODE& mc) const
    {
        if (uzel < mc.uzel) return true;
        if (uzel > mc.uzel) return false;

        if (pr < mc.pr) return true;
        if (pr > mc.pr) return false;

        return false;
    }
};



struct PARA {
    NODE n1, n2;
    int table_id;
    int nomer_mesto;

    QString destination;
};


static std::map<QString, std::map<QString, std::list<PARA> > > map_b5;
static std::map<QString, std::set<NODE> >  map_n_b5;
static std::map<QString, std::set<NODE> >  map_vp;

struct B4 {
    int id;
    QString obj;
    QString destination;
//    QString name;
    int image_index;
    QString out;
    QString g_p_t_out;
};

static std::map<int, B4 >  map_b4;

const B4 *get_b4(int id)
{
    auto it = map_b4.find(id);
    if (it != map_b4.end()) {
        return &it->second;
    }
    return nullptr;
}

QString get_b4_dest(int id)
{
    const B4 *b4 = get_b4(id);
    if (b4) {
        return QString("%1 %2").arg(b4->obj, b4->destination);
    }
    return "";
}



void open_b4()
{
    QString klfn = QString("%1kls/b4.txt").arg(argpath());

    QFile file(klfn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

//1.1,0,P,1,P,2,400130,-399900,400210,-399900,7

            static QRegularExpression re("^(.?[0-9]+),'(.*)','(.*)',(.*),'?(.*)'?,'?(.*)'?,'?(.*)'?$");
            QRegularExpressionMatch match = re.match(s);
            if (match.hasMatch()) {
                bool ok;
                B4 b4;
                b4.id = match.captured(1).toInt(&ok);
                b4.obj = match.captured(2);
                b4.destination = match.captured(3);
//                b4.name = match.captured(4);
                b4.image_index = match.captured(5).toInt(&ok);
                b4.out = match.captured(6);
                b4.g_p_t_out = match.captured(7);
                map_b4[b4.id] = b4;
            }
        }
    }
}

void open_b5()
{
    QString klfn = QString("%1kls/b5.txt").arg(argpath());

    QFile file(klfn);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QTextStream in(&file);
//        in.setEncoding(QStringConverter::System);

        while (!file.atEnd()) {
//            QString s = in.readLine();
            QString s = readCP1251Line(file);

//1.1,0,P,1,P,2,400130,-399900,400210,-399900,7

            static QRegularExpression re("^(.?[0-9]+\\.[0-9]+),([0-9]*),([PO]?),([0-9]*),([PO]?),(.+),([-0-9]*),([-0-9]*),([-0-9]*),([-0-9]*),([0-9]+)$");
            QRegularExpressionMatch match = re.match(s);
            if (match.hasMatch()) {
                PARA para;

                QString nomer_cx = match.captured(1);


                para.n1.uzel = match.captured(2);
                para.n1.pr = match.captured(3);
                para.n2.uzel = match.captured(4);
                para.n2.pr = match.captured(5);

                QString name_table = match.captured(6);

                if (name_table == "Расходомер") continue;
                if (name_table == "Манометр") continue;
                if (name_table == "Термометр") continue;

                para.nomer_mesto = match.captured(11).toInt();

                para.n1.x = match.captured(7).toInt();
                para.n1.y = match.captured(8).toInt();
                para.n1.nomer_mesto = para.n1.uzel.toInt() * 2 + 10;

                para.n2.x = match.captured(9).toInt();
                para.n2.y = match.captured(10).toInt();
                para.n2.nomer_mesto = para.n2.uzel.toInt()*2 + 10 + 1;

//                para.destination = ado->readStr("destination");

                if (nomer_cx == "7.10") {
                    int qq;
                    qq = 1;
                }

                if (para.n1.uzel != "") {
                    auto it = map_n_b5[nomer_cx].find(para.n1);
                    if (it == map_n_b5[nomer_cx].end()) {
                        map_n_b5[nomer_cx].insert(para.n1);
                    }
                }

                if (para.n2.uzel != "") {
                    auto it = map_n_b5[nomer_cx].find(para.n2);
                    if (it == map_n_b5[nomer_cx].end()) {
                        map_n_b5[nomer_cx].insert(para.n2);
                    }
                    map_b5[nomer_cx][name_table].push_back(para);
                }



                if (name_table == "Узел подпитки") {
                    map_vp[nomer_cx].insert(para.n1);
                }
            }
        }
    }
}




bool delCxemaFull(CCxema *m_cxema, const CNode2 *node);
#if 0
{
    if (node) {
        QString q = QString("DELETE nodes  WHERE internalNodeID=%1").arg(node->id);


        QSqlQuery query(m_cxema->m_db);
        query_exec(query, q);

        if (query_exec(query, q)) {
            QString q = QString("DELETE linesobj WHERE nodeID1 in (SELECT id FROM net.v_nodes WHERE internalNodeID=%1)").arg(node->id);
            if (query_exec(query, q)) {
                m_cxema->m_graph->clearVnutr(node->id);
            }
        }
        if (node->node.typ != TIP_PR) {
//            addRemoved(m_cxema->m_ado, CH_T_DELETE_CXEMA_END, node->id, "");
        }
    }
    return true;
}
#endif

QSizeF GidWidget::draw_example_style_line(QPainter *painter, int mag_ras, int tubingTypeID, bool open, bool q, double ring) 
{
    double old_mas_otn = mas_otn;
    mas_otn = 1.0;

    int w = painter->device()->width();
    int h = painter->device()->height();

    int x = 20, y = h/2;
    QColor color = Qt::red;

    geom_old = geom;

    geom.masx = 1;
    geom.masy = 1;
    geom.bx = 0;
    geom.by = 0;


//    drawLineTyp(painter, 1, x, y, h-x, y, TIP_UT, color, Qt::red, Qt::red, 0, true, true, false, false);

    CGraph2 graph(nullptr);

    CNode2 *n1 = graph.find_ins(1);
    CNode2 *n2 = graph.find_ins(2);

    if (n1) {
        n1->node.fileID = 0;
        n1->node.coord.x = x;
        n1->node.coord.y = y;
        n1->node.internalNodeID = 0;
    }

    if (n2) {
        n2->node.fileID = 0;
        n2->node.coord.x = h-x;
        n2->node.coord.y = y;
        n2->node.internalNodeID = 0;
    }

    CCoordList NP;
    CLINE2 * ll = graph.insert_line(n2, n1, NP, false);
    if (ll) {
        CLine2 *l = (CLine2 *) ll;

        l->line.typ = TIP_UT;
        l->line.nomP = l->line.nomO = 1;
        l->line.pod.q = l->line.obr.q = q;
        l->line.pod.diam = l->line.obr.diam = mag_ras == 0 ? 1000 : 100;
        l->line.tubingTypeID = tubingTypeID;
        l->line.pod.isOtkl = !open;
        l->line.obr.isOtkl = !open;

        double ring_old = m_ring;
        m_ring = ring;

        drawLine1(painter, ll, false, false);
        drawNodeTyp(painter, x, y, TIP_US, color);
        drawNodeTyp(painter, h-x, y, TIP_PR, color);
        m_ring = ring_old;

    }

    geom = geom_old;

    return QSizeF(0, 0);

}

QSizeF GidWidget::draw_b5(QPainter *painter, const QString &cx) 
{
    double old_mas_otn = mas_otn;
    mas_otn = 1.0;

    int w = painter->device()->width();
    int h = painter->device()->height();

    // Границы картины
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    auto updateBounds = [&](double x, double y) {
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    };

    // Рисование (логика осталась без изменений)
    auto it0 = map_b5.find(cx);
    if (it0 != map_b5.end()) {
        for (auto it1 : it0->second) {
            int idid = getLineTyp2(it1.first);
            if (idid >= 0) {
                for (auto &itl : it1.second) {
                    NODE n1 = itl.n1;
                    NODE n2 = itl.n2;
                    double x1 = n1.x;
                    double x2 = n2.x;
                    double y1 = n1.y;
                    double y2 = n2.y;

                    updateBounds(x1, y1);
                    updateBounds(x2, y2);
                }
            }
        }

        minX -= 40*(maxX-minX)/w;
        minY -= 20*(maxY-minY)/h;
        maxX += 40*(maxX-minX)/w;
        maxY += 20*(maxY-minY)/h;

        for (auto it1 : it0->second) {
            int idid = getLineTyp2(it1.first);
            if (idid >= 0) {
                for (auto &itl : it1.second) {
                    NODE n1 = itl.n1;
                    NODE n2 = itl.n2;

                    double x1 = (n1.x - minX)*w/(maxX-minX);
                    double x2 = (n2.x - minX)*w/(maxX-minX);
                    double y1 = (n1.y - minY)*h/(maxY-minY);
                    double y2 = (n2.y - minY)*h/(maxY-minY);

                    QColor color = (itl.n1.pr == CPOD || itl.n2.pr == CPOD) ? Qt::red : Qt::blue;
                    bool punct = true;

                    drawLineTyp(painter, 1, x1, y1, x2, y2, idid, color, Qt::red, Qt::red, 0, punct, true, true, false, false);
                }
            }
        }

        auto itn = map_n_b5.find(cx);
        if (itn != map_n_b5.end()) {
            for (const NODE &node : itn->second) {
//                double x = node.x - 40000;
//                double y = node.y + 40000;

//                  double x = node.x - minX;
//                double y = node.x - minY;

                double x = (node.x - minX)*w/(maxX-minX);
                double y = (node.y - minY)*h/(maxY-minY);


//                updateBounds(x, y);

                QColor color = (node.pr == CPOD) ? Qt::red : Qt::blue;

                drawNodeTyp(painter, x, y, TIP_US, color);
            }
        }
    }

    auto itup = map_vp.find(cx);
    if (itup != map_vp.end()) {
        for (const auto &node : itup->second) {
//            double x = node.x - 40000;
//            double y = node.y + 40000;

//            double x = node.x - minX;
//            double y = node.y - minY;

            double x = (node.x - minX)*w/(maxX-minX);
            double y = (node.y - minY)*h/(maxY-minY);

//            updateBounds(x, y);

            drawNodeTyp(painter, x, y, TIP_VODOR, Qt::red);
        }
    }

    mas_otn = old_mas_otn;

    // Возвращаем размеры картины
    double paintingWidth = maxX - minX;
    double paintingHeight = maxY - minY;

    return QSizeF(paintingWidth, paintingHeight);
}






void create_b5(QSqlDatabase &db, CNode2 *nP, CCxema* cxema, int m_user)
{
#if 1
    std::map<QString, std::map<QString, std::list<PARA> > >::const_iterator it0;
    std::map<QString, std::list<PARA> > ::const_iterator it1;
    std::list<PARA>::const_iterator itl;

    std::map<QString, std::set<NODE> >::const_iterator itn;
    std::map<std::pair<QString, int>, CNode2*> map_nnn;

    if (!nP) return;

//    delCxema1(cxema, nP);
    delCxemaFull(cxema, nP);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    int id = nP->id;
    int externalCodeID = nP->node.id_kod;
    int fileID = nP->node.fileID;
    QString cx = nP->node.schemeNum;

    itn = map_n_b5.find(cx);
    if (itn != map_n_b5.end()) {
        set<NODE>::const_iterator it;

        for (it = itn->second.begin(); it != itn->second.end(); ++it) {
            int po = it->pr == CPOD ? 2 : 3;

            int id2 = 0;
            int typ = TIP_US;

            CFPoint point(it->x*10, it->y*10);

            int idn = addNodeUS(db, "nodes", it->uzel, fileID, po, id, externalCodeID, point, m_user);

            if (idn > 0) {

                CNode2* n = cxema->m_graph->ins(idn);
                if (n) {

                    QString qq = QString("nodes.id=%1").arg(idn);
                    QString q = getNodeQ(n->node.typ, n->getTableMySQL(), qq);

                    if (query_exec(db, query, q)) {
                        if (query.next()) {
                            n->node.nomP = -1;
                            n->node.nomO = -1;
                            n->node.fileID = fileID;
//                            readNode(db, n, n->node.typ, cxema->m_graph, n->getTableMySQL());
                            readNodeNew(query, ColIndex(query), n, n->node.typ, cxema->graph(), false);
                        }
                    }

                    pair<QString, int> para(it->uzel, po);

                    map_nnn[para] = n;
                }
            }
        }
    }

    it0 = map_b5.find(cx);
    if (it0 != map_b5.end()) {
        for (it1 = it0->second.begin(); it1 != it0->second.end(); ++it1) {
            QString str;
            int idid = getLineTyp2(it1->first);

            if (idid >= 0) {
                for (itl = it1->second.begin(); itl != it1->second.end(); ++itl) {
                    CCoordList cl;

                    CNode2* n1 = NULL, * n2 = NULL;
                    int po1, po2;

                    po1 = itl->n1.pr == CPOD ? 2 : 3;
                    po2 = itl->n2.pr == CPOD ? 2 : 3;

                    int po;

                    if (po1 == 2 && po2 == 2) po = 2;
                    if (po1 == 2 && po2 == 3) po = 4;
                    if (po1 == 3 && po2 == 2) po = 5;
                    if (po1 == 3 && po2 == 3) po = 3;

                    std::pair<QString, int> para1(itl->n1.uzel, po1);
                    std::pair<QString, int> para2(itl->n2.uzel, po2);

                    const auto it1 = map_nnn.find(para1);
                    if (it1 != map_nnn.end()) n1 = it1->second;

                    const auto it2 = map_nnn.find(para2);
                    if (it2 != map_nnn.end()) n2 = it2->second;

                    int idv;

                    int idl = ::addLine(db, idid, n1, n2, po, cl, idv, m_user);

                    QString tn = getLineTableMySQL(idid);

                    std::map<QString, QVariant>  map_v;

//                    ado->Edit(tn, idv);

                    if (idid == TIP_ZD) {
//                        map_v["dispatcherSwitch"] = itl->destination;
//                        ado->write("dispatcherSwitch", itl->destination);
                        if (itl->nomer_mesto == 39 || itl->nomer_mesto == 50) {
                            map_v["damperArmatureStateID"] = 2;
                        }
                    }
                    if (idid == TIP_DR) {
                        map_v["throtDiaphLoc"] = itl->destination;
                    }

                    execUpdateQ(db, tn, idv, map_v);

//                    ado->Update();

                    if (idl > 0) {
                        CCoordList cl;

                        CLINE2* line = cxema->m_graph->insert_line(n1, n2, cl);
                        if (line) {
                            CLine2* l = bline(line);
                            l->line.typ = idid;

                            if (po == SignLine_ob || po == SignLine_pp || po == SignLine_po) {
                                l->line.nomP = idl;
                                l->line.idP2 = idv;
                            }
                            if (po == SignLine_ob || po == SignLine_oo || po == SignLine_op) {
                                l->line.nomO = idl;
                                l->line.idO2 = idv;
                            }

                            QString qq = QString("l1.id=%1").arg(idl);

                            QString q = getLineQ(l->line.typ, l->getTableMySQL(), getLineOtkr(l->line.typ), qq);

                            if (query_exec(db, query, q)) {
                                if (query.next()) {
//                                    readLine(db, l, idid, cxema->m_graph, l->getTableMySQL(), getLineOtkr(l->line.typ));
                                    readLineNew(query, ColIndex(query), bline(l), bline(l)->line.typ, cxema->graph());
                                }
                            }
                        }
                    }
                }
            }
        }

          auto itup = map_vp.find(cx);

          if (itup != map_vp.end()) {
            auto it = itup->second.begin();

            for (; it != itup->second.end(); it++) {

                CNode2* n1 = NULL;

                int po1 = it->pr == CPOD ? 2 : 3;
                std::pair<QString, int> para(it->uzel, po1);

                const auto it1 = map_nnn.find(para);
                if (it1 != map_nnn.end()) n1 = it1->second;

                if (n1) {
                    std::map<QString, QVariant> map_v;
                    map_v["nodeID"] = (int)n1->id;
                    map_v["externalSignID"] = 2;
//                    n1->node.nVP = execInsertQ(db, "WDOdevices", map_v);
                    n1->node.nVP = insertIntoDatabase(db, "WDOdevices", map_v);
                }
            }
        }
    }

    nP->node.isP = true;
#endif
}

void init_b5(CGraph2 *m_graph, int id, const QString & schemeNum)
{
#if 1
    std::map<QString, std::map<QString, std::list<PARA> > >::const_iterator it0;
    std::map<QString, std::list<PARA> > ::const_iterator it1;
    std::list<PARA>::const_iterator itl;

    std::map<QString, std::set<NODE> >::const_iterator itn;
    std::map<std::pair<QString, int>, CNode2*> map_nnn;

//    CGraph2 *m_graph = cxema->graph();

//    if (!nP) return;


//    delCxema1(cxema, nP);
//    delCxemaFull(cxema, nP);


//    int id = nP->id;
//    int externalCodeID = nP->node.id_kod;
    int externalCodeID = 1;
    int fileID = 1;

    QString cx = schemeNum;

    int id0  = 100000;

    int internalNodeID = id;

    itn = map_n_b5.find(cx);
    if (itn != map_n_b5.end()) {
        set<NODE>::const_iterator it;

        for (it = itn->second.begin(); it != itn->second.end(); ++it) {
            int po = it->pr == CPOD ? 2 : 3;

            int id2 = 0;
            int typ = TIP_US;

//            CFPoint point(it->x*10, it->y*10);

            CNode2* node2 = m_graph->find_ins(id0++);

            if (node2) {
                node2->node.node_name = CNodeName("", it->uzel, S_OBA);
                node2->node.name = it->uzel;
                node2->node.internalNodeID = internalNodeID;
                node2->node.coord.x = it->x*10;
                node2->node.coord.y = it->y*10;
                node2->node.typ = typ;

                node2->node.externalSignID = po;
                node2->node.id_kod = 1;
                node2->node.fileID = 1;
                node2->node.isPodp = 1;

                pair<QString, int> para(it->uzel, po);
                map_nnn[para] = node2;
            }
        }
    }


    it0 = map_b5.find(cx);
    if (it0 != map_b5.end()) {
        for (it1 = it0->second.begin(); it1 != it0->second.end(); ++it1) {
            QString str;
            int idid = getLineTyp2(it1->first);

            if (idid >= 0) {
                for (itl = it1->second.begin(); itl != it1->second.end(); ++itl) {
                    CCoordList cl;

                    CNode2* n1 = NULL, * n2 = NULL;
                    int po1, po2;

                    po1 = itl->n1.pr == CPOD ? 2 : 3;
                    po2 = itl->n2.pr == CPOD ? 2 : 3;

                    int po;

                    if (po1 == 2 && po2 == 2) po = 2;
                    if (po1 == 2 && po2 == 3) po = 4;
                    if (po1 == 3 && po2 == 2) po = 5;
                    if (po1 == 3 && po2 == 3) po = 3;

                    std::pair<QString, int> para1(itl->n1.uzel, po1);
                    std::pair<QString, int> para2(itl->n2.uzel, po2);

                    const auto it1 = map_nnn.find(para1);
                    if (it1 != map_nnn.end()) n1 = it1->second;

                    const auto it2 = map_nnn.find(para2);
                    if (it2 != map_nnn.end()) n2 = it2->second;

                    std::map<QString, QVariant>  map_v;

                    CCoordList NP;

                    CLine2* line = m_graph->insert_line(n1, n2, NP, true);

                    bool otkl = false;

                    if (idid == TIP_ZD) {
                        if (itl->nomer_mesto == 39 || itl->nomer_mesto == 50) {
                            otkl = true;
                        }
                    }
                    if (idid == TIP_DR) {
//                        map_v["throtDiaphLoc"] = itl->destination;
                    }

                    if (line) {
                        line->line.typ = idid;
                        line->line.pod.isOtkl = otkl;
                        line->line.obr.isOtkl = otkl;
                    }
                }
            }
        }
    }



    auto itup = map_vp.find(cx);

    if (itup != map_vp.end()) {
        auto it = itup->second.begin();

        for (; it != itup->second.end(); it++) {

            CNode2* n1 = NULL;

            int po1 = it->pr == CPOD ? 2 : 3;
            std::pair<QString, int> para(it->uzel, po1);

            const auto it1 = map_nnn.find(para);
            if (it1 != map_nnn.end()) n1 = it1->second;

            if (n1) {
                n1->node.nVP = 1;
//                std::map<QString, QVariant> map_v;
//                map_v["nodeID"] = (int)n1->id;
//                map_v["externalSignID"] = 2;
//                n1->node.nVP = execInsertQ(db, "WDOdevices", map_v);
            }
        }
    }


#endif
}



void create_b5_new(QSqlDatabase &db, CNode2 *nP, CCxema* cxema)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);
    QString q;

     /*

        {4, "b20"}, // Подпорная
        {5, "b14"}, // Вход теплового пункта
        {1, "b23"}, // Отопление
        {2, "b28"}, // Вентиляция (воздушного отопления)
        {3, "b32"}, // ГВС; закрытая
        {37, "b35"}, // На входе подающего трубопровода открытой ГВС
        {6, "b38"}, // Ограничительная в циркуляционном контуре ГВС
        {45, "b36"}, // Подпорно-циркуляционная в открытой ГВС

        {7, "zd7"}, // Вход ТП
        {8, "zd8"}, // Выход ТП
        {27, "zd27"}, // Вход СО
        {48, "zd48"}, // Вход СВ
        {28, "zd28"}, // Вх.II ступен.ГВ

        {36, "zd36"}, // ОткрГВ подача
        {38, "zd38"}, // ОткрГВ обратка
        {39, "zd39"}, // Летняя СО для открытой ГВС обратка
        {49, "zd49"}, // СО посл, предв
        {50, "zd50"}, // Летняя СО посл, предв


*/


    q = QString("select zd7, zd8, zd27, zd48, zd28, zd36, zd38, zd39, zd49, zd50, b20, b14, b23, b28, b32, b35, b38, b36 from net.v_realconsumers where nodeID=%1").arg(nP->id);

    std::vector<QVariant> vv;

    std::map<int, bool> map_open;
    std::map<int, double> map_diam;

    if (readTableValues(db, q, 18, vv)) {
        map_open[7] = vv[0].toInt() == 1;
        map_open[8] = vv[1].toInt() == 1;
        map_open[27] = vv[2].toInt() == 1;
        map_open[48] = vv[3].toInt() == 1;
        map_open[28] = vv[4].toInt() == 1;

        map_open[36] = vv[5].toInt() == 1;
        map_open[38] = vv[6].toInt() == 1;
        map_open[39] = vv[7].toInt() == 1;
        map_open[49] = vv[8].toInt() == 1;
        map_open[50] = vv[9].toInt() == 1;


        map_diam[4] = vv[10].toDouble();
        map_diam[5] = vv[11].toDouble();
        map_diam[1] = vv[12].toDouble();
        map_diam[2] = vv[13].toDouble();
        map_diam[3] = vv[14].toDouble();
        map_diam[37] = vv[15].toDouble();
        map_diam[6] = vv[16].toDouble();
        map_diam[45] = vv[17].toDouble();
    }

//    QString q = QString("select id, externalsign, pih, t from US_OUT WHERE US_OUT.nodeID=%1 and (select MAX(id) from calculation c where c.fileID=N1.fileID)").arg(nP->id);
    q = QString(

"select us_out.id, us_out.externalsign, us_out.pih, us_out.t\n"
"--, n.id, n.externalNodeName\n"
"--, * \n"
"from us_out\n"
"join net.v_nodes n on n.id=us_out.nodeid \n"
"join net.v_realconsumers rc on rc.nodeid =n.id\n"
"where n.removed = 0\n\n"
//"where externalsign  > 4\n"
//"and n.removed = 0\n"
"and n.id=%1\n"
"and us_out.calculationID = (select MAX(id) from calculation c where c.fileID=n.fileID)\n")
    .arg(nP->id);

    qDebug() << q;

    struct PIH_T {
        double pih;
        double t;
    };

    std::map<int, PIH_T> map_piht;

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int externalsign = query.value("externalsign").toInt();
            PIH_T pt;
            pt.pih = query.value("pih").toDouble();
            pt.t = query.value("t").toDouble();
            map_piht[externalsign] = pt;
        }
    }

    q = QString(

"select * from any2_out\n"
"join net.v_nodes n on n.id=any2_out.nodeid \n"
"join net.v_realconsumers rc on rc.nodeid =n.id\n"
"where n.removed = 0\n"
"and n.id=%1\n"
"and any2_out.calculationID = (select MAX(id) from calculation c where c.fileID=n.fileID)\n")
    .arg(nP->id);

    std::map<int, double> map_ras;

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int mesto = query.value("mesto").toInt();
            double ras = query.value("ras").toDouble();

            map_ras[mesto] = ras;
        }
    }


#if 1
    std::map<QString, std::map<QString, std::list<PARA> > >::const_iterator it0;
    std::map<QString, std::list<PARA> > ::const_iterator it1;
    std::list<PARA>::const_iterator itl;

    std::map<QString, std::set<NODE> >::const_iterator itn;
    std::map<std::pair<QString, int>, CNode2*> map_nnn;

    if (!nP) return;

    change_group_start(db, "Создание внутренней схемы");

//    delCxema1(cxema, nP);
    delCxemaFull(cxema, nP);

    int idn = 1;
    int idl = 1;

    idn = 2000000000;
    idl = 2000000000;

    int id = nP->id;
    int externalCodeID = nP->node.id_kod;
    int fileID = nP->node.fileID;
    QString cx = nP->node.schemeNum;

    itn = map_n_b5.find(cx);
    if (itn != map_n_b5.end()) {
        set<NODE>::const_iterator it;

        for (it = itn->second.begin(); it != itn->second.end(); ++it) {
            int po = it->pr == CPOD ? 2 : 3;

            CFPoint point(it->x*10, it->y*10);

            idn += 1;

            CNode2* n = cxema->m_graph->ins(idn);
            if (n) {

                bool ok;
                int idname = it->uzel.toInt(&ok);
                n->node.nomP = -1;
                n->node.nomO = -1;

                if (po == 2) {
                    auto itpih = map_piht.find(idname*2+10);
                    if (itpih != map_piht.end()) {
                        n->node.data.pihP = itpih->second.pih;
                        n->node.data.tempP = itpih->second.t;
                        n->node.nomP = idname*2+10;
                        n->node.nomgP = idname*2+10;
                    }
                }
                if (po == 3) {
                    auto itpih = map_piht.find(idname*2+1+10);
                    if (itpih != map_piht.end()) {
                        n->node.data.pihO = itpih->second.pih;
                        n->node.data.tempO = itpih->second.t;
                        n->node.nomO = idname*2+1+10;
                        n->node.nomgO = idname*2+1+10;
                    }
                }

                n->node.internalNodeID = nP->id;
                n->node.fileID = fileID;
                n->node.coord = point;
                n->node.typ = TIP_US;
                n->node.name = it->uzel;
                n->node.externalSignID = po;

                pair<QString, int> para(it->uzel, po);
                map_nnn[para] = n;
            }
        }
    }

    it0 = map_b5.find(cx);
    if (it0 != map_b5.end()) {
        for (it1 = it0->second.begin(); it1 != it0->second.end(); ++it1) {
            QString str;
            int idid = getLineTyp2(it1->first);

            if (idid >= 0) {
                for (itl = it1->second.begin(); itl != it1->second.end(); ++itl) {
                    CCoordList cl;

                    CNode2* n1 = NULL, * n2 = NULL;
                    int po1, po2;

                    po1 = itl->n1.pr == CPOD ? 2 : 3;
                    po2 = itl->n2.pr == CPOD ? 2 : 3;

                    int po;

                    if (po1 == 2 && po2 == 2) po = 2;
                    if (po1 == 2 && po2 == 3) po = 4;
                    if (po1 == 3 && po2 == 2) po = 5;
                    if (po1 == 3 && po2 == 3) po = 3;

                    std::pair<QString, int> para1(itl->n1.uzel, po1);
                    std::pair<QString, int> para2(itl->n2.uzel, po2);

                    const auto it1 = map_nnn.find(para1);
                    if (it1 != map_nnn.end()) n1 = it1->second;

                    const auto it2 = map_nnn.find(para2);
                    if (it2 != map_nnn.end()) n2 = it2->second;

                    int idv;

                    QString tn = getLineTableMySQL(idid);

                    std::map<QString, QVariant>  map_v;

                    if (idid == TIP_ZD) {
                        if (itl->nomer_mesto == 39 || itl->nomer_mesto == 50) {
                            map_v["damperArmatureStateID"] = 2;
                        }
                    }
                    if (idid == TIP_DR) {
                        map_v["throtDiaphLoc"] = itl->destination;
                    }

                    idl += 1;
                    idv = idl;

                    double diam = 0;
                    bool open = true;

                    if (idid == TIP_ZD) {
                        auto it = map_open.find(itl->nomer_mesto);
                        if (it != map_open.end()) {
                            open = it->second;
                        }
                    }
                    if (idid == TIP_DR || idid == TIP_EL) {
                        auto it = map_diam.find(itl->nomer_mesto);
                        if (it != map_diam.end()) {
                            diam  = it->second;
                        }
                    }

                    CLINE2* line = cxema->m_graph->insert_line(n1, n2, cl);
                    if (line) {
//                        int nodeID1 = itl->n1.uzel.toInt()*2+10+po1-2;
//                        int nodeID2 = itl->n2.uzel.toInt()*2+10+po2-2;

                        auto itt = map_ras.find(itl->nomer_mesto);
//                        auto itt = map_ras.find(std::pair<int, int>(nodeID1, nodeID2));

                        double ras = 0;

                        if (itt != map_ras.end()) {
                            ras = itt->second;
                        }

                        CLine2* l = bline(line);
                        l->line.typ = idid;
                        double dru_home = 0;

                        double pih1 = max(n1->node.data.pihP, n1->node.data.pihO);
                        double pih2 = max(n2->node.data.pihP, n2->node.data.pihO);

                        if (po == SignLine_ob || po == SignLine_pp || po == SignLine_po) {
                            l->line.nomP = idl;
                            l->line.idP2 = idv;
                            l->line.pod.q = ras;
                            l->line.nomP = itl->nomer_mesto;
                            l->line.nomgP = itl->nomer_mesto;
                        }
                        if (po == SignLine_ob || po == SignLine_oo || po == SignLine_op) {
                            l->line.nomO = idl;
                            l->line.idO2 = idv;
                            l->line.obr.q = ras;
                            l->line.nomO = itl->nomer_mesto;
                            l->line.nomgO = itl->nomer_mesto;
                        }
                        l->line.obr.diam = diam;
                        l->line.pod.diam = diam;
                        l->line.pod.dru_home = diam != 0 ? 1 : 0;
                        l->line.obr.dru_home = diam != 0 ? 1 : 0;
                        l->line.pod.isOtkl = !open;
                        l->line.obr.isOtkl = !open;
                    }
                }
            }
        }

          auto itup = map_vp.find(cx);

          if (itup != map_vp.end()) {
            auto it = itup->second.begin();

            for (; it != itup->second.end(); it++) {

                CNode2* n1 = NULL;

                int po1 = it->pr == CPOD ? 2 : 3;
                std::pair<QString, int> para(it->uzel, po1);

                const auto it1 = map_nnn.find(para);
                if (it1 != map_nnn.end()) n1 = it1->second;

                if (n1) {
                    std::map<QString, QVariant> map_v;
                    map_v["nodeID"] = (int)n1->id;
                    map_v["externalSignID"] = 2;
                    n1->node.nVP = 1; //execInsertQ(db, "WDOdevices", map_v);
                }
            }
        }
    }

    nP->node.isP = true;
#endif

    change_group_end(db);
}
