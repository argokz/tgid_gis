#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <gidview/GidWidget.h>

#include <ui/Nagr_treeDialog.h>

struct Nagr
{
  Nagr() : o(0), g(0), v(0) {};
  double o, g, v;
};


CNode2 *other2(CLINE2 *l0) 
{
  CNode2 *o = other(l0);

  if (o->node.typ == TIP_PR || o->node.typ == TIP_PO/* || o->name_typ_alma*/) return o;

  CNode2 *n1 = o;
  int n_v = 0;

  CLINE2 *ln = NULL;

  for (CLINE2 *l = n1->lines; l; l = nextl(l)) {
    CLine2 *ll = bline(l);
    double naprP = ll->line.pod.q*napr(l);

    if (ll->line.nomP != -1 && naprP > 0) {
      ln = l;
      n_v ++;
    }
  }
  if (n_v != 1) return o;

  return other2(ln);
}

Nagr getNagr(CNode2 *n1, QTreeWidgetItem *ht, int level)
{
  Nagr nagr;

  ht->setExpanded(true);

  if (level > 1000) return nagr;
  
  QString str = n1->getName();
//  str.Format("%s %s", n1->node_name.kod, n1->node_name.name);

  str = "";
  if (/*n1->name_typ_alma || */ n1->node.nZN != -1) 
    str = QString("%1").arg(n1->node.name);

  int n_v = 0;

  for (CLINE2 *l = n1->lines; l; l = nextl(l)) {
    CLine2 *ll = bline(l);
    double naprP = ll->line.pod.q*napr(l);

    if (ll->line.nomP != -1 && naprP > 0) {
      n_v ++;
    }
  }


  if (n1->node.typ == TIP_PR || n1->node.typ == TIP_PO) {
    str = QString("%1 (%2 %3 %4)").arg(n1->getName()).arg(n1->node.Qot).arg(n1->node.Qgvs).arg(n1->node.Qvent);
  }


  QTreeWidgetItem *i0 = new QTreeWidgetItem(ht);
  i0->setText(0, str);

  if (n_v > 1) {
//    h = m_tree.InsertItem(str, ht);
  }
  else {
//    h = ht
  }

  if (n1->node.typ == TIP_PR || n1->node.typ == TIP_PO) {
    if (!n1->node.isPjezo) {
        nagr.o = n1->node.Qot;
        nagr.g = n1->node.Qgvs;
        nagr.v = n1->node.Qvent;
        n1->node.isPjezo = true;
    }
  }
  CLINE2 *l;

  for (l = n1->lines; l; l = nextl(l)) {
    CLine2 *ll = bline(l);
    double naprP = ll->line.pod.q*napr(l);

    if (ll->line.nomP != -1 && naprP > 0) {
      CNode2 *o = other2(l);

      Nagr nagr1 = getNagr(o,  i0, level+1);

      nagr.o += nagr1.o;
      nagr.g += nagr1.g;
      nagr.v += nagr1.v;
    }
  }

//  m_tree.Expand(ht, TVE_EXPAND);

  return nagr;
}


void GidWidget::viewNodeNagr()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        CNode2 *n = (CNode2*)action->data().toLongLong();
        if (n) {
            beginDraw(n);
//            info(m_cxema.m_db, "Узел", "nodes", n->id, -1, pr_type_any );

            CNagr_treeDialog dlg(this);

            CGraph2 *graph = m_cxema.m_graph;

            graph->save_pjezo();
            graph->reset();

            QTreeWidgetItem *i00 = new QTreeWidgetItem(dlg.ui.tree_Tree1);
            i00->setText(0, n->getName());

            Nagr nagr = getNagr(n, i00, 0);
            graph->restore_pjezo();

            dlg.m_strOtopl = QString("%1").arg(nagr.o);
            dlg.m_strGvs = QString("%1").arg(nagr.g);
            dlg.m_strVent = QString("%1").arg(nagr.v);
            dlg.m_strAll = QString("%1").arg(nagr.o+nagr.g+nagr.v);
            dlg.m_strName = n->getName();

            dlg.write();

            dlg.exec();
        }
    }
}

