#pragma once

#include <QtGui>
#include <QDockWidget>
#include <QProcess>
#include <QTextEdit>

#include "ShellEdit.h"


class Shell : public QDockWidget
{
    Q_OBJECT

private:
  QProcess *m_process;
  ShellEdit *m_ptxtDisplay;

protected:

  void hideEvent ( QHideEvent * event );
  void mouseDoubleClickEvent(QMouseEvent *event);

public:
  Shell(QMainWindow *pwgt);
  void start(QString strCommand, QWidget *widget, const char *slot_name);
   
public slots:

  void slotDataOnStdout();
  void onFinished(int, QProcess::ExitStatus);
};
  
