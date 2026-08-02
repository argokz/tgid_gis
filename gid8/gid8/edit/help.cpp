#include <QApplication>
#include  <QtWidgets>
//#include <QtSql>
#include <QMap>
#include <QtGui>

#include "editdialog.h"
#include "help.h"

#include <db/db.h>


static QMap<QString, QMap<QString, HelpStr> > map_help;

void open_help1(const QString & dbn, const QString & tn)
{
  QSqlDatabase db = QSqlDatabase::database(dbn); 
  QSqlQuery query(db);

  if (query_exec(db, query, "SELECT stroka,English,Russu,Name,Help,HelpShiftF1,Velichina,Stand_Razmer FROM [Help_" + tn + "]")) {
    while (query.next()) { 
      HelpStr hs;
      hs.stroka = query.value(0).toString(); 
      hs.English = query.value(1).toString(); 
      hs.Russu = query.value(2).toString(); 
      hs.Name = query.value(3).toString(); 
      hs.Help = query.value(4).toString();
      
      hs.HelpShiftF1 = query.value(5).toString(); 
  //    hs.Razm = query.value(6).toString(); 
      hs.Velichina = query.value(6).toString(); 
      hs.Stand_Razmer = query.value(7).toString(); 
      map_help[tn][hs.English] = hs;
//      QMessageBox::warning(0, tn, QString("%1 %2 = %3").arg(tn, hs.English, hs.Russu));
    }
  }
  else {
//    QMessageBox::warning(0, tn, query.lastError().text());
  }
}

bool createConnection(const QString & mdb, const QString & other);


