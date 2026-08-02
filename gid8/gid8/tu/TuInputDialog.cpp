#include "TuInputDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QStandardItem>
#include <QHeaderView>
#include <QTreeWidgetItem>

#include "db/db.h"
#include <cxema/Graph.h>
#include <cxema/graph2.h>
#include <cxema/cxema1.h>
#include <geo/Klassif.h>
#include "cxema/undo.h"

#include <property/PropertyDial.h>

#define ZDAN "Здания ТУ"

QString transl(const QString & s);


void set_list(QSqlDatabase &db, QComboBox *field, const QString &q)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    field->setEditable(true);
    field->setFixedWidth(400);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            QString s = query.value(0).toString();
            field->addItem(s);
        }
    }
}

void set_list2(QSqlDatabase &db, QComboBox *field, const QString &q, int id0)
{
    QSqlQuery query(db);
    query.setForwardOnly(true);

    int i = 0;

    field->setEditable(false);
    field->setFixedWidth(400);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value(0).toInt();

            QString s = query.value(1).toString().trimmed();

            int NN = 40;



            if (s.size() > NN) {
                field->addItem(s.left(NN) + "...", id);
            }
            else {
                field->addItem(s, id);
            }


            if (id == id0) {
                field->setCurrentIndex(i);
            }
            i++;
        }
    }
}


static std::set<int> m_org;
static set<int> m_del;

struct ProcTu
{
    ProcTu() {
        proc = 0;
    }
    double proc;
};


struct TuRow
{
  TuRow()
  {
    date, addr = name = nazn = kontr = nomer = "";
    Qot = Qgvs = Qvent = Qpar = 0;
    Q = Qot = Qgvs = Qvent = Qpar = 0;
    Q_pr = Qot_pr = Qgvs_pr = Qvent_pr = Qpar_pr = 0;
  }

  int id;
  int zh;

  QString sost;
  QString date;
  QString addr;
  QString name;  // Наименование объекта
  QString nazn;  // Назначение объекта
  QString kontr; // Наименование контрагента
  QString nomer; // Номер договора
  
  double Q, Qot, Qgvs, Qvent, Qpar;
  double Q_pr, Qot_pr, Qgvs_pr, Qvent_pr, Qpar_pr;

  std::map<CNode2*, ProcTu> map_proc;
  std::map<CNode2*, ProcTu> map_proc2;
};

void read_proc(QSqlQuery &query, std::map<CNode2*, ProcTu> &map_proc, list<CNode2 *> &m_list_node)
{
    QString s, s1, s2, s3;

    map_proc.clear();

    for (int i = 0; i < 5; i++) {
        QString kod = query.value(QString("kod%1").arg(i + 1)).toString();
        QString uzel = query.value(QString("uzel%1").arg(i + 1)).toString();

        ProcTu p;

        p.proc = query.value(QString("protsent_nagruzki %1").arg(i + 1)).toDouble();

#if 0

        list<CNode2 *>::const_iterator it = m_list_node.begin();

        for (; it != m_list_node.end(); it++) {
            CNode2 *n = (*it);
#if 0
            if (kod == n->node_name.kod && uzel == n->node_name.name) {
                map_proc[n] = p;
            }
#endif
        }
#endif

    }
}

QString readSost(int sostID)
{
    QString sost = "Д";

    switch (sostID) {
    case 1: sost = "Д"; break;
    case 2: sost = "А"; break;
    case 3: sost = "М"; break;
    case 4: sost = "З"; break;
    }
    return sost;
}



QString readSost(QSqlQuery &query, const QString &fn)
{
    QString sost = "Д";
    long sostID = query.value(transl(fn)).toInt();

    switch (sostID) {
    case 1: sost = "Д"; break;
    case 2: sost = "А"; break;
    case 3: sost = "М"; break;
    case 4: sost = "З"; break;
    }
    return sost;
}


std::vector<TuRow> v_nagr;
std::list<CNode2 *> m_list_node;

QString read_n(QSqlQuery &query, const QString &frm, int n)
{
    QString s = QString(frm).arg(n);
    s = transl(s);

    return query.value(s).toString();
}

double read_n_double(QSqlQuery &query, const QString &frm, int n)
{
    QString s = QString(frm).arg(n);
    s = transl(s);

    auto qqq = query.value(s);

    return query.value(s).toDouble();
}



