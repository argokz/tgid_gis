# tgid_gis

Перевод БД и приложения ТГИД с реляционной модели «надтип + подтипы» на ГИС-модель
**«одна таблица — один объект»**, пригодную для прямой работы в QGIS и других ГИС.

## Цели

1. Каждый тип объекта тепловой сети — отдельная таблица со своей геометрией PostGIS.
2. Реальные внешние ключи, индексы, ограничения — целостность на уровне БД, а не приложения.
3. Совместимость: существующее приложение ТГИД продолжает работать во время и после миграции.
4. Скорость: убрать мега-джойны на 26 таблиц при каждом открытии схемы.

**Полная переписка БД (продукт без compat):**
[docs/11-full-rewrite.md](docs/11-full-rewrite.md) — чистая БД `net`+`ref`
для QGIS/ArcGIS. Сборка: `tools/build_clean_gis_db.ps1`.

## Приложения

- [gid8](gid8) — **основное приложение**. Сохраняются существующие ribbon,
  карта, паспорта, расчёты, меню и диалоги; меняются только подключение,
  запросы и внутренний слой доступа к новой БД.
- [gid6](gid6) — предыдущая версия и источник поведения для регрессионной
  сверки функций.
- [tgid_app](tgid_app) — экспериментальный прототип новой архитектуры. Он
  не является продуктовым интерфейсом и не должен заменять старый `gid8`.

Шаги 23–44 в таблице ниже описывают реализованные в прототипе операции и
SQL-контракты. При дальнейшем переносе эти контракты используются внутри
старого интерфейса `gid8`, без создания нового UI.

## Состояние

