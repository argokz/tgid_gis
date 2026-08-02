#include <QtGui>
#include <QtWidgets>

#include <db/db.h>

#include <geo/geofile.h>
#include <geo/Klassif.h>

#include <ui/Klassif_editDialog.h>
static QFont m_font;

static MyLOGFONT m_lf;


static unsigned int color1;
static unsigned int color2;
static unsigned int color3;
static unsigned int typ_point = -1;
static int m_loc;

QIcon iconPic(int typ);

bool isPoint(int loc);

bool klEditCallback(CKlassif_editDialog *prop)
{
    bool is_point = prop->m_cmbLoc == 0 || prop->m_cmbLoc == 3;
    bool is_width = prop->m_cmbLoc == 1 || prop->m_cmbLoc == 2;
    bool is_area =  prop->m_cmbLoc == 2;

    prop->ui.edit_Width->setEnabled(is_width);
    prop->ui.edit_Width->setHidden(!is_width);

    prop->ui.spin_Width->setEnabled(is_width);
    prop->ui.spin_Width->setHidden(!is_width);

    prop->ui.label_Width_static->setEnabled(is_width);
    prop->ui.label_Width_static->setHidden(!is_width);

#if 0
    prop->ui.check_Punct->setEnabled(prop->m_cmbLoc == 1);
    prop->ui.check_Punct->setHidden(prop->m_cmbLoc != 1);
#endif

    prop->ui.push_Rastr->setEnabled(is_point);
    prop->ui.push_Rastr->setHidden(!is_point);

    prop->ui.label_Color_w2->setEnabled(is_area);
    prop->ui.label_Color_w2->setHidden(!is_area);
    prop->ui.push_Color2->setEnabled(is_area);
    prop->ui.push_Color2->setHidden(!is_area);


    if (is_point) {
        QIcon icon = iconPic(typ_point);
        prop->ui.push_Rastr->setIcon(icon);
        prop->ui.push_Rastr->setIconSize(QSize(64, 64));
    }


    return false;
}


#include <dialog/PicturesDialog.h>

void CKlassif_editDialog::onRastr() {
    read();

    PicturesDialog dlg(this);

    if (dlg.exec()) {
        typ_point = dlg.m_id;
        QIcon icon = iconPic(typ_point);
        ui.push_Rastr->setIcon(icon);
        ui.push_Rastr->setIconSize(QSize(64, 64));
    }

//    QMessageBox::warning(this, "", QString("id=%1").arg(typ_point));
}

void CKlassif_editDialog::onColor() {
    read();

    unsigned int color = color1;
    if (isPoint(m_loc)) {
        color = color3;
        color = color1;
    }

    QColorDialog dlg(color, this);
    if (dlg.exec()) {
        color = dlg.currentColor().rgb();
        ui.label_Color_w->setStyleSheet(QString("background-color: #%1").arg(color, 6, 16, QChar('0')));

        if (isPoint(m_loc)) {
            color3 = color;
            color2 = color;
            color1 = color;
        }
        else {
            color1 = color;
        }

        write();
    }
}

void CKlassif_editDialog::onColor2() {
    read();

    QColorDialog dlg(color2, this);
    if (dlg.exec()) {
        color2 = dlg.currentColor().rgb();
        ui.label_Color_w2->setStyleSheet(QString("background-color: #%1").arg(color2, 6, 16, QChar('0')));
        write();
    }
}

void CKlassif_editDialog::onFont() {
    read();

//    QFont font = m_font;
//    QFontDialog dlg(m_font, this);
//    if (dlg.exec()) {

    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_font, this);
    if (ok) {
//        m_font = dlg.selectedFont();

        m_lf.lfFaceName = font.family();
        m_lf.lfHeight   = font.pointSize();
    //    m_lf.lfWeight   = fm.();
        m_lf.lfItalic   = font.italic() ? 1 : 0;
        m_lf.lfStrikeOut = font.stretch();
        m_lf.lfUnderline = font.underline();
        m_lf.lfWeight    = font.weight();

        m_font = font;
        write();
    }
}

QString findColumnRusName(const QString & d, const QString & n1, const QString & n2);