bool read_ism(QSqlQuery &query, int n, TuRow *nr)
{
    if (n == 0) {
        read_n(query, "Изменения продления %1", n);
        nr->date = read_n(query, "Дата выдачи ТУ", n);
        nr->Q = read_n_double(query, "Тепловые потоки, Гкал/ч", n);
        nr->Qot = read_n_double(query, "В том числе отопление", n);
        nr->Qvent = read_n_double(query, "В том числе вентиляция", n);
        nr->Qgvs = read_n_double(query, "В том числе ГВС макс", n);
        nr->Q_pr = read_n_double(query, "Прирост нагрузки", n);
        nr->Qot_pr = read_n_double(query, "В том числе прирост отопление", n);
        nr->Qvent_pr = read_n_double(query, "В том числе прирост вентиляция", n);
        nr->Qgvs_pr = read_n_double(query, "В том числе прирост ГВС макс", n);
        read_n(query, "Дополнительные технические мероприятия", n);
    }
    else {
        read_n(query, "Изменения продления %1", n);
        nr->date = read_n(query, "Дата изменения продления %1", n);
        nr->Q = read_n_double(query, "Тепловые потоки, Гкал/ч %1", n);
        nr->Qot = read_n_double(query, "В том числе отопление %1", n);
        nr->Qvent = read_n_double(query, "В том числе вентиляция %1", n);
        nr->Qgvs = read_n_double(query, "В том числе ГВС макс %1", n);
        nr->Q_pr = read_n_double(query, "Прирост нагрузки %1", n);
        nr->Qot_pr = read_n_double(query, "В том числе прирост отопление %1", n);
        nr->Qvent_pr = read_n_double(query, "В том числе прирост вентиляция %1", n);
        nr->Qgvs_pr = read_n_double(query, "В том числе прирост ГВС макс %1", n);
        read_n(query, "Дополнительные технические мероприятия %1", n);
    }

    return nr->date != "";
}

void read_ism(QSqlQuery &query, TuRow *nr)
{
    TuRow nr1;

    read_ism(query, 0, nr);

    if (nr->Q_pr == 0) {
        for (int n = 1; n <= 7; n++) {
            TuRow nr1;

            read_ism(query, n, &nr1);
            if (nr1.date == "") break;

            *nr = nr1;
        }
    }
}



void init0(QSqlDatabase &db)
{
    Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
    if (!kls) return;

    QString q;

    v_nagr.clear();

    QString where = "";

    int f = 0;

    std::set<int>::const_iterator it = m_org.begin();

    for (; it != m_org.end(); it++) {
      QString s = QString("ID=%1").arg(*it);
      if (f == 0) {
        f++;
        where = " WHERE "+s;
      }
      else {
        where += " OR ";
        where += s;
      }
    }

    if (where != "") {

        q = QString("SELECT id, * FROM %1").arg(transl("Технические условия"));
        q = q + where;

        QSqlQuery query(db);
        query.setForwardOnly(true);

        if (query_exec(db, query, q)) {
            while (query.next()) {
                TuRow nr;

                read_proc(query, nr.map_proc, m_list_node);
                read_ism(query, &nr);

                nr.nomer = query.value(transl("Номер ТУ")).toString();
    //          nr.sost = query.value(transl("Состояние договора")).toString();
                nr.sost = readSost(query, transl("Состояние договора"));

                nr.date = query.value(transl("Дата выдачи ТУ")).toString();
                nr.addr = query.value(transl("Адрес объекта")).toString();
                nr.nazn = query.value(transl("Назначение объекта")).toString();
                nr.kontr = query.value(transl("Наименование организации, запрашивающей ТУ")).toString();
                nr.name = query.value(transl("Наименование объекта")).toString();

                nr.id = query.value("ID").toInt();
                nr.zh = 0;

                v_nagr.push_back(nr);
            }
        }
    }
}


static double Qot, Qgvs, Qvent;
const CNode2 *m_nodeTU = 0;
int m_n_el;


