#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QRadioButton>
#include <QTextBrowser>
#include <QTableView>
#include <QStandardItemModel>
#include <QDialogButtonBox>
#include <QLabel>

class BigDialog : public QDialog {
    Q_OBJECT
public:
    BigDialog(QSqlDatabase &db, int id, QWidget *parent = nullptr);

    QString mk = "", st = "", dm = "";

    double _otop = 0;
    double _gvs = 0;
    double _vent = 0;
    double _par = 0;

//    double sum = otop + gvs + vent + par;


private:
    QList<QLineEdit*> searchEdits;
    QList<QListWidget*> lists;
    QListWidget *checkList;
    QTextBrowser *textBrowser;
    QTableView *table;
    QStandardItemModel *model;

    QString add_q = "";
    QString add_q2 = "1=1";

    void init(QListWidget* lst, QSqlDatabase &db, int i);
    void init2(QSqlDatabase &db, int id);
    void init_q(bool all);

    void filterList(QListWidget *list, const QString &text) {
        for (int i = 0; i < list->count(); ++i) {
            QListWidgetItem *item = list->item(i);
            item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
        }
    }
};