| Шаг | Что | Статус |
|---|---|---|
| 1 | Обследование БД, базлайн производительности | готово — [01-analysis-current-db.md](docs/01-analysis-current-db.md) |
| 2 | Проект целевой модели, DDL, тесты целостности | готово — [02-target-model.md](docs/02-target-model.md) |
| 3 | Классификация типов объектов по измерениям | готово |
| 4 | Конвертер `public` → `net` | готово — [converter/convert.py](converter/convert.py) |
| 5 | Валидация переноса | готово — [converter/validate.py](converter/validate.py) |
| 6 | Новые запросы чтения, замеры | готово — [03-performance.md](docs/03-performance.md) |
| 7 | Готовность слоёв для QGIS, починка геометрии | готово — [170](sql/170_gis_geometry_cleanup.sql), [тест](sql/tests/test_gis_geometry_cleanup.sql) |
| 8 | Слой совместимости, переключение и откат | готово — [04-compat-layer.md](docs/04-compat-layer.md) |
| 9 | Путь записи через `INSTEAD OF`, смена класса объекта | готово |
| 10 | Новая БД `tgid_gis` на целевой схеме | готово — [tools/build_new_db.ps1](tools/build_new_db.ps1) |
| 11 | Развёртывание запросов в приложение | готово — [tools/deploy_sql.ps1](tools/deploy_sql.ps1) |
| 12 | Правки C++, сборка приложения | сборка готова — Qt 6.8.3; функциональная адаптация старого `gid8` продолжается |
| 13 | «Тонкое» представление для отрисовки карты | готово — [sql/070_map_thin.sql](sql/070_map_thin.sql), замер [map_thin_bench.json](docs/schema/map_thin_bench.json) |
| 14 | Развёртывание SQL + aqt при закрытом download.qt.io | готово |
| 15 | Сборка и проверка приложения на новой БД | пакет собирается и подключается; полный аудит всех старых экранов продолжается |
| 16–17 | Сироты оборудования / «дубли» справочников | готово — [05](docs/05-orphan-equipment.md), [06](docs/06-duplicate-tables.md) |
| 18 | Выбор строки у дублей по данным; защита от чтения из view | готово |
| 19 | Спорные объекты: обе версии + `needs_review` | готово — [spornye_obekty.sql](sql/queries/spornye_obekty.sql) |
| 20 | Подтипы тоже на `net` (views + INSTEAD OF) | контракт БД готов; старый интерфейс переводится на него по сценариям |
| 21 | Регрессия гидравлического расчёта (sety) | **structural_ok** на фр. 2 и 1; фикс `regulator_press` — [09](docs/09-calc-regression-results.md) |
| 22 | Разбор спорных потребителей глазами | очередь — [07](docs/07-duplicates-next.md) |
| 23 | Экспериментальный Qt-клиент: карта, карточка и безопасное редактирование | прототип — [tgid_app](tgid_app), продуктом остаётся `gid8` |
| 24–26 | История, архив, создание объектов и топология Qt/QGIS | готово |
| 27 | Реестр функций и SQL старого ТГИД | готово — [12](docs/12-legacy-function-inventory.md) |
| 28 | Справочники `ref` и метаданные карточек Qt | готово — [098](sql/098_core_references.sql), [105](sql/105_ui_field_catalog.sql) |
| 29 | Полный каталог полей и типизированные редакторы Qt | готово — [106](sql/106_complete_field_catalog.sql) |
| 30 | Атомарное разрезание трубопровода из Qt | готово — [описание](docs/13-line-split.md), [110](sql/110_line_split.sql), [тест](sql/tests/test_line_split.sql) |
| 31 | Безопасное соединение двух трубопроводных участков из Qt | готово — [описание](docs/14-line-join.md), [111](sql/111_line_join.sql), [тест](sql/tests/test_line_join.sql) |
| 32 | Атомарное массовое редактирование объектов из Qt | готово — [описание](docs/15-batch-edit.md) |
| 33 | Универсальный типизированный поиск по объектным таблицам | готово — [описание](docs/16-object-search.md) |
| 34 | Отчёт протяжённости трубопроводов по `net.pipe_section` | готово — [описание](docs/17-pipe-length-report.md) |
| 35 | Многополевые объектные запросы с 1–8 AND-условиями | готово — [описание](docs/18-multifield-search.md) |
| 36 | Атомарное перемещение узлов из Qt с перестроением линий | готово — [описание](docs/19-node-move.md), [112](sql/112_node_move.sql), [тест](sql/tests/test_node_move.sql) |
| 37 | Безопасная смена класса узлов и линий без потери данных | готово — [описание](docs/20-object-reclass.md), [113](sql/113_object_reclass.sql), [тест](sql/tests/test_object_reclass.sql) |
| 38 | Запрос `aZap1`: объём воды в сети и выделенных трубопроводах | готово — [описание](docs/21-network-volume.md) |
| 39 | Запросы `aZap3–aZap5`: теплопотребление последнего расчёта | готово — [описание](docs/22-heat-consumption-report.md) |
| 40 | Запрос `aZap6`: закрытые потребители и справочник внешних кодов | готово — [описание](docs/23-closed-consumers.md), [123](sql/123_externalcodes_reference.sql) |
| 41 | Запрос `onPotNagr0`: потребители с нулевой нагрузкой | готово — [описание](docs/24-zero-load-consumers.md), [тест](sql/tests/test_zero_load_consumers.sql) |
| 42 | Запрос `onPotrOtkl`: отключённые потребители по результатам расчёта | готово — [описание](docs/25-disconnected-consumers.md), [тест](sql/tests/test_disconnected_consumers.sql) |
| 43 | Запрос `onUtZakr`: закрытые участки трубопровода | готово — [описание](docs/26-closed-pipe-sections.md), [тест](sql/tests/test_closed_pipe_sections.sql) |
| 44 | Запрос `onUtZakrAll`: отключённые участки без результата `UT_OUT` | готово — [описание](docs/27-disconnected-pipe-sections.md), [тест](sql/tests/test_disconnected_pipe_sections.sql) |
| 45–47 | Полный перевод чтения/записи на `net`, снятие публичного compat | готово — [полная переписка](docs/11-full-rewrite.md) |
| 48 | Чистые типизированные GIS-слои и воспроизводимая продуктовая сборка | готово — [170](sql/170_gis_geometry_cleanup.sql) |
| 49 | `sety` на чистой БД без `net.v_*`, детерминированная регрессия расчёта | готово — [описание](docs/28-clean-calc-engine.md), [тест](sql/tests/test_calc_product_inputs.sql) |
| 50 | Дубли уже перенесённых объектов убраны из `public` на чердак | готово — [162](sql/162_attic_migrated_objects.sql) |
| 51 | Сверка сохранённых расчётов переведена на `calc`, экспериментальная регрессия закрыта свежим прогоном | готово — [полная переписка](docs/14-single-net-db.md) |
| 52 | Старые интерфейсы `gid6`/`gid8` и движок `sety` включены в основной репозиторий | готово — продуктом остаётся старый `gid8` |
| 53 | Отдельный `compat` для сеансов `gid8`, без изменения общего `search_path` QGIS | готово — [171](sql/171_compat_for_gid8.sql) |
| 54 | Плоский `fragments` поверх нормализованных таблиц, включая запись | готово — [172](sql/172_compat_fragments.sql), [тест записи](sql/tests/test_compat_fragments_write.sql) |
| 55 | Воспроизводимый пакет старого `gid8`: Qt SQL, libpq, SQL, карточки и шаблоны | готово — [build_gid8.ps1](tools/build_gid8.ps1); аудит внешних запросов продолжается |
| 56 | Паспорт `pipeSections` без второй копии сетевого объекта | готово — [173](sql/173_compat_pipesections.sql), [тест чтения/записи](sql/tests/test_compat_pipesections.sql) |
| 57 | Рабочие таблицы дефектов, опрессовок, осмотров, шурфов и ремонтов | готово — [174](sql/174_return_ops_tables.sql), [175](sql/175_return_gid8_maintenance_tables.sql), [тест INSERT](sql/tests/test_gid8_maintenance_tables.sql) |
| 58 | Временное отключение сетевого ключа активации и аппаратного HASP | готово — сборка `gid8` задаёт `USE_ACTIVATION=OFF`, `USE_HASP=OFF`; старый механизм сохранён для обратного включения |
| 63–64 | Прямой `net.v_*` для 6 классов; пробелы экспорта фрагмента | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 65 | `tbl_sql` для карточек; фикс join/reclass; `net.v_*` ИТП; multi-dir deploy | готово — [18](docs/18-gid8-cpp-rewrite.md), [178](sql/178_itp_net_views.sql); `sql/tests` **25/25** |
| 66 | Системный проход UI: запись/топология через `tbl_sql`/`net.v_*`; undo/move/add/del | готово — [18](docs/18-gid8-cpp-rewrite.md); запись на живой БД и `sql/tests` **25/25** |
| 67 | Запуск расчёта из клиента: резолверы `ww.py`/python; CLI `getDoIt` проверен прогоном фр. 2 (расчёт 158) | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 68 | Attic закрыт: `wdodevices` → `net.node_metering_device` + writable compat; электрика — только справочники `el`, мигрировать нечего | готово — [179](sql/179_wdodevices_net.sql), [тест](sql/tests/test_wdodevices_compat.sql); `sql/tests` **27/27** |
| 69 | Карточки ИТП с записью + восстановлена `net.reclass_line` (INSERT в линейные подтипы был сломан) | готово — [180](sql/180_itp_write.sql), [тест](sql/tests/test_itp_write.sql) |
| 70 | Фрагменты-копии (72/76/91): A/B на `almatygid` — не считаются из-за исходных данных (ГВ-нагрузки, коды источников), не миграция | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 71 | Фр. 1: крах `check_GG` (StopIteration) устранён — guard + `min(src)`; входы БД идентичны, 1-й проход 1:1, детерминизм calc 159≈160 | готово — [18](docs/18-gid8-cpp-rewrite.md); эталон дальше — самосогласованность `tgid_gis` |
| 72 | Блок «Карта и навигация» 16/16: `aFindKti`, `aFindTuIst`, `aColorOnlyPts`, `aHelpFinder` перенесены из gid6 (A/B-проверка), остальные — рабочий чистый UI | готово — [18](docs/18-gid8-cpp-rewrite.md); матрица `function-matrix.canvas.tsx`, все команды утверждены required |
| 73 | Блок «Гидравлические расчёты» 15/15: `aClearOut`, `aRasList`, `aInfoGid`, `aExport` реализованы; остальные A/B-проверены (24≡24 РС, экспорт 4592/3459 1:1) | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 74 | Рассинхрон деревьев gid8 устранён: шаги 65–73 перенесены в канон `tgid_gis/gid8/gid8`. Блок 4: `aTableDan`, `aTuTableNeiz`, `aTuIst`, `print_tu_itog` (A/B 1:1) | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 75 | Блок «Запросы и отчёты» 34/34: `onCreateSortNode` — порт gid6 sortNode/sortLine в `ops.sortnodesforuchastok/sortlinesforuchastok` | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 76 | Блок «Техусловия (ТУ)» 10/10: редакторы zhile/organizatsii/ТУ/присоединённая нагрузка; «неизвестные» A/B 653≡653, 4225≡4225 | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 77 | Блок «Ремонты» (IM): `setDate`/`aSetDate1–3`, `aRemontPlan`+фильтры, `aIznos`, фикс `TOP` для PG; `ops.calculation_iznos`/`iznos` из attic ([181](sql/181_return_iznos.sql)); мёртвые `--` не трогаем | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 78 | Блок «Коррозия» 17/17: `openKorrozTable` + журналы/доки, add/del/info на карте, `aSetKorrozia` (ST_DWithin), сезон/`onoff`, toolbar/ribbon; PG-native вместо MSSQL `sql/korrozia/*` | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 79 | Блок «Паспортизация»: `aAlma` (`m_bIsPts`), `aPaspNew`/`aMsPassport` → `passport_ps`, toolbar ПТС; деплой `python/docs/passport_ps` | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 80 | Блок «Теплопотери» 3/3: `onPoteri`, `onSetOpenRez`, `onTeplopoteri`→`poteriNewPg`; деплой python-пакета | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 81 | Блок «GIS-обмен»: PNG/подложка/геодезия/мульти-экспорт/адреса; Shape и findNext геобазы отложены | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 82 | Блок «Администрирование» + `aIspravl`: Update/UpdateSetup, Ispravl→`create_b5_new`, OPC-заглушка; Catalog/Swap уже были | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 83 | Блок «Прочее» (часть): `SqlSave` (PG TEMPLATE), Invert, AvtoOn/Off, AppExit, StatusBar, SetAddr, `a1001` | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 84 | Блок «Прочее» (часть 2): поворот/F5/зона/ТП/источники/ТУ/ЭС/флаги вида; тяжёлые отчёты — сообщения | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 85 | Отчёты ТУ: `onTuSvod`/`onTuZhurnal` → Excel (QXlsx), `tu/tu_reports.cpp` | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 86 | Карточки труб/узлов: `get_table_columns` видит `net.v_*` (relkind v/m + tbl_sql) | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 87 | Создание участка: PG `reset_shape`/`setNodeXY` — алиасы `net.v_*`, rebuild через триггер | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| 88 | Пилот карты: HTTPS/User-Agent тайлов; `GeoFile::find`/`findNext` + `onFindGeo`/`onGeoFindNext` | готово — [18](docs/18-gid8-cpp-rewrite.md) |
| — | 16 PR без концов | не в net (orphan) — [10](docs/10-orphan-pressregulators.md) |