void getPotr(CCxema *cxema, CCoordList &cl, QTreeWidget *tree2, QStandardItemModel *tableModel)
{
    Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
    if (!kls) return;


    double Qot = 0, Qgvs = 0, Qvent = 0;

    map<int, map<int, map<CNode2 *, Nagr3> > > map_nagr3;


    map_nagr3.clear();


    CGraph2 *graph = cxema->m_graph;

//  CMapGraph::iterator p = graph->map_node.begin();

    auto p = cxema->m_graph->map_node.begin();

    while (p != cxema->m_graph->map_node.end()) {
        CNode2 *n = p->second;

        if ((n->node.typ == TIP_PR || n->node.typ == TIP_PO) && n->node.coord.x != 0 && n->node.coord.x != 0 ) {

            if (cl.find3(n->node.coord)) {

                if (!m_nodeTU) m_nodeTU = n;

                QString str;

                m_list_node.push_back(n);

                Qot   += n->node.Qot  ;
                Qgvs  += n->node.Qgvs ;
                Qvent += n->node.Qvent;

                QTreeWidgetItem *h = new QTreeWidgetItem(tree2);
                h->setText(0, n->node.name);


        //        HTREEITEM h = m_tree.InsertItem(n->node.name, m_el);
        //        m_tree.SetItemData(h, (DWORD) n );
        //        HTREEITEM h1 = m_tree.InsertItem("Расчетная", h);
                QTreeWidgetItem *h1 = new QTreeWidgetItem(h);
                h1->setText(0, "Расчетная");

                {
                    if (n->node.Qot > 0) {
                        str = QString(" %1").arg(n->node.Qot);
                        QTreeWidgetItem *h11 = new QTreeWidgetItem(h1);
                        h11->setText(0, "Отопление" + str);
                    }
                    if (n->node.Qvent > 0) {
                        str = QString(" %1").arg(n->node.Qvent);
                        QTreeWidgetItem *h13 = new QTreeWidgetItem(h1);
                        h13->setText(0, "Вентиляция" + str);
                  }
                  if (n->node.Qgvs > 0) {
                      str = QString(" %1").arg(n->node.Qgvs);
                      QTreeWidgetItem *h12 = new QTreeWidgetItem(h1);
                      h12->setText(0, "ГВС" + str);
        //            HTREEITEM h12 = m_tree.InsertItem("ГВС"+str, h1);
                  }
                }
               
        //        HTREEITEM h3 = m_tree.InsertItem("Прирост", h);

        //        map_h[n] = h3;

                m_n_el ++;
            }
        }
        p++;
    }

    double Q = Qot+Qgvs+Qvent;

    int col = 2;
    tableModel->setItem(0, col, new QStandardItem(QString("%1").arg(Qot)));
    tableModel->setItem(1, col, new QStandardItem(QString("%1").arg(Qvent)));
    tableModel->setItem(2, col, new QStandardItem(QString("%1").arg(Qgvs)));
    tableModel->setItem(3, col, new QStandardItem(QString("%1").arg(Q)));



    double Qot_pr = 0, Qvent_pr = 0, Qgvs_pr = 0, Qpar_pr = 0;

    {
        vector<TuRow>::const_iterator it = v_nagr.begin();

        for (; it != v_nagr.end(); it++) {
            TuRow nr = *it;
            if (nr.sost == "Д") {
                Qot_pr += nr.Qot_pr;
                Qvent_pr += nr.Qvent_pr;
                Qgvs_pr += nr.Qgvs_pr;
                Qpar_pr += nr.Qpar_pr;
            }
        }
    }


//    s = m_grid2.GetTextMatrix(1, 1); double Q_r_ot = atof(s);
//    s = m_grid2.GetTextMatrix(2, 1); double Q_r_vent = atof(s);
//    s = m_grid2.GetTextMatrix(3, 1); double Q_r_gvs = atof(s);
//    s = m_grid2.GetTextMatrix(4, 1); double Q_r = atof(s);

    double Q_pr = Qot_pr + Qgvs_pr + Qvent_pr;

    // Сумма расчетной и общей
//    s.Format("%.12g", Q_r_ot + Qot_pr);   m_grid2.SetTextMatrix(1, 3, s);
//    s.Format("%.12g", Q_r_vent + Qvent_pr); m_grid2.SetTextMatrix(2, 3, s);
//    s.Format("%.12g", Q_r_gvs + Qgvs_pr);  m_grid2.SetTextMatrix(3, 3, s);
//    s.Format("%.12g", Q_r + Q_pr); m_grid2.SetTextMatrix(4, 3, s);

    col = 3;
    tableModel->setItem(0, col, new QStandardItem(QString("%1").arg(Qot + Qot_pr)));
    tableModel->setItem(1, col, new QStandardItem(QString("%1").arg(Qvent + Qvent_pr)));
    tableModel->setItem(2, col, new QStandardItem(QString("%1").arg(Qgvs + Qgvs_pr)));
    tableModel->setItem(3, col, new QStandardItem(QString("%1").arg(Q + Q_pr)));

}

#if 0


void CTu2::initTree() 
{

  map_h.clear();

  m_nodeTU = 0;

  CGidrView *pView = getView();
  if (!pView) return;

  Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
  if (!kls) return;

  m_tree.DeleteAllItems();

  m_n_el = 0;

  m_el = m_tree.InsertItem("Элеваторы", 0);

  Qot = 0, Qgvs = 0, Qvent = 0;

  map_nagr3.clear();

  CCxema *m_cxema = pView->m_cxema;

  getPotr(m_cxema, m_cl, m_el, kls);

  m_tree.Expand(m_el, TVE_EXPAND);

  CString s;

//  Расчетная

  double Q = Qot+Qgvs+Qvent;

  s.Format("%.12g", Qot);   m_grid2.SetTextMatrix(1, 1, s);
  s.Format("%.12g", Qvent); m_grid2.SetTextMatrix(2, 1, s);
  s.Format("%.12g", Qgvs);  m_grid2.SetTextMatrix(3, 1, s);
  s.Format("%.12g", Q); m_grid2.SetTextMatrix(4, 1, s);
}

#endif

bool readWKB(QVariant &var, CCoordList &cl);


void TuInputDialog::onPropertyAccepted()
{
    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        if (update_db(dlg->map_edited, dlg->getUser())) {
            repaint();
        }
    }
}

void TuInputDialog::editTU(int id)
{
    QString tn = transl("Технические условия");
    QString title = "Технические условия";

    auto *dlg = new PropertyDial(this, m_cxema->m_db, title, tn, tn, id, -1, pr_type_any, m_cxema);
    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
}


void TuInputDialog::onEdit()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int id = action->data().toInt();
        editTU(id);
    }
}


