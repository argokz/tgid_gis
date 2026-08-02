#pragma once

#include <QDialog>

class QListWidget;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;

class ParamDr : public QDialog
{
    Q_OBJECT

public:
    ParamDr(QWidget *parent);
    QString path() { return m_str; }

private slots:
    void okClicked();
    void helpClicked();
    void urClicked(int state);

protected:
  bool eventFilter ( QObject * o, QEvent * e );

private:
  QString m_str;

  QCheckBox *ur;
  QCheckBox *teplopoter;
  QCheckBox *m_uf_calc;
  QCheckBox *teplovyd;
  QCheckBox *veter;
  QCheckBox *dross;
  QCheckBox *uf_new;
  QCheckBox *avtomat;

  QCheckBox *zn0;
  QCheckBox *utechki;


  QCheckBox *char_sety;
  QCheckBox *save_po;
  QCheckBox *leto;    
  QDoubleSpinBox *e1;
  QComboBox *e2;

};
