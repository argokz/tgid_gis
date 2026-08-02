mainAction.aFileNew = new QAction(tr("Подключить"), main_window);
mainAction.aFileNew->setIcon(QIcon(":images/new/big/connect.svg"));
mainAction.aFileNew->setToolTip("Подключить базу данных SQL\nПодключить");
connect(mainAction.aFileNew, SIGNAL(triggered()), this, SLOT(onFileNew()));

mainAction.aFileSqlite = new QAction(tr("Открыть файл"), main_window);
//mainAction.aFileSqlite->setIcon(QIcon(":/images3/FileSqlite.png"));
mainAction.aFileSqlite->setToolTip("Открыть файл\nОткрыть файл");
connect(mainAction.aFileSqlite, SIGNAL(triggered()), this, SLOT(onFileSqlite()));



mainAction.aBdNew = new QAction(tr("Создать БД"), main_window);
mainAction.aBdNew->setIcon(QIcon(":images/new/big/add-db.svg"));
mainAction.aBdNew->setToolTip("Создать новую базу данных SQL\nСоздать БД");
connect(mainAction.aBdNew, SIGNAL(triggered()), this, SLOT(onBdNew()));

mainAction.aImportShape = new QAction(tr("Импортировать Shape-файлы"), main_window);
mainAction.aImportShape->setIcon(QIcon(":/images2/ImportShape.png"));
mainAction.aImportShape->setToolTip("Импортировать Shape-файлы в новую или существующую базу данных SQL карты местности\nИмпортировать Shape-файлы");
connect(mainAction.aImportShape, SIGNAL(triggered()), this, SLOT(onImportShape()));

mainAction.aFilePrintSetup = new QAction(tr("Параметры страницы..."), main_window);
mainAction.aFilePrintSetup->setIcon(QIcon(":/images2/FilePrintSetup.png"));
mainAction.aFilePrintSetup->setToolTip("Выбор принтера и свойств печати\nПараметры страницы");
connect(mainAction.aFilePrintSetup, SIGNAL(triggered()), this, SLOT(onFilePrintSetup()));

mainAction.aConvert = new QAction(tr("Конвертировать"), main_window);
mainAction.aConvert->setIcon(QIcon(":/images2/Convert.png"));
mainAction.aConvert->setToolTip("Конвертирование файлов из БД ТГИД-07 в БД ТГИД-07-SQL\nКонвертор из предыдущих версий");
connect(mainAction.aConvert, SIGNAL(triggered()), this, SLOT(onConvert()));

mainAction.aCatalog = new QAction(tr("Каталоги"), main_window);
mainAction.aCatalog->setIcon(QIcon(":/images2/Catalog.png"));
mainAction.aCatalog->setToolTip("Настройка каталогов хранения документов\nКаталоги");
connect(mainAction.aCatalog, SIGNAL(triggered()), this, SLOT(onCatalog()));

mainAction.aAppExit = new QAction(tr("Выход"), main_window);
mainAction.aAppExit->setIcon(QIcon(":images/new/big/Exit.svg"));
mainAction.aAppExit->setToolTip("Выход из программы\nЗавершить работу");
connect(mainAction.aAppExit, SIGNAL(triggered()), this, SLOT(onAppExit()));

mainAction.aProtocol = new QAction(tr("Протокол"), main_window);
mainAction.aProtocol->setIcon(QIcon(":/images2/Protocol.png"));
mainAction.aProtocol->setToolTip("Показать/скрыть окно протокола теплогидравлического расчета режима тепловой сети");
connect(mainAction.aProtocol, SIGNAL(triggered()), this, SLOT(onProtocol()));

mainAction.aViewStatusBar = new QAction(tr("Строка статуса"), main_window);
mainAction.aViewStatusBar->setIcon(QIcon(":/images2/ViewStatusBar.png"));
mainAction.aViewStatusBar->setToolTip("Показать или спрятать строку состояния\nСтрока состояния");
connect(mainAction.aViewStatusBar, SIGNAL(triggered()), this, SLOT(onViewStatusBar()));

mainAction.aUpdate = new QAction(tr("Последнее обновление"), main_window);
mainAction.aUpdate->setIcon(QIcon(":/images2/Update.png"));
mainAction.aUpdate->setToolTip("Повторить последнее обновление программы\nПоследнее обновление");
connect(mainAction.aUpdate, SIGNAL(triggered()), this, SLOT(onUpdate()));

mainAction.aUpdateSetup = new QAction(tr("Настройка обновлений"), main_window);
mainAction.aUpdateSetup->setIcon(QIcon(":/images2/UpdateSetup.png"));
mainAction.aUpdateSetup->setToolTip("Настройка обновления программы по местам применения (на предприятиям)");
connect(mainAction.aUpdateSetup, SIGNAL(triggered()), this, SLOT(onUpdateSetup()));

mainAction.aAppAbout = new QAction(tr("О программе"), main_window);
mainAction.aAppAbout->setIcon(QIcon(":/images2/AppAbout.png"));
mainAction.aAppAbout->setToolTip("Показать информацию о программе, ее версии и авторах\nО программе");
connect(mainAction.aAppAbout, SIGNAL(triggered()), this, SLOT(onAppAbout()));

mainAction.aQtAbout = new QAction(tr("О Qt"), main_window);
mainAction.aQtAbout->setIcon(QIcon(":/images2/AppAbout.png"));
mainAction.aQtAbout->setToolTip("Показать информацию о программе, ее версии и авторах\nО программе");
connect(mainAction.aQtAbout, &QAction::triggered, this, QApplication::aboutQt);