void TuInputDialog::onDelete()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {

        Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
        if (!kls) return;

        int id = action->data().toInt();

        QString tn = transl("Технические условия");
        QString title = "Технические условия";


        int m_row = 0;

        for (m_row = 0; m_row < v_nagr.size(); m_row++ ) {
            TuRow nr = v_nagr[m_row];
            if (nr.id == id) {
                QString s1 = nr.name;
                QString s = QString("Удалить %1 ? Внимание ТУ будет удалено из Базы данных!").arg(s1);

                if (QMessageBox::question(this, "", s)  == QMessageBox::Yes) {
                  m_org.erase(nr.id);
                  m_del.insert(nr.id);
                  init0(*kls->m_db);
                  init();
//                  m_isEdit = !m_isReadOnly;
//                  init();
                }

                break;
            }
        }

#if 0
        if (m_row < 0 || m_row >= v_nagr.size()) return;
        TuRow nr = v_nagr[m_row];

        QString s1 = nr.name;
        QString s = QString("Удалить %1 ? Внимание ТУ будет удалено из Базы данных!").arg(s1);

        if (QMessageBox::question(this, "", s)  == QMessageBox::Yes) {
          m_org.erase(nr.id);
          m_del.insert(nr.id);
          m_isEdit = !m_isReadOnly;
          init();
        }
#endif
    }
}

void TuInputDialog::onNenaid()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int id = action->data().toInt();

        QString tn = transl("Технические условия");
        QString title = "!!!";

        auto *dlg = new PropertyDial(this, m_cxema->m_db, title, tn, tn, id, -1, pr_type_any, m_cxema);
        dlg->show();

        connect(dlg, SIGNAL(accepted()), this, SLOT(onPropertyAccepted()));
    }
}

QString readSost(int sostID);


void TuInputDialog::change_tu(int tu_typ)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {

        Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
        if (!kls) return;

        int id = action->data().toInt();

        int m_row = 0;

        for (m_row = 0; m_row < v_nagr.size(); m_row++ ) {
            TuRow nr = v_nagr[m_row];
            if (nr.id == id) {
                QString s1 = nr.name;

                nr.sost = readSost(tu_typ);

//                m_org.erase(nr.id);
//                m_del.insert(nr.id);
                init0(*kls->m_db);
                init();

                break;
            }
        }


/*
        QString d1 = "";

        CControl *c1 = getControl("Дата аннулирования");
        if (c1) {
            d1 = getValue(c1);
            ActivateFrame(c1->m_frame);
            c1->m_wnd->SetFocus();
            QMessageBox::warning(this, "", "Введите дату операции");
        }

        CControl *cntrl = getControl("Состояние договора");
        if (cntrl) {
            setValue(cntrl, op);
        }
        //  if (saveDialog())
        //    CDialog::OnOK();
*/


    }
}



void TuInputDialog::customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *nd = tree1->itemAt( pos );
    if (!nd) return;

    int id = nd->data(0, Qt::UserRole).toInt();

    if (id > 0) {
        QMenu *menu = new QMenu(this);
        QAction *a;

        a = new QAction(tr("Редактировать"), this);
        a->setData(QVariant::fromValue(id));
        connect(a, SIGNAL(triggered()), this, SLOT(onEdit()));
        menu->addAction(a);

        a = new QAction(tr("Удалить"), this);
        a->setData(QVariant::fromValue(id));
        connect(a, SIGNAL(triggered()), this, SLOT(onDelete()));
        menu->addAction(a);

        a = new QAction(tr("Поместить в список ненайденных"), this);
        a->setData(QVariant::fromValue(id));
        menu->addAction(a);
        connect(a, SIGNAL(triggered()), this, SLOT(onNenaid()));


        menu->addSeparator();

        if (1) {
            a = new QAction(tr("Аннулировать"), this);
            a->setData(QVariant::fromValue(id));
            menu->addAction(a);
            connect(a, &QAction::triggered, this,  [this]() {
                change_tu(2);
            });
        }

        if (1) {
            a = new QAction(tr("Завершить"), this);
            a->setData(QVariant::fromValue(id));
            menu->addAction(a);
            connect(a, &QAction::triggered, this,  [this]() {
                change_tu(4);
            });
        }
            
        if (1) {
            a = new QAction(tr("Действующий"), this);
            a->setData(QVariant::fromValue(id));
            menu->addAction(a);
            connect(a, &QAction::triggered, this,  [this]() {
                change_tu(1);
            });
        }


        menu->popup(tree1->viewport()->mapToGlobal(pos));
    }
}

void TuInputDialog::init()
{
    Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
    if (!kls) return;

//    auto *db = &kls->m_db;

    init0(*kls->m_db);

    init2();
}

