    menu1 = menuBar()->addMenu(tr("Файл"));
        menu1->addAction(mainAction.aFileNew); // Подключить
        menu1->addAction(mainAction.aFileSqlite); // Подключить
        menu1->addAction(mainAction.aBdNew); // Создать БД
        menu1->addAction(mainAction.aImportShape); // Импортировать Shape-файлы
        menu1->addSeparator();
        menu1->addAction(mainAction.aFilePrintSetup); // Параметры страницы...
        menu1->addSeparator();
        menu1->addAction(mainAction.aConvert); // Конвертировать
        menu1->addSeparator();
        menu1->addAction(mainAction.aCatalog); // Каталоги
        menu1->addSeparator();
        menu1->addAction(mainAction.aAppExit); // Выход
    menu1->setToolTipsVisible(true);
    menu1 = menuBar()->addMenu(tr("Вид"));
        menu1->addAction(mainAction.aProtocol); // Протокол
        menu1->addAction(mainAction.aViewStatusBar); // Строка статуса
    menu1->setToolTipsVisible(true);
    menu1 = menuBar()->addMenu(tr("&?"));
        menu1->addAction(mainAction.aUpdate); // Последнее обновление
        menu1->addAction(mainAction.aUpdateSetup); // Настройка обновлений
        menu1->addAction(mainAction.aAppAbout); // О программе
//        menu1->addAction(mainAction.aQtAbout); // О Qt
    menu1->setToolTipsVisible(true);