## Что дальше

1. **Спорные потребители** — следующий крупный пункт (по решению в
   конце): [07](docs/07-duplicates-next.md),
   [dubli_generalizedconsumers.sql](sql/queries/dubli_generalizedconsumers.sql).
2. Фоном (по желанию): `setPsMap`, `PrPo`/reclass, журнал электросети.
3. Канон: `tgid_gis/gid8/gid8`. Матрица — `function-matrix.canvas.tsx`.

Инвентаризация старого интерфейса и SQL воспроизводится командой:

```bash
python tools/inventory_legacy.py
```

Полный машинный реестр: [legacy_inventory.json](docs/schema/legacy_inventory.json).

## Новая БД и запуск приложения на ней

```bash
$env:PGPASSWORD='...'; .\tools\build_new_db.ps1 -Source almatygid -Target tgid_gis
psql -d tgid_gis -f sql/040_switch_to_net.sql   # nodes/linesobj -> представления
psql -d tgid_gis -f sql/050_write_triggers.sql  # запись через представления
psql -d tgid_gis -f sql/055_subtype_views.sql   # views подтипов в net
psql -d tgid_gis -f sql/060_switch_subtypes.sql # public.*_subtype -> view
psql -d tgid_gis -f sql/070_map_thin.sql        # тонкая карта
.\tools\deploy_sql.ps1                          # новые us.sql / ut.sql в приложение
psql -d tgid_gis -f sql/171_compat_for_gid8.sql # старые имена только для сеанса gid8
psql -d tgid_gis -f sql/172_compat_fragments.sql
psql -d tgid_gis -f sql/173_compat_pipesections.sql
psql -d tgid_gis -f sql/174_return_ops_tables.sql
psql -d tgid_gis -f sql/175_return_gid8_maintenance_tables.sql
.\tools\build_gid8.ps1                          # старый интерфейс + runtime + Qt SQL
```