void TuInputDialog::init2()
{
    qDeleteAll(iD->takeChildren());
    qDeleteAll(iA->takeChildren());
    qDeleteAll(iZ->takeChildren());

    double Qot0 = 0, Qvent0 = 0, Qgvs0 = 0, Qpar0 = 0;

    for (int r = 0; r < v_nagr.size(); r++) {
        TuRow nr = v_nagr[r];

        QTreeWidgetItem *i0 = iD;

        if (nr.sost == "Д") {
            i0 = iD;
        }
        else if (nr.sost == "А") {
            i0 = iA;
        }
        else if (nr.sost == "М") {
            i0 = iD;
        }
        else if (nr.sost == "З") {
            i0 = iZ;
        }

        QString s = QString("%1 %2 %3 %4 %5 %6").arg(nr.nomer, nr.date, nr.addr, nr.name, nr.nazn, nr.kontr);

        QTreeWidgetItem *i1 = new QTreeWidgetItem(i0);
        i1->setText(0, s);
        i1->setData(0, Qt::UserRole, QVariant::fromValue(nr.id));


//      HTREEITEM hh = m_tree2.InsertItem(s, h0);
//      m_tree2.SelectItem(hh);
//      m_tree2.SetItemData(hh, (DWORD)  r);

      if (nr.Qot_pr != 0) {
        s = QString("Отопление : %1 Гкал").arg(nr.Qot_pr);
        QTreeWidgetItem *i = new QTreeWidgetItem(i1);
        i->setText(0, s);
      }
      if (nr.Qvent_pr != 0) {
        s = QString("Вентиляция : %1 Гкал").arg(nr.Qvent_pr);
        QTreeWidgetItem *i = new QTreeWidgetItem(i1);
        i->setText(0, s);
      }
      if (nr.Qgvs_pr != 0) {
        s = QString("ГВС : %1 Гкал").arg(nr.Qgvs_pr);
        QTreeWidgetItem *i = new QTreeWidgetItem(i1);
        i->setText(0, s);
      }
      if (nr.Qpar_pr != 0) {
        s = QString("Пар : %1 Гкал").arg(nr.Qpar_pr);
        QTreeWidgetItem *i = new QTreeWidgetItem(i1);
        i->setText(0, s);
      }
    
      if (nr.sost == "Д") {
        Qot0 += nr.Qot_pr;
        Qvent0 += nr.Qvent_pr;
        Qgvs0 += nr.Qgvs_pr;
        Qpar0 += nr.Qpar_pr;
      }
    }

    tree1->expandAll();

    int col = 1;
    tableModel->setItem(0, col, new QStandardItem(QString("%1").arg(Qot0)));
    tableModel->setItem(1, col, new QStandardItem(QString("%1").arg(Qvent0)));
    tableModel->setItem(2, col, new QStandardItem(QString("%1").arg(Qgvs0)));
    tableModel->setItem(3, col, new QStandardItem(QString("%1").arg(Qpar0)));

}


TuInputDialog::TuInputDialog(QWidget *parent,
QSqlDatabase &db, const QString & title, const QString & table, int id, CCxema *cxema
)
    : QDialog(parent)
    , save(this, "TuInputDialog")
{

//    init_tu_find(db);

    m_cxema = cxema;
    
    QString q;

    m_org.clear();
    m_del.clear();

    q = QString("SELECT ID FROM %1 WHERE %2 = %3")
            .arg(transl("Технические условия"), transl("Здание"))
            .arg(id);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            m_org.insert(id);
        }
    }

    init0(db);


//      m_ist0 = ado->read_long(transl("Источник"));
//      m_rs0 = ado->read_long(transl("Район эксплуатации"));
//      m_comment = ado->readStr(transl("Комментарий"));

    Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
    if (!kls) return;

    QString wkb_str = "";


    if (is_POSTGRESQL()) {
        wkb_str = QString("ST_AsBinary(%1)").arg(kls->shape);
    }
    else {
        wkb_str = QString("%1.STAsBinary()").arg(kls->shape);
    }



    q = QString("select distinct Name, Name_2 from %1").arg(transl("Улицы"));


    if (query_exec(db, query, q)) {
        while (query.next()) {
            QString s1 = query.value(0).toString();
            QString s2 = query.value(1).toString();
            map_str1[s1] = s2;

            if (m_street == s2) m_street = s1;
//        m_ctrl_street.AddString(s1);
      }
  }




    q = QString("select " 
                " id, gorod, mikrorayon, ulitsa, dom, " 

                " istochnik, rayon_ekspluatatsii, kommentariy, kod_rs_uzla_prisoedineniya, uzel_prisoedineniya, %3"
                "  from %1 where id=%2").arg(table).arg(id).arg(wkb_str);
    
    std::vector<QVariant> vv;

    m_n1 = id;

    CCoordList cl;

    if (readTableRowValues(db, q, vv)) {
        m_gorod       = vv[1].toString();
        m_raj         = vv[2].toString();
        m_street      = vv[3].toString();
        m_dom         = vv[4].toString();
        m_ist0        = vv[5].toInt();
        m_rs0         = vv[6].toInt();
        m_comment     = vv[7].toString();
        m_kod         = vv[8].toString();
        m_uzel        = vv[9].toString();
        readWKB(vv[10], cl);
    }


  
  