bool KlEdit(QWidget *widget, Klassif *kls)
{
    CKlassif_editDialog dlg(widget);

//    dlg.color1 = kls->coli;
//    dlg.color2 = kls->colb;
//    dlg.m_font = QFont(kls->lf.lfFaceName, kls->lf.lfHeight, kls->lf.lfWeight, kls->lf.lfItalic);

    bool yes = false;

    // Временнр убрали недоступные функции

    dlg.ui.check_Punct->setEnabled(yes);
    dlg.ui.check_Ramka->setEnabled(yes);
    dlg.ui.check_Nomas->setEnabled(yes);
    dlg.ui.check_Label_color->setEnabled(yes);
    dlg.ui.combo_List_label_color->setEnabled(yes);

    dlg.ui.check_Punct->setHidden(!yes);
    dlg.ui.check_Ramka->setHidden(!yes);
    dlg.ui.check_Nomas->setHidden(!yes);
    dlg.ui.check_Label_color->setHidden(!yes);
    dlg.ui.combo_List_label_color->setHidden(!yes);

    color1 = kls->coli;
    color2 = kls->colb;
    color3 = kls->color;
//    color3 = kls->color;
    m_font = QFont(kls->lf.lfFaceName, kls->lf.lfHeight, kls->lf.lfWeight, kls->lf.lfItalic);


    dlg.ui.combo_Loc->addItem("Точечный");
    dlg.ui.combo_Loc->addItem("Линейный");
    dlg.ui.combo_Loc->addItem("Площадной");
    dlg.ui.combo_Loc->addItem("Точечный с поворотом");

    std::list<AdoField> fields;
//    QSqlQue   ry query(*kls->m_db);
//    QSqlDatabase *db = &m_view->getCxema()->m_db;
    QSqlDatabase *db = kls->m_db;

    dlg.m_cmbList_label = -1;
    dlg.m_cmbList_label_color = -1;

    if (get_table_columns(*db, kls->nazv, fields)) {
        int i = 0;
        for (auto &field: fields) {
            QString fn = findColumnRusName(getDatabaseName(*kls->m_db), kls->nazv, field.COLUMN_NAME);

            dlg.ui.combo_List_label->addItem(fn);
            dlg.ui.combo_List_label_color->addItem(fn);
            if (field.COLUMN_NAME.toLower() == kls->codlabel.toLower()) {
                dlg.m_cmbList_label = i;
            }
            if (field.COLUMN_NAME.toLower() == kls->codlabel_color.toLower()) {
                dlg.m_cmbList_label_color = i;
            }
            i ++;
        }
    }

    dlg.m_strName = kls->nazv;
    dlg.m_strRus =  kls->RusName();

    dlg.m_iWidth = kls->typ;

    m_loc = kls->loc%10;
    int m_nLoc = m_loc <= 3 ? m_loc-1 : m_loc == 6 ? 3 : -1;

    dlg.m_cmbLoc = m_nLoc;
//    dlg.m_loc10 = kls->loc >= 10;
//    dlg.m_nListLabel = -1;
    dlg.m_bLabel = kls->label;
    dlg.m_iPrio = kls->prio;
//    dlg.m_typ = kls->typ;
//    dlg.m_potr = kls->is_potr;
    dlg.m_bRamka = kls->ramka;
    dlg.m_bPunct = false;
    if (kls->loc == LOC2) dlg.m_bPunct = kls->punct;

    if (isPoint(kls->loc)) {
        typ_point = kls->typ;
    }

//    dlg.m_check_color = FALSE;
    dlg.m_bLabel_color = kls->label_color;
//    dlg.m_nListLabelColor = -1;
    dlg.m_bNomas = kls->no_mas;

//    dlg.m_cmbLoc = kls->loc;
//    dlg.m_cmbLoc = kls->loc;

    if (!isPoint(kls->loc)) {
        dlg.ui.label_Color_w->setStyleSheet(QString("background-color: #%1").arg(kls->coli, 0, 16));

        if (kls->loc == LOC3) {  // Цвет границы
            dlg.ui.label_Color_w2->setStyleSheet(QString("background-color: #%1").arg(kls->colb, 0, 16));
        }
    }
    else {
        dlg.ui.label_Color_w->setStyleSheet(QString("background-color: #%1").arg(kls->color, 0, 16));
    }

    dlg.ui.push_Color2->setVisible(kls->loc == LOC3);


    dlg.m_labelWidth_static = QObject::tr("Толщина линии");

    dlg.setCallback(klEditCallback);
    klEditCallback(&dlg);

    dlg.write();

    dlg.ui.edit_Name->setEnabled(false);

    if (!dlg.exec()) return false;


//    int m_loc = kls->loc%10;

    m_nLoc = dlg.m_cmbLoc;

    kls->loc = m_nLoc < 3 ? m_nLoc+1 : m_loc == 3 ? LOC6 : LOC3;

    kls->coli = color1;
    kls->colb = color2;
    kls->color = color3;

    kls->label = dlg.m_bLabel;
    kls->prio  = dlg.m_iPrio ;
    kls->ramka = dlg.m_bRamka;
    kls->no_mas = dlg.m_bNomas;

    kls->label = dlg.m_bLabel;
    kls->label_color = dlg.m_bLabel_color;




//    QFontMetrics fm(m_font);

    kls->lf = m_lf;

/*
    kls->lf.lfFaceName = m_font.defaultFamily();
    kls->lf.lfHeight   = m_font.pointSize();
//    kls->lf.lfWeight   = fm.();
    kls->lf.lfItalic   = m_font.italic() ? 1 : 0;
    kls->lf.lfStrikeOut = m_font.stretch();
    kls->lf.lfUnderline = m_font.underline();
    kls->lf.lfWeight    = m_font.weight();
*/
    if (isPoint(kls->loc)) {
        kls->typ = typ_point;
//        kls->typ = dlg.m_typ;

        kls->coli = color1;
        kls->colb = color1;
        kls->color = color1;
    }
    else {
        kls->typ = dlg.m_iWidth;
    }

    kls->RusName() = dlg.m_strRus;


    int i = 0;
    kls->codlabel = "";
    kls->codlabel_color = "";
    
    for (auto &field: fields) {
        if (dlg.m_cmbList_label == i) {
            kls->codlabel = field.COLUMN_NAME;
        }
        if (dlg.m_cmbList_label_color == i) {
            kls->codlabel_color = field.COLUMN_NAME;
        }
        i ++;
    }
    return true;
}
