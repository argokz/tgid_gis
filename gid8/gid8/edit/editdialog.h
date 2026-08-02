#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVariant>
#include <QtSql>
#include <QMap>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;

struct EditItem {
  EditItem() {
    next = false;
    query = "";
  };
  QString name;
  QString value;
  QString typ;
  QVector<QString> combo;
  QVector<QString> combo1;
  QString dbn;
  QString table;
  QString query;
  QSqlField field;
  QMetaType sql_type;
  bool next;
};

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    EditDialog(const QString & _tn, QVector<QPair<QString, QVector<QString> >  > &v, QMap<QString, QVariant> &mapv, QWidget *parent = 0);
    QMap<QString, QVariant> &get_out() { return mapv_out; }

signals:
    void findNext(const QString &str, Qt::CaseSensitivity cs);
    void findPrevious(const QString &str, Qt::CaseSensitivity cs);

private slots:
    void okClicked();
    void helpClicked();
    void textChanged(const QString & txt);
    void dateChanged( const QDate & date);
    void currentIndexChanged(int index);
    void runDialog(int n);

protected:
  bool eventFilter ( QObject * o, QEvent * e );

private:
    QTabWidget *tabWidget;
    QPushButton *okButton;
    QPushButton *closeButton;

    QWidget *curent;

//    QMap<QString, QVector<QString> > v;
    QString tn;
    QMap<QString, QVariant> mapv_out;

};

#endif