/*

  int n_ist = 0;
  int n_rs = 0;

  q = QString("SELECT * FROM %1 ORDER BY %2").arg(transl("Источники тепла"), transl("Наименование"));

  if (ado->openTable0(q)) {
      while ( !ado->isEOF() ) {
        long id = ado->read_long("ID");
        CString s = ado->readStr(transl("Наименование"));
        if (id == m_ist0 || (m_ist0 == 0 && s == ist)) {
          m_ist = n_ist;
        }
        m_ctrl_ist.AddString(s);
        ado->MoveNext();
        n_ist++;
      }
    ado->closeTable();
  }

  q = QString("SELECT * FROM %1 ORDER BY %2").arg(transl("Район эксплуатации"), transl("Наименование района эксплуатации источника тепла"));

  if (ado->openTable0(q)) {
      while ( !ado->isEOF() ) {
        long id = ado->read_long("ID");
        CString s = ado->readStr(transl("Наименование района эксплуатации источника тепла"));
        if (id == m_rs0 || (m_rs0 == 0 && s == rn)) {
          m_rs2 = n_rs;
        }
        m_ctrl_rs2.AddString(s);
        ado->MoveNext();
        n_rs++;
      }
    ado->closeTable();
  }

*/



    

    // ---- Поля ----
    field1 = new QComboBox(); 
    q = QString("select distinct %2 from %1 order by %2").arg(transl("Микрорайоны")).arg("name");
    set_list(db, field1, q);
    field1->setEditText(m_raj);

    field2 = new QComboBox(); 
    q = QString("select distinct %2 from %1 order by %2").arg(transl("Улицы")).arg("name_2");
    set_list(db, field2, q);
    field2->setEditText(m_street);

    field3 = new QLineEdit(); field3->setText(m_dom);
    
    field4 = new QComboBox(); 
    q = QString("select distinct %1 FROM %2").arg(transl("Код РС узла присоединения"), transl("Здания ТУ"));
    set_list(db, field4, q);
    field4->setEditText(m_kod);

    
    field5 = new QComboBox();
    q = QString("select distinct %1 FROM %2").arg(transl("Узел присоединения"), transl("Здания ТУ"));
    set_list(db, field5, q);
    field5->setEditText(m_uzel);


    field6 = new QComboBox(); 
    q = QString("SELECT id, %2 FROM %1 ORDER BY %2").arg(transl("Район эксплуатации"), transl("Наименование района эксплуатации источника тепла"));
    set_list2(db, field6, q, m_rs0);

    
    field_ist = new QComboBox(); field_ist->setEditable(true);
    q = QString("SELECT id, %2 FROM %1 ORDER BY %2").arg(transl("Источники тепла"), transl("Наименование"));
    set_list2(db, field_ist, q, m_ist0);
    

//    field_ist->addItems({"Опция 7.1", "Опция 7.2"});

    // ---- Кнопки ----
    okButton = new QPushButton("OK");
    cancelButton = new QPushButton("Отмена");

    b2Button = new QPushButton("Таблица");



    b4Button = new QPushButton("Добавить новое ТУ");
    b5Button = new QPushButton("Добавить ТУ из списка");
    b6Button = new QPushButton("Сохранить нагрузки");
    
//    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(okButton, &QPushButton::clicked, this, &TuInputDialog::okClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(b4Button, &QPushButton::clicked, this, &TuInputDialog::onAddTUNew);
    connect(b5Button, &QPushButton::clicked, this, &TuInputDialog::onAddTUOld);


    // ---- Таблица ----
    tableView = new QTableView();
    tableModel = new QStandardItemModel(4, 4, this);


    tableModel->setVerticalHeaderLabels({
//    "Нагрузка", 
    "Отопление", "Вентиляция", "ГВС", "Суммарная"
    });

    tableModel->setHorizontalHeaderLabels({
        "Расчетная", "Прирост", "Перспективная", "Договорная"
    });
    

    tableView->horizontalHeader()->setStyleSheet(
        "QHeaderView::section { background-color: lightgray; color: black; font-weight: bold; }"
    );
    tableView->verticalHeader()->setStyleSheet(
        "QHeaderView::section { background-color: #d3d3d3; color: black; font-weight: bold; }"
    );

    
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            tableModel->setItem(row, col, new QStandardItem(QString("%1-%2").arg(row).arg(col)));


    tableView->setModel(tableModel);
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // ---- Форма слева ----
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Микрорайон:", field1);
    formLayout->addRow("Улица:", field2);
    formLayout->addRow("Дом:", field3);
    formLayout->addRow("Расчетная схема:", field4);
    formLayout->addRow("Камера:", field5);
    formLayout->addRow("Район:", field6);
    formLayout->addRow("Источник:", field_ist);

    QWidget *formWidget = new QWidget();
    formWidget->setLayout(formLayout);

    // ---- Таблица и кнопки справа от неё ----
    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    QVBoxLayout *buttonsLayout2 = new QVBoxLayout();

    
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(b2Button);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addStretch();
    buttonsLayout2->addWidget(b4Button);
    buttonsLayout2->addWidget(b5Button);
    buttonsLayout2->addWidget(b6Button);
    buttonsLayout2->addStretch();

    QHBoxLayout *tableWithButtons = new QHBoxLayout();
    tableWithButtons->addWidget(tableView, 3);
    tableWithButtons->addLayout(buttonsLayout, 1);
    tableWithButtons->addLayout(buttonsLayout2, 1);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addLayout(tableWithButtons);

    // ---- Верхняя часть (форма + таблица с кнопками) ----
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(formWidget, 1);
    topLayout->addLayout(rightLayout, 2);

    // ---- Деревья ----
    tree1 = new QTreeWidget();
    tree1->setStyle(QStyleFactory::create("windows"));
