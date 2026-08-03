#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>

#include "mainwindow.h"

#include <property/PropertyDial.h>


#if 0
void MainWindow::onFileNew() // Подключить
{
}
#endif
    

void MainWindow::onBdNew() // Создать БД
{
    void newBaza0(QWidget *widget);
    newBaza0(this);
}
    

void MainWindow::onImportShape() // Импортировать Shape-файлы
{
}
    

void MainWindow::onFilePrintSetup() // Параметры страницы...
{
}
    

void MainWindow::onConvert() // Конвертировать
{
}
    
void MainWindow::onCatalogAccepted()
{
    QSettings settings;

    auto *dlg = qobject_cast<PropertyDial *>(sender());
    if (dlg) {
        for (auto &it1 : dlg->map_edited) {
            for (auto &it2 : it1.second) {
                for (auto &it3 : it2.second) {
                    settings.setValue("paths/" + it3.first, it3.second.value);
                }
            }
        }
    }
}

void MainWindow::onCatalog() // Каталоги
{
    auto *dlg = new PropertyDial(this, "catalogs", "", "shape", "id", -1, -1);
    QSqlDatabase db;

    std::map<QString, QVariant> map_val;

    QString ff[] = {"PATH_AKT_TU","PATH_AKT_RAZD","PATH_CXEMA","PATH_DEFECT","PATH_OSMOTR","PATH_SHURF","PATH_KAPREMONT","PATH_KORROZ","PATH_OPRESSOVKA","PATH_PAS","PATH_CONTROL_TU","PATH_ELECTRO"};

    QSettings settings;

    for (int i = 0; i < sizeof(ff)/sizeof(ff[0]); i++)  {
        QString path = settings.value("paths/"+ff[i], "").toString();
        map_val[ff[i]] = path;
    }

    dlg->init_dialog(&db, "Настройка каталогов хранения документов", "catalogs", map_val);
    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onCatalogAccepted()));
}
    

void MainWindow::onAppExit() // Выход
{
    QApplication::quit();
}

    

void MainWindow::onProtocol() // Протокол
{
}
    

void MainWindow::onViewStatusBar() // Строка статуса
{
    if (statusBar()) {
        statusBar()->setVisible(!statusBar()->isVisible());
    }
}
    

void MainWindow::onUpdateSetup() // Настройка обновлений
{
    QSettings settings;
    QString code = settings.value("UpdateSetup", "").toString();
    bool ok = false;
    QString text = QInputDialog::getText(
        this,
        tr("Настройка обновления программы"),
        tr("Введите код обновления для предприятия"),
        QLineEdit::Normal,
        code,
        &ok);
    if (!ok) return;
    settings.setValue("UpdateSetup", text.trimmed());
}


void MainWindow::onUpdate() // Последнее обновление
{
    QSettings settings;
    QString code = settings.value("UpdateSetup", "").toString().trimmed();
    if (code.isEmpty()) {
        QMessageBox::information(this, "",
            tr("Код обновления не задан.\nСначала выполните «Настройка обновлений»."));
        return;
    }

    // Старый UpdateProgr тянул zip с lan.avto-glass.kz — в tgid_gis этот
    // контур не переносим. Показываем код и предлагаем открыть страницу info.
    QString url = QString("http://lan.avto-glass.kz/?action=updateInfo&db=%1").arg(code);
    if (QMessageBox::question(
            this, "",
            tr("Повторить обновление для «%1»?\n"
               "Будет открыта страница сведений об обновлении:\n%2")
                .arg(code, url))
        != QMessageBox::Yes) {
        return;
    }
    QDesktopServices::openUrl(QUrl(url));
}
    
/*
void MainWindow::onAppAbout() // О программе
{
}
*/
