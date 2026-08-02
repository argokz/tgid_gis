#ifndef PUMPSHELPER_H
#define PUMPSHELPER_H
#include <QtGui>
#include <QtWidgets>
#include <vector>
#include <QString>
#include <utility>

class PumpsHelper {
public:
    static void populateComboBox(QComboBox* combo, const std::vector<std::pair<int, QString>>& items, int selectedId = -1);
    static void clearAllTabs(QTabWidget* tabWidget);
    static int getIdFromIndex(QComboBox* combo, int index);
};
#endif // PUMPSHELPER_H