//    tree1->setHeaderLabels(QStringList() << "" << "");
//    tree1->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Действующие"));
//    tree1->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Аннулированные"));
//    tree1->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Завершенные"));

    tree1->setHeaderHidden(true);
    tree1->setHeaderLabels({ tr("") });
    tree1->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree1, &QTreeWidget::customContextMenuRequested, this, &TuInputDialog::customContextMenuRequested);


    iD = new QTreeWidgetItem(tree1);
    iD->setText(0, "Действующие");

    iA = new QTreeWidgetItem(tree1);
    iA->setText(0, "Аннулированные");

    iZ = new QTreeWidgetItem(tree1);
    iZ->setText(0, "Завершенные");

    init2();


#if 0

    double Qot0 = 0, Qvent0 = 0, Qgvs0 = 0, Qpar0 = 0;

    {
        for (int r = 0; r < v_nagr.size(); r++) {
            TuRow nr = v_nagr[r];

            map<CNode2*, ProcTu>::iterator it = nr.map_proc2.begin();

            for (; it != nr.map_proc2.end(); it++) {
                CNode2 *n = it->first;
                double pr = it->second.proc;
                Qot0 += nr.Qot*pr / 100;
                Qgvs0 += nr.Qgvs*pr / 100;
                Qvent0 += nr.Qvent*pr / 100;
                Qpar0 += nr.Qpar*pr / 100;
            }
        }
    }
#endif


    tree2 = new QTreeWidget();
    tree2->setStyle(QStyleFactory::create("windows"));

    tree2->setHeaderHidden(true);
    tree2->setHeaderLabels({ tr("") });
    
    getPotr(cxema, cl, tree2, tableModel);


    tree2->expandAll();


//    tree2->setHeaderLabels(QStringList() << "Дерево 2 - Колонка A" << "Колонка B");
//    tree2->addTopLevelItem(new QTreeWidgetItem(QStringList() << "Пункт 2.1" << "Значение"));


//    initTree();




    // ---- Главный макет ----
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(tree1);
    mainLayout->addWidget(tree2);

    setLayout(mainLayout);
    setWindowTitle("Технические условия.");
//    resize(900, 600);
}

void TuInputDialog::read_data()
{
    m_raj = field1->currentText();
    m_street = field2->currentText();
    m_dom = field3->text();
}


void TuInputDialog::okClicked()
{
//    m_sum = calcSum();

    if (QMessageBox::question(0, "", QString(tr("Сохранить изменения?"))) != QMessageBox::Yes) return;

    QString q;

    Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
    if (!kls) return;

    int m_ist = 0;

#if 0

//    QString q = QString("SELECT id FROM %s ORDER BY %s").arg(transl("Источники тепла"), transl("Наименование"));

    q = QString(R"(      
SELECT *
FROM (
    SELECT p.id, ROW_NUMBER() OVER (ORDER BY %2) AS rn
    FROM %1 p
) sub
WHERE rn = %3;
    )")
            .arg(transl("Источники тепла"), transl("Наименование"))
            .arg(m_ist);


    m_ist0 = readTableVariant(*kls->m_db, q).toInt();


    q = QString(R"(      
SELECT *
FROM (
    SELECT p.id, ROW_NUMBER() OVER (ORDER BY %2) AS rn
    FROM %1 p
) sub
WHERE rn = %3;
    )")
    .arg(transl("Район эксплуатации"), transl("Наименование района эксплуатации источника тепла"))
    .arg(m_rs2);


    auto it1 = map_str1.find(m_street);

    if (it1 != map_str1.end()) {
        m_street = it1->second;
    }

    q.Format("UPDATE %s SET %s = 0 WHERE %s = %d", transl("Технические условия"), transl("Здание"), transl("Здание"), m_n1);
    ado->Execute(q);

    set<int>::const_iterator it = m_org.begin();
    for (; it != m_org.end(); it++) {
      q = QString("UPDATE %1 SET %2 = %3 WHERE id = %4")
        .arg(transl("Технические условия"), transl("Здание")).arg(m_n1).arg(*it);

      ado->Execute(q);
    }

    {
      set<int>::const_iterator it = m_del.begin();
      for (; it != m_del.end(); it++) {
        q.Format("DELETE FROM %s WHERE id = %d", transl("Технические условия"), *it);
        ado->Execute(q);
      }
    }


    CString ist = "", rn = "";

    if (m_ctrl_ist.GetCurSel() >= 0) m_ctrl_ist.GetLBText( m_ctrl_ist.GetCurSel() , ist );
    if (m_ctrl_rs2.GetCurSel() >= 0) m_ctrl_rs2.GetLBText( m_ctrl_rs2.GetCurSel() , rn );


#endif

#if 0

    q = QString("UPDATE %1 SET %2 = '%3', %4 = '%5' WHERE %6 = %7") 
        .arg(transl("Технические условия"))
        .arg(transl("Источник"))
        .arg(ist)
        .arg(transl("Район эксплуатации"))
        .arg(rn)
        .arg(transl("Здание"))
        .arg(m_n1);
#endif

     change_group_start(*kls->m_db, "Изменение ТУ");


//    query_exec(db, query, q);

    for (int zd : m_org) {
        QString q = QString("UPDATE %1 SET %2 = %3 WHERE id = %4")
            .arg(transl("Технические условия"), transl("Здание"))
            .arg(m_n1)
            .arg(zd);

        query_exec(*kls->m_db, q);
    }

    for (int id_tu : m_del) {
        QString q = QString("DELETE FROM %1 WHERE id = %2")
            .arg(transl("Технические условия"))
            .arg(id_tu);
        query_exec(*kls->m_db, q);
    }


    read_data();

    QMap<QString, QVariant> data;

/////    data[transl("Город")] = m_gorod;
    data[transl("Микрорайон")] = m_raj;
    data[transl("Улица")] = m_street;
    data[transl("Дом")] = m_dom;
/////    data[transl("Узел присоединения")] = m_uzel;
    //data[transl("Код РС узла присоединения")] = m_rs;
/////    data[transl("Источник")] = m_ist0;
/////    data[transl("Район эксплуатации")] = m_rs0;
  //  data[transl("Камера")] = m_uzel;
/////    data[transl("Комментарий")] = m_comment;

    updateRow(*kls->m_db, transl(ZDAN), m_n1, data);


  //  read_proc(m_grid1, m_list_node, v_nagr, m_n_rows);

  #if 0
    for (int r = 0; r < v_nagr.size(); r++) {
      CString s, q;
      int c = nnn;

      int zh = v_nagr[r].zh;
      int id = v_nagr[r].id;

      map<const CNode*, ProcTu>::iterator it = v_nagr[r].map_proc2.begin();

      CString qq = "";

      for ( int i = 0; it != v_nagr[r].map_proc2.end() && i < 5; it++, i++) {
        const CNode *n = it->first;
        double pr = it->second.proc;

        CString ss;
        ss.Format("[kod%d]='%s', [uzel%d]='%s', [Процент нагрузки %d]=%g ", i+1, n->node_name.kod, i+1, n->node_name.name, i+1, pr);

        if (i != 0) qq += ",";
        qq += ss;

      }
      for ( ; i < 5; i++) {
        CString ss;
        ss.Format("[kod%d]=NULL, [uzel%d]=NULL, [Процент нагрузки %d]=NULL ", i+1, i+1,i+1);
        if (i != 0) qq += ",";
        qq += ss;
      }
      q.Format("UPDATE [Технические условия] SET %s WHERE ID=%d", qq, id);
      db->Execute(q);
    }
  #endif

//  closeDialog();




    change_group_end(*kls->m_db);

    done(QDialog::Accepted);
}