Откат: `sql/041_rollback_to_public.sql`, `sql/061_rollback_subtypes.sql`,
`.\tools\deploy_sql.ps1 -Rollback`.

## Сборка приложения

Окружение на машине разработчика: MSVC и CMake уже входят в Visual Studio 2022.
Дополнительно нужны только Qt 6 и заголовки Boost — PROJ не требуется, `proj.h`
подключается, но ни одна функция PROJ API не вызывается.

```bash
.\tools\build_gid8.ps1
```

Если `download.qt.io` недоступен, Qt ставится через зеркало с настройками
из [tools/aqt_settings.ini](tools/aqt_settings.ini).

## Достигнуто

**41 ГИС-слой, 440 480 объектов** — каждый тип в своей таблице, с геометрией PostGIS,
первичным ключом и пространственным индексом. Открываются в QGIS напрямую.

**Скорость** (открытие одного фрагмента): линии **5.1x**, узлы **2.2x**.
Схема фрагмента целиком: было ~660 мс, стало ~210 мс.
Тонкая карта (шаг 13, фрагмент 80): узлы **4.2x**, линии **2.1x** относительно
полных `us_net`/`ut_net` — см. [map_thin_bench.json](docs/schema/map_thin_bench.json).

**Целостность**: 45 внешних ключей в `net` против 0 в исходной БД.

