#include <QtGui>
#include <QtWidgets>

#include "std.h"

#include "mainwindow.h"
//#include "GidWidget.h"

#if 1

//struct MainAction mainAction;


void MainWindow::createActions()
{

#include "mainAction3.h"





/*
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
*/

}

#endif

#if 1

void MainWindow::createMenus() 
{
    QMenu *menu1;

#include "mainAction2.h"

    mainMenu = menu1;
}

#endif

void MainWindow::createToolBars() 
{

  mainToolBar = addToolBar(tr("Главный"));
  mainToolBar->setObjectName(tr("Главный"));
//  mainToolBar->addAction(mainAction.file_newAction);
//  mainToolBar->addAction(mainAction.file_openAction);

//    mainToolBar->addAction(mainAction.file_openAction);

    mainToolBar->addAction(mainAction.aFileNew);
//    mainToolBar->addAction(mainAction.aFileOpen);
//    mainToolBar->addAction(mainAction.aFileSave);
//    separator
//    mainToolBar->addAction(mainAction.aEditCut);
//    mainToolBar->addAction(mainAction.aEditCopy);
//    mainToolBar->addAction(mainAction.aEditPaste);
//    separator
//    mainToolBar->addAction(mainAction.aFilePrint);
//    mainToolBar->addAction(mainAction.aAppAbout);
//    mainToolBar->addAction(mainAction.aQtAbout);




//  mainToolBar->setIconSize ( QSize(16, 16) );

}