void TuInputDialog::reject()
{
   if (QMessageBox::question(0, "", QString(tr("Отказаться от изменений?"))) != QMessageBox::Yes) return;

   QDialog::reject(); // не забыть вызвать базовый метод, если нужно закрыть окно
}

QString getAddr(const QString &m_gorod, const QString &m_raj, const QString &m_street, const QString &m_dom)
{
    QString m_addr = m_gorod;

    if (m_addr != "") m_addr += " ";
    if (m_raj != "") m_addr += "мкр. "+m_raj;
    if (m_addr != "") m_addr += " ";

    m_addr += m_street+" "+m_dom;

    return m_addr;
}



void TuInputDialog::onAddTUNew()
{
    Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
    if (!kls) return;

    
    QString s = "Добавить новое Техническое Условие?";

    if (QMessageBox::question(this, "", s) != QMessageBox::Yes) return;

//    updateDatabaseRow(*kls->db, const QString &tableName, int id, const std::map<QString, QVariant> &data, const QString & geom_col)
    std::map<QString, QVariant> data;
    
    read_data();

    QString addr = getAddr(m_gorod, m_raj, m_street, m_dom);

    int ist = 0;
    int rn = 0;



    data[transl("Здание")] = m_n1;
//    data[transl("Район эксплуатации")] = rn;
//    data[transl("Источник")] = ist;
    data[transl("Адрес объекта")] = addr;
//    data[transl("Камера")] = m_uzel;

    int id = insertIntoDatabase(*kls->m_db, transl("Технические условия"), data);

    if (id > 0) {
        m_org.insert(id);
        init0(*kls->m_db);
        init();
        editTU(id);
    }
}

int MMenuQ(QWidget *widget, const QString & title, QSqlDatabase &db, const QString & q, QString &out, bool *ok);


void TuInputDialog::onAddTUOld()
{
    Klassif *kls = m_kl_list.findKlN(transl(ZDAN));
    if (!kls) return;
  
    QString q =
      "SELECT ID, IIF (nomer_tu IS NULL, '', nomer_tu+' ')  + IIF (naimenovanie_obekta IS NULL, '', naimenovanie_obekta+' ') + IIF (adres_obekta IS NULL, '', adres_obekta) "
      "FROM tehnicheskie_usloviya  "
      "WHERE (zdanie=0 OR zdanie IS NULL) AND (truba=0 OR truba IS NULL)";

    q = R"(
SELECT 
    id,
    COALESCE(nomer_tu, '') || ' ' ||
    COALESCE(naimenovanie_obekta, '') || ' ' ||
    COALESCE(adres_obekta, '')
FROM tehnicheskie_usloviya
WHERE (zdanie = 0 OR zdanie IS NULL)
  AND (truba = 0 OR truba IS NULL);
  )";


    QString name;
    bool ok;

    int id = MMenuQ(this, tr("Список Технических условий"), *kls->m_db, q, name, &ok);

    if (ok) {
        m_org.insert(id);
        init0(*kls->m_db);
        init();
    }

}