**Геометрия восстановлена**: было видно 53 966 узлов и 45 917 линий,
стало 91 490 и 86 016.

**Совместимость подтипов**: 18 таблиц-подтипов — представления над `net` с записью
через `INSTEAD OF`; приложение создаёт типизированные объекты без потери класса.

## Порядок применения

```bash
psql -f sql/005_net_functions.sql      # функции разбора координат
psql -f sql/010_net_schema.sql         # схема net (создаётся рядом с public)
python converter/convert.py --mapping converter/mapping.json          # пробный прогон
python converter/convert.py --mapping converter/mapping.json --apply  # запись
python converter/validate.py --mapping converter/mapping.json         # проверки
psql -f sql/020_fix_geometry.sql       # необязательно: починка геометрии из источника
psql -f sql/070_map_thin.sql           # тонкие представления карты для Qt
psql -f sql/080_concurrency.sql        # row_version и защита совместного редактирования
psql -f sql/090_object_history.sql     # общий журнал Qt/QGIS и архивирование
psql -f sql/095_object_insert_history.sql # аудит новых объектов
psql -f sql/096_line_topology.sql       # единая топология линий Qt/QGIS
psql -f sql/097_node_topology.sql       # перенос концов линий при перемещении узлов
psql -f sql/098_core_references.sql      # справочники ref; запускать, пока доступен public
psql -f sql/100_gis_catalog.sql        # каталог слоёв QGIS/Qt и версия схемы
psql -f sql/105_ui_field_catalog.sql    # подписи, единицы и справочные редакторы Qt
psql -f sql/106_complete_field_catalog.sql # все поля и типизированные редакторы Qt
psql -f sql/110_line_split.sql          # атомарное разрезание трубопровода
psql -f sql/111_line_join.sql           # безопасное соединение двух участков
psql -f sql/112_node_move.sql           # перемещение узла и аудит геометрии
psql -f sql/113_object_reclass.sql      # смена класса узла или линии
psql -f sql/120_calc_schema.sql         # результаты расчётов в схему calc
psql -f sql/123_externalcodes_reference.sql # externalcodes в ref и FK узлов
psql -f sql/124_ref_single_copy.sql          # удалить проверенные дубли справочников public
psql -f sql/125_ref_move.sql                 # перенести остальные справочники public в ref
```

До шага 123 откат выполняется через `DROP SCHEMA net CASCADE`, исходные таблицы
`public` остаются без изменений. Шаг 124 намеренно удаляет только построчно
совпавшие копии справочников из `public`, без `CASCADE`; рабочая копия остаётся
в `ref`.

`098_core_references.sql` относится к сборочному контуру: он копирует необходимые
справочники из исходной `public` в `ref` до создания чистого продуктового дампа.

## Подключение

Параметры берутся из переменных окружения, см. [.env.example](.env.example).
Пароли в репозиторий не коммитятся. Локально: скопировать в `.env`.

```bash
cp .env.example .env   # заполнить
python tools/dump_schema.py --out docs/schema/almatygid.json
python tools/analyze_schema.py --schema docs/schema/almatygid.json --out docs/schema/tables.md
```

## Требования

- PostgreSQL 16 + PostGIS 3.5
- Python 3.10+, `psycopg2`
- Для расчёта: venv sety (`H:\venv\sety`) + ODBC PostgreSQL + `gid8/python/sety`