void open_help()
{

{
QStringList tl;

tl << "ANY_OUT";
tl << "BP_OUT";
tl << "DR_OUT";
tl << "FT_OUT";
tl << "GO_Ожидаемый месячный график работы источника";
tl << "IO_OUT";
tl << "IT_OUT";
tl << "ITOG_OUT";
tl << "IZ_OUT";
tl << "NS_OUT";
tl << "NST_OUT";
tl << "NT_OUT";
tl << "PO_OUT";
tl << "PT_OUT";
tl << "RASPAR_OUT";
tl << "RASPAR_OUT_старое";
tl << "RS_OUT";
tl << "T1_OUT";
tl << "T2_OUT";
tl << "TG_Развернутый температурный график";
tl << "TK_График температур качественного регулирования";
tl << "TN_График нормативных температур";
tl << "US_OUT";
tl << "UT_OUT";
tl << "ZD_OUT";
tl << "Байпас";
tl << "Водоразборный прибор";
tl << "График нагрузки ГВС";
tl << "Диафрагма";
tl << "Задвижка";
tl << "Здание";
tl << "Источник тепла";
tl << "Калориферная установка";
tl << "Канал";
tl << "Компенсатор";
tl << "Коэффициенты вариации";
tl << "Магистраль";
tl << "Манометр";
tl << "Местные сопротивления участка трубопровода";
tl << "Насосная станция";
tl << "Насосный агрегат";
tl << "Обратный клапан";
tl << "Объекты с измеряемыми параметрами";
tl << "Опора";
tl << "Организация";
tl << "Потребитель обобщенный";
tl << "Потребитель реальный";
tl << "Потребитель секционированный";
tl << "Радиаторы системы отопления";
tl << "Район эксплуатации";
tl << "Распределительная сеть";
tl << "Расходомер";
tl << "Расчетная схема";
tl << "Расчетные температуры";
tl << "Регулятор давления";
tl << "Регулятор перепада давления";
tl << "Регулятор расхода";
tl << "Регулятор температуры";
tl << "Система теплоснабжения";
tl << "Температуры воздуха и грунта";
tl << "Теплообменник";
tl << "Теплообменник пластинчатый";
tl << "Теплообменник трубчатый";
tl << "Термометр";
tl << "Удельные расходы";
tl << "Узел";
tl << "Узел подпитки";
tl << "Узел присоединения";
tl << "Узел с заданным напором";
tl << "Участок теплопровода";
tl << "ЦТП";
tl << "Элеватор";

    for (int i = 0; i < tl.size(); i++) {
      open_help1("Help_Основная", tl[i]);
    }
}
{

QStringList tl;
tl << "DC_Регуляторы давления стандартные";
tl << "EL_Электродвигатель стандартный";
tl << "EV_Элеватор стандартный";
tl << "KC_Компесаторы стандартные";
tl << "KL_Калориферы стандартные";
tl << "KO_Клапан обратный стандартный";
tl << "MN_Манометр стандартный";
tl << "NC_Насос стандартный";
tl << "PL_Теплообменники пластинчатые стандартные";
tl << "PT_Регулятор температуры стандартные";
tl << "RC_Регуляторы расхода стандартные";
tl << "RD_Расходомер стандартный";
tl << "RP_Регулятор перепада стандарные";
tl << "RP_Регулятор перепада стандартные";
tl << "SM_Местные гидравлические сопротивления";
tl << "TB_Теплообменники трубчатые стандартные";
tl << "TO_Термометр стандартный";
tl << "TP_Трубы стандартные";
tl << "ZC_Задвижки стандартные";

    for (int i = 0; i < tl.size(); i++) {
      open_help1("Help_СтандОбор", tl[i]);
    }
  }



{

QStringList tl;
tl << "01_Теплопроводность грунта";
tl << "02_Нормы  плотн_тепл_потока при бескан прокладке_1990_98г";
tl << "03_Нормы плотности тепл_потока при кан_прокладке_1990_98г";
tl << "04_Нормы плотн_тепл_потока при надз_прокладке_1990_98г";
tl << "05_Нормы тепловых потерь трубами АВВ";
tl << "06_Состояние изоляции";
tl << "07_Теплоизоляционный материал";
tl << "08_Местные гидравлические сопротивления";
tl << "09_Список улиц";
tl << "10_Тепловые характеристики зданий";
tl << "11_Материал покровного слоя изоляции";
tl << "12_Материалы конструкций камер";
tl << "13_Климатические данные";
tl << "14_Шероховатость стальных труб";
tl << "15_Нормы тепловых потерь теплопроводом_до1990г";
tl << "16_Перепады температур при расчете норм тепловых потерь";
tl << "17_Предельная толщина теплоизоляции при подземке";
tl << "18_Физические свойства воды";
tl << "19_Таблица для гидравлического расчета трубопроводов";
tl << "20_Коэффициент стоимости теплоты и изоляции";
tl << "21_Коэффициент уплотнения теплоиоляционного материала";
tl << "22_Местные гидравлические сопротивления компенсаторов";
tl << "23_Поправочный коэфф расчета отопительной нагрузки здания";
tl << "24_Удельный объем воды в системе отопления";
tl << "25_Нормативные значения анализа режима работы сети";
tl << "26_Место прокладки участка трубопровода";
tl << "27_Вид покрытия участка теплопровода";
tl << "28_Коэффициенты расчета балансовой нагрузки ГВ";
tl << "29_Коэфф часовой неравномерности нагрузки ГВ  СП 41 101 95";
tl << "30_Коэффициенты местных тепловых потерь";
tl << "31_Удельный объем воды в системе вентиляции";
tl << "32_Категории_организаций";
tl << "34_Перепады темпер при расчете норм плотности__безк_1990_98";
tl << "35_Перепады темпер при расчете норм плотности__надз_1990_98";
tl << "36_Наименование района местонахождения магистрали";

    for (int i = 0; i < tl.size(); i++) {
      open_help1("Help_Справочная", tl[i]);
    }
  }


}


HelpStr *getHelp(const QString & tn, const QString & name)
{
  QMap<QString, QMap<QString, HelpStr> >::iterator it;

  it = map_help.find(tn);

  if (it != map_help.end()) {
    QMap<QString, HelpStr>::iterator it2;
    it2 = it.value().find(name);
    if (it2 != it.value().end()) {
      return &it2.value();
    }    
  }
  return NULL;
}

HelpStr *getHelp2(const QString & tn, const QString & name)
{
  QMap<QString, QMap<QString, HelpStr> >::iterator it;

  it = map_help.find(tn);

  QMessageBox::warning(0, tn, name);

  if (it != map_help.end()) {

  QMessageBox::warning(0, tn, "it != end");

    QMap<QString, HelpStr>::iterator it2;
    it2 = it.value().find(name);
    if (it2 != it.value().end()) {
      return &it2.value();
    }    
  }
  return NULL;
}

