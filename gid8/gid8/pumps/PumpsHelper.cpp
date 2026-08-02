#include "pumps/PumpsHelper.h"

void PumpsHelper::populateComboBox(QComboBox* combo, const std::vector<std::pair<int, QString>>& items, int selectedId) {
        combo->clear(); // Очистить существующие элементы
        int index = 0;

        for (const auto& [id, name] : items) {
            combo->addItem(name, id);
            if (id == selectedId) {
                combo->setCurrentIndex(index);
            }
            ++index;
        }
    }

int PumpsHelper::getIdFromIndex(QComboBox* combo, int index) {
    // Проверяем, что индекс допустимый
    if (index >= 0 && index < combo->count()) {
        return combo->itemData(index).toInt(); // Получаем ID как int
    }
    return -1; // Возвращаем -1, если индекс недопустим
}

void PumpsHelper::clearAllTabs(QTabWidget* tabWidget) {
    while (tabWidget->count() > 0) {
        QWidget* tab = tabWidget->widget(0); // Получаем виджет первой вкладки
        tabWidget->removeTab(0);            // Удаляем вкладку
        delete tab;                         // Удаляем сам виджет, чтобы избежать утечки памяти
    }
}

