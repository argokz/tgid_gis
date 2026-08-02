#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>

void psw(int k, long l, QString &s) {
  long n;
  int i;
  n = l*(k+1) & 0xFFFF;

  s = "";

  for (i = 0; n && i < 8; i++ ) {
    s += QChar((int)(QChar('A').unicode() + n%26));
    n = n/26;
  }
}

void psw(int k, QString &s) {
  QString ss;

  psw(k, 31991L, s);
  psw(k, 22259L, ss); s += ss;
  psw(k, 27011L, ss); s += ss;
  s = s.left(8);
}


bool check_password()
{
//    srand( (unsigned)time( NULL ) );

    bool ok;
    int n = rand();
    QString s = QString("Введите пароль для %1").arg(n);
    QString text = QInputDialog::getText(nullptr,"", QString(QObject::tr("Введите пароль для %1")).arg(n), QLineEdit::Normal, "", &ok);
    if (!ok) return false;

    psw(n, s);

    if (text.toLower() != s.toLower()) {
      QMessageBox::warning(nullptr, "", "Неверный пароль");
      return false;
    }
    return true;
}
