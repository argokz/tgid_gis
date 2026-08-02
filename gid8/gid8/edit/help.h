#include <cstddef>
struct EditItem;

class HelpStr
{
 public:
  HelpStr() {
    item = NULL;
  };
  QString Kod,stroka,English,Russu,Name,Help,HelpShiftF1,Razm,Velichina,Stand_Razmer;
  EditItem *item;
  QString typ;
};

void open_help();

HelpStr *getHelp(const QString & tn, const QString & name);
HelpStr *getHelp2(const QString & tn, const QString & name);
