# Переписывание gid8: C++ на прямой доступ к net

Решение заказчика: переписывать исходники, а не поддерживать слой
совместимости бессрочно. Старый код gid8 служит спецификацией — какие
таблицы и в каком составе колонок нужны реальной работе приложения,
решает не абстрактное проектирование, а то, что уже написано и
эксплуатируется.

## Где редактировать

Правки идут в `tgid_gis\gid8\gid8` — это дерево, которое собирает
`tools/build_gid8.ps1` по умолчанию. Внешняя копия
`H:\projects\tgid-app-new\gid8` не собирается; правки туда молча
теряются (см. [17](17-inventory-and-source-tree.md)).

## Шаг 1 выполнен: шесть основных классов

`tools/db_consumers.py`, сфокусированный на реально компилируемом дереве
(`gid8\gid8\*.cpp,*.h` — 486 файлов), дал точную цифру вместо прежней
оценки по всему проекту с учётом внешних `.sql`-шаблонов и python:
**30 файлов, 200 обращений** к `nodes`, `linesobj`, `heatsources`,
`realconsumers`, `generalizedconsumers`, `connectnodes`.

Перед заменой проверено, а не предположено: `compat.<имя>` для каждого
из шести — буквально `SELECT [те же колонки] FROM v_<имя>`. Значит
замена имени таблицы в C++ на `net.v_<имя>` даёт **то же самое**, что
сейчас идёт через `compat`, только без лишнего слоя — поведение не
меняется, меняется только маршрут.

Механическая подстановка (`FROM|JOIN <имя>` → `FROM|JOIN net.v_<имя>`,
без разбора регистра, с защитой от повторной подстановки уже
переведённых мест) применена к 30 файлам. Первая попытка испортила diff
до нечитаемости — файл открывался и писался без сохранения переводов
строк (CRLF→LF), и 6 логических замен превращались в diff на весь файл.
Исправлено чтением и записью с `newline=''` — та же грабля, что уже
попадалась на `converter/convert.py`.

### Проверено

* diff чист — 30 файлов, 181 строка, каждая замена точечная;
* строковые сравнения (`if (table == "nodes")`) и параметры-метки
  (`export_table(out, db, "nodes", ...)`) не тронуты — подстановка
  работает только в контексте `FROM`/`JOIN`;
* извлечённые из diff запросы построены заново и проверены `EXPLAIN`
  **без `compat` в пути поиска** — план строится, код архитектурно не
  зависит от слоя совместимости для этих шести классов;
* внутреннее дерево собрано (`H:\build\gid8-net-direct\gid8.exe`,
  273 объекта, `windeployqt` прошёл) — компиляция подтверждает, что
  подстановка синтаксически корректна;
* `sql/tests` — 23 из 25 (два известных провала, не новые).

### Что не входит в этот шаг

Найден отдельный, более крупный механизм: **общий движок карточек
свойств** (`cxema_edit.cpp`, `export_fragment.cpp`, `conv_tgid_qgis.cpp`)
читает и пишет объект по имени таблицы, переданному переменной `tn`,
через `br_text(tn)` — 136 мест в проекте с динамической подстановкой
имени. Это, судя по всему, единый путь для карточек всех ~37 типов
объектов, а не только шести переведённых. Пока продолжает идти через
`compat` — это следующая, отдельная задача, крупнее и рискованнее:
нужно перенести отображение `NODE_CLASS`/`LINE_CLASS`
(`gid8/python/sety/sety/net_mode.py`) на сторону C++ и научить
резолвер `tn` возвращать `net.v_<класс>` вместо квотирования старого
имени.

## Правило, подтверждённое ещё раз в этом шаге

Диагностика зависшего `psql` во время проверки чуть не увела в сторону
несуществующей проблемы блокировки БД — на деле в командах пропал
`PGPASSWORD`, и клиент висел на невидимом запросе пароля. `pg_isready`
(лёгкий, не требует авторизации) сразу показал бы, что сервер жив;
несколько минут ушло на догадки вместо этой одной команды.

## Шаг 2: пробелы экспорта фрагмента и генератор ИТП-классов

`export_fragment.cpp` строит запросы динамически (`FROM %1`,
`.arg(br_text(tn))`), поэтому в замер задачи выше не попал — статический
разбор не видит имя, подставляемое в рантайме. Извлечён полный список:
**34 имени**, используемых для экспорта всех типов объектов фрагмента.
Проверка каждого при `search_path` gid8 дала **10 нерезолвящихся**.

Первая попытка проверки дала ложный результат — 27 из 34 не резолвились.
Причина в самой проверке: `except: c.rollback()` откатывал и `SET
search_path`, установленный в той же транзакции, поэтому после первой
же неудачи все следующие проверки в цикле шли уже без `compat`.
Переделано с новым соединением на каждую проверку — реальных пробелов
оказалось 10, не 27.

**Группа 1** — четыре класса оборудования ИТП
(`consumptRegulators`, `pressDropRegulators`, `regulArmatures`,
`reverseValves`), заведённые в `net` пустыми по решению заказчика (шаг
41). Представлений для чтения не было — при пустых таблицах пожаловаться
было некому. Отображение `id`/`lineid` проверено на населённом классе
того же происхождения (`net.regulator_press`, 509 строк): `id` объекта —
исходный id самой линии, `lineid` в старой модели — то же самое; в `net`
это `subtype_src_id`/`id` соответственно.

При подготовке представлений найден и исправлен реальный дефект
генератора: `tools/gen_line_class.py` считал `nodeid` всегда избыточным
(«выражен через `node_from`/`node_to`»), но у регуляторов это
самостоятельная колонка, не совпадающая с концами линии. Она была в
источнике и терялась при создании класса — обнаружилось только сейчас,
при попытке построить представление. Отдельно нашёлся второй дефект:
генератор был жёстко привязан к схеме `public` для поиска исходной
таблицы, а три из пяти источников уже лежали на чердаке (`attic`) —
из-за этого свои колонки просто не находились, без единой ошибки.
Оба исправлены; `sql/130_itp_classes.sql` пересоздан и применён
(идемпотентно, `ADD COLUMN IF NOT EXISTS`).

**Группа 2** — пять узловых таблиц на чердаке (`overgroundNodes`,
`pavilions`, `undergroundNodes`, `uninstalledNodes`, `internalNodes` —
все пустые, `WDOdevices` — 3385 строк). Представления — прямой проброс
к `attic.<имя>`, данные не менялись.

[`sql/177_compat_export_gaps.sql`](../sql/177_compat_export_gaps.sql)
закрывает все десять. Проверка: все 34 имени резолвятся,
`sql/tests` — 23 из 25 (те же два известных провала).

## Находка, не связанная с сегодняшней правкой: `ist` и id источника тепла

При сверке результатов расчёта обнаружилось резкое расхождение —
тысячи вместо десятков, худшая колонка `ist` (ссылка на источник тепла)
со значением 176/177 вместо 0. Прежде чем считать это регрессией,
проверено по цепочке, а не предположено:

* на эталонной, нетронутой `almatygid` `ist = 0` **во всех 136 876
  строках всех 32 исторических расчётов фрагмента** без исключения —
  это не разовая случайность, а постоянное свойство старой системы;
* `gidview/analiz.cpp` действительно использует `us_out.ist`, соединяя
  его с `heatSources` для показа источника потребителя —
  `JOIN heatSources hs ON hs.id=o.ist`;
* **этот код не менялся сегодня**: сравнение с версией до коммита
  показало дословно тот же `JOIN ... ON hs.id=o.ist`, изменилось только
  имя таблицы (`heatSources` → `net.v_heatsources`);
* `compat.heatsources` — это `SELECT * FROM v_heatsources` без всяких
  условий (проверено ещё в задаче №1 этого документа), то есть та же
  семантика была доступна через `compat` и раньше, до сегодняшних
  правок C++.

Причина не в коде запроса, а в самой модели данных: `net.heat_source.id`
— канонический id узла (правило, единое для всех объектов-узлов), а
исходный id строки в таблице-подтипе `heatsources` (176 = АТЭЦ1,
177 = АТЭЦ2) при переносе ушёл в `subtype_src_id`. `externalcodes.heatsourceid`
(источник значения `ist`) продолжает хранить старую нумерацию (176/177)
и никогда не переотображался на новую. Раньше это было незаметно,
потому что `ist` в самом расчёте всегда оставался нулём (отдельная,
самостоятельная особенность старой системы) — обе стороны совпадения
`hs.id=o.ist` были одинаково нерабочими, только по разным причинам.
Сейчас `sety` стал вычислять `ist` корректно (реальные значения вместо
нуля), и несовпадение системы нумерации стало видно впервые.

### Перепроверка (шаг 65)

На живой `tgid_gis` join `net.v_heatsources hs ON hs.id = o.ist`
**корректен**: у представления колонка `id` уже есть `subtype_src_id`
(`sql/055_subtype_views.sql`). Сверка:

* `calc.us_out` с `ist <> 0` — 24 392 строки;
* join через `net.v_heatsources` — 24 392;
* join через физический `net.heat_source.id` — 0;
* join через `net.heat_source.subtype_src_id` — 24 392;
* `ref.externalcodes.heatsourceid` → `v_heatsources.id` — 469 / 469.

Менять C++ на `hs.subtype_src_id` **нельзя**, пока запрос идёт к
`net.v_heatsources`: там нет такой колонки в роли ключа строки, ключ
уже назван `id`. Расхождение с эталоном `almatygid` (там `ist` всегда 0)
— улучшение расчёта, не регрессия join.

## Шаг 3: резолвер `tbl_sql` для динамических имён таблиц

Добавлен `tbl_sql(tn)` в [`db.cpp`](../gid8/gid8/db/db.cpp) /
[`db.h`](../gid8/gid8/db/db.h): для 20 типов с представлениями
`net.v_*` возвращает `net.v_<имя>`, иначе — прежний `br_text(tn)`
(через `compat` / `search_path`).

Подключено в движке карточек и соседних путях, где имя приходит
переменной: `cxema_edit`, `read_tab_db`, `PropertyDial`,
`export_fragment`, `geo_select`, `geo_open`, `edit`, `TableWindow`,
`DbWindow`, `MultiHeaderTable`. Колонки и русские подписи по-прежнему
идут через `br_text`.

Сборка: `H:\build\gid8-tgid-gis-step65\gid8.exe` (273 объекта).

ИТП-классы и `bypass` переведены на `net.v_*` в
[`sql/178_itp_net_views.sql`](../sql/178_itp_net_views.sql); `compat.*` —
тонкие обёртки. Attic-пробросы (`WDOdevices` и пустые узлы) пока через
`compat` / `br_text`.

## Сопутствующие фиксы точности (шаг 65)

Два известных провала `sql/tests` (23/25) оказались регрессией после
паспорта `pipe_section` (шаг 56 / `173`):

* `pipesectionid` BEFORE INSERT := `id` → `join_lines` считал участки
  разными по «бизнес-атрибутам»;
* `id_old` DEFAULT 0 и `pipesectionid` блокировали `reclass_object`
  `pipe_section → line_plain` как «потерю заполненных полей».

Исправлено в [`111_line_join.sql`](../sql/111_line_join.sql) и
[`113_object_reclass.sql`](../sql/113_object_reclass.sql): суррогаты
исключены из сравнения/копирования и из проверки потери полей.
После применения — **25/25**.

`deploy_sql.ps1` раскатывает `us.sql`/`ut.sql` во все каталоги
`gid8/runtime`, `gid6/gidr` и сборки `H:\build\gid8*`, а не в один.

## Шаг 66: системный проход по интерфейсу — запись

По матрице UI→SQL ([gidrMenu](../gid8/gid8/gidview/gidrMenu.cpp) →
[gidrAction](../gid8/gid8/gidview/gidrAction.cpp) →
[gidrSlot](../gid8/gid8/gidview/gidrSlot.cpp)) проверено и доведено
по цепочке: открыть фрагмент → карточка → изменить → удалить/перенести →
откатить → отчёт. Чтение (`us_net`/`ut_net`, `fragments`, `calc.us_out`)
и основные запросы `onZap1`/`onZap7`/`onZap3` уже были на `net`; оставался
путь **записи**.

* `db.cpp`: `insertIntoDatabase`, `updateDatabaseRow`, `updateRow`
  теперь всегда резолвят имя через `tbl_sql`, а не только при чтении.
* `undo_gid.cpp`: undo move/delete узла/линии переведены с `nodes`/
  `linesobj` на `net.v_nodes`/`net.v_linesobj`.
* `gidr_info.cpp`: удаление подтипа при отмене карточки (`pr_type_node`/
  `pr_type_line`) — через `tbl_sql(dlg->m_table)`.
* `gidr_rbut.cpp`: скрыть/массовые UPDATE (`nodes`, `linesobj`,
  `realConsumers`, `refillNodes`, `setPressNodes`, динамические
  `getTableMySQL()` через `tbl_sql`).
* `gidr_del.cpp`, `gidr_insert.cpp`, `gidr_find.cpp`,
  `GidWidget.cpp`: `UPDATE` координат/переключателей идут напрямую в
  `net.v_linesobj`.

Проверка записи на живой `tgid_gis` (в транзакции, откат): `INSERT` узла,
`UPDATE` узла, `DELETE` через `net.v_nodes` (removed=1), `INSERT` подтипа
в `net.v_connectnodes`, `UPDATE` через `net.v_connectnodes` — всё
успешно. `sql/tests` — 25/25. Сборка `H:\build\gid8-tgid-gis-step65`
— без ошибок.

Остаток, не входивший в карту записи: метаданные
(`fragments`/`externalCodes`/`texts`/`heatSystem`) и запуск `sety4` из
`gidr_calc.cpp` — следующий блок; электрика и `sprav.dbo` мёртвые.

## Шаг 67: запуск расчёта из клиента

Проверка цепочки `onDoIt*` → `get_sety4()` → bat → `ww.py` показала,
что из свежей сборки расчёт не стартовал бы: `init_sety4()` искал
`ww.py` только в развёрнутом пакете (`argpath()`/`dir+"/../python"`),
а `python_exe()` — только embed-интерпретатор; ни того ни другого рядом
с `H:\build\gid8-tgid-gis-step65\gid8.exe` нет.

Исправлено:

* [`gidr_calc.cpp`](../gid8/gid8/gidview/gidr_calc.cpp) `init_sety4()` —
  сначала явный `config/sety4` из QSettings, затем кандидаты пакета
  с проверкой существования, последним — рабочая копия
  `gid8/python/sety/sety/ww.py`.
* [`export.cpp`](../gid8/gid8/gidview/export.cpp) `python_exe()` —
  после embed-пакета ищет venv движка (`dir+"/../venv/sety/..."`,
  затем `H:/venv/sety/Scripts/python.exe`), в конце — `python` из PATH.

Проверка: CLI, который строит `getDoIt()` (`-type_of_net 1 -Tn -25
-GWS 1 -GWS2 1 -sopr 0 -roP 0.975 -roO 0.975 -ro_temp -rdbms postgreSQL
-color -user_gid`), прогнан на фрагменте 2 — создан расчёт 158
(`calc.us_out` 4 908 строк, `calc.ut_out` 2 773). Параметр
`-type_of_net` движком не используется (suppress в `config.py`),
оставлен для совместимости с `tools/recalc_all.ps1`.

Запросы отчётов (`zaprosy.cpp` `onZap1/2/3/7`, `onPoteri`) и чтение
результатов (`calc.us_out/ut_out` по `fileid`) проходят `EXPLAIN` на
`tgid_gis`; `heatPipeSections`/`CALCULATION`/`IIF` резолвятся через
`compat` — функционально закрыто, переписывание литералов на
`net.v_*`/`CASE` остаётся косметикой.

## Шаг 68: attic-пробросы закрыты — wdodevices на целевой таблице

Целевая модель (docs/02) предписывала `wdodevices` дочерней таблицей
`net.node_metering_device`; таблица была создана ещё в `sql/010` с
10 живыми строками (src_id 481–490, узел 14522), а `compat.wdodevices`
остался пробросом на чердак со всеми 3 385 строками (3 375 — мёртвые,
разбор в docs/14, шаг 12) и без записи. `b5.cpp` (построение внутренней
схемы) делает `insertIntoDatabase("WDOdevices", …)` — через проброс это
падало бы.

`sql/179_wdodevices_net.sql`: `compat.wdodevices` теперь представление
над `net.node_metering_device` в старой форме (id/nodeid/externalsignid/
wdo/hydrores), `id = COALESCE(src_id, id)` — старые строки отдают прежние
481–490 (экспорт не заметит), новые — реальный net id (RETURNING у
`insertIntoDatabase` не NULL). INSTEAD OF INSERT/UPDATE/DELETE пишет в
целевую таблицу. Тест `test_wdodevices_compat.sql` — OK.

Пять пустых узловых таблиц (`overgroundnodes`, `pavilions`,
`undergroundnodes`, `uninstallednodes`, `internalnodes`) — 0 строк, их
читает только экспорт фрагмента; проброс через `compat` остаётся
осознанным резолвером, переносить нечего.

Электрика: в `tgid_gis` схема `el` содержит только 5 справочников
(energosistemy 1, marki_kabeley_es 16, marki_transformatorov_es 2,
tipy_istochnikov 4, tipy_lep 2, tipy_priemnikov 6) — объектных таблиц
нет и никогда не было; слоты `onElectro*` в `gidrSlot.cpp` пустые.
Мигрировать нечего, UI мёртвый.

## Шаг 69: запись через карточки ИТП + восстановлена net.reclass_line

Проверка вставки выявила латентный дефект: триггеры линейных подтипов из
`sql/055_subtype_views.sql` вызывали `net.reclass_line(bigint, text)`,
которой в живой БД не было (существовали только `reclass_node` и
`reclass_object`). INSERT в `net.v_pressregulators` и любой другой
линейный подтип падал с «function net.reclass_line does not exist» —
карточки регуляторов/насосов/шиберов на запись были сломаны.

`sql/180_itp_write.sql`:
* `net.reclass_line` — симметрично `reclass_node`, по `net.line_reg`:
  перенос строки между классовыми таблицами с копированием общих колонок.
* INSTEAD OF INSERT/UPDATE/DELETE на пять представлений шага 65
  (`v_bypass`, `v_consumptregulators`, `v_pressdropregulators`,
  `v_regularmatures`, `v_reversevalves`) по образцу `v_pressregulators`:
  INSERT — reclass в класс + UPDATE атрибутов, DELETE — снятие типа в
  `line_plain` (объект не удаляется).

Тест `test_itp_write.sql`: reclass туда-обратно, INSERT/UPDATE/DELETE
через `v_bypass`, ранее мёртвый путь `v_pressregulators`,
`v_consumptregulators`, `v_reversevalves` — OK. `sql/tests` — 27/27.

## Шаг 70: фрагменты-копии — диагноз снят, это исходные данные

В очереди висело «фрагменты без строки в `fragments` (80, 91, …)».
Строки уже были восстановлены ранее («Фрагмент 80 (восстановлен
автоматически)»), поэтому проверили, считается ли теперь sety.

Прогоны CLI `getDoIt` (`tools/_calc_copy_probe.py`):
* фр. 76 (копия): «Не задана нагрузка ГВ [РС1-4 пэу5/12-2]»;
* фр. 72 (оригинал семьи, ЗЭР): «Не задана нагрузка ГВ [РС1-7 пэу17*]»
  (`read_vnutr.py:410`, элемент t=46 рециркуляции с нулевыми расходами);
* фр. 91 (оригинал своей семьи): «Не указан Источник тепла в Расчетных
  схемах» (`check_ec_hs.py:60`, список 6-8, 9-9, К1-1…К1-6) + та же ГВ.

A/B-контроль: тот же прогон на **исходной `almatygid`** для фр. 72 дал
идентичную ошибку (`tools/_sety_almatygid_72_stdout.txt`). Значит схемы
никогда не считались — это незаполненность исходных данных (нагрузки ГВ
во внутренних схемах, коды источников), а не дефект миграции. Правится
оператором в gid8, после чего расчёт пойдёт по уже проверенному контуру.

Топология фрагментов-копий полная (76: 2 972 узла / 6 846 линий во всех
классах; 91: 1 419 / 3 771).

## Шаг 71: фр. 1 — крах check_GG устранён, расхождение со старой БД объяснено

Регрессия дня: `tgid_gis` фр. 1 стала падать `StopIteration` в
`check_ist3.check_GG` (`src_max = next(iter(src))` на пустом множестве),
хотя 29.07 та же схема считалась, а `almatygid` считается по-прежнему.

**Расследование по слоям** (пробы `tools/_inputs_diff_probe.py`,
`_dict_diff_probe.py`, `_log_diff_probe.py`):

1. **Все прямые входы идентичны**: узлы/линии карты (2028/1997),
   все классы подтипов (насосы, регуляторы, потребители, участки),
   словари `ref.*` (varCoefficients, specExpends, calcTemperatures,
   externalCodes, deployedtempgraphs, standardpumps/tubes…),
   параметры источников и узлы с заданным напором — 0 расхождений.
2. **Первый гидравлический проход сходится 1:1** до 16-й итерации
   (расхождение d < 1e-5 относительное) — ядро и данные эквивалентны.
3. Расхождение входит **между проходами**: в `check_GG` выбор источника
   для перевода поддерева зависел от `next(iter(src))` (порядок множества
   = порядок хешей = порядок физических строк БД) и от argmax с нестрогим
   порядком обхода. На `tgid_gis` максимальным по расходу оказалось ребро
   с пустым `src` (у узлов '2-46' и '2КИР-4' — расходы 58.8 и 75.5 т/ч
   при нулевой атрибуции) → раньше это был крах, на старой БД порядок
   строк «везло» и ребро с пустым src в максимум не попадало.
4. Отдельный механизм дрейфа: движок пишет расчётные поля обратно в
   карточки потребителей (`net.consumer_real/general`), поэтому следующий
   расчёт читает результат предыдущего — результаты зависят от истории
   расчётов БД (у tgid_gis свои расчёты 155–160, у almatygid только
   эталонные). Узел 1747 ('Айтеке Б,80/1') — показательный: tg=0 против
   131.6 между соседними прогонами.

**Исправлено в `check_ist3.py`**:
* пустой `src` на максимальном ребре больше не роняет расчёт —
  `set_descendant` пропускается с диагностикой в лог (имя узла, ребро,
  расход);
* `next(iter(src))` заменён на `min(src)` — детерминированный выбор
  источника, не зависящий от порядка строк и хешей. Остальные
  `next(iter(...))` в движке проверены и безопасны (все под защитой
  проверок непустоты).

**Проверка**:
* `run_calc.ps1 -Compare -Fragment 1` — оба расчёта exit 0, строки во всех
  `*_OUT` совпадают (4592/3459/956/34/61);
* детерминизм `tgid_gis`: calc 159 против 160 — `us/ut/ns/rs_out`
  идентичны полностью, в `pt_out` 1 строка (узел 1747, write-back дрейф).

**Вывод**: бит-в-бит паритет с `almatygid` для фр. 1 недостижим и не нужен:
расчёт зависит от истории расчётов самой БД (write-back в потребителей,
узлы с заданным напором от последнего расчёта магистрали), а атрибуция
источников исторически порядок-зависима. Эталон дальше —
самосогласованность `tgid_gis` (детерминизм подтверждён), а не побайтовое
равенство со старой БД.

## Шаг 72: блок «Карта и навигация» — 16/16 команд закрыты

Решение заказчика от 2026-08-02: все 335 команд интерфейса — `required`,
перенос по блокам сверху вниз (`docs/12`, матрица `function-matrix.canvas.tsx`).
Первый по порядку незакрытый блок — «Карта и навигация».

Инвентаризация блока показала: 11 команд — чистый UI без обращений к БД
(`aFon`, `aFont`, `aFont2`, `aFontMag`, `aFontPanel`, `aMasPodpis`,
`aMasall`, `aMasshtab`, `aNavigator`, `aFindcoordDeg`, `aZoom` — режим
рамки-масштаба через `regimGroup`, тело слота не требуется), `aMapSearch`
закрыт шагом 16. Четыре команды были пустыми заглушками в
`gidrSlot.cpp` и перенесены из gid6:

* `onFindKti` (aFindKti, «По коэффициенту тепловых испытаний») — выбор
  коэффициента из `heatpipesections`, выделение линий через
  `vyd_lines`. A/B: `almatygid` 5982 линии (k=1.0, фр. 1–3), `tgid_gis`
  5979 — разница ровно 3 линии (10196–10198), все три учтены в
  `net.line_orphan` (известный пробел шага 27, геометрия не
  восстановлена). Ошибки миграции нет.
* `onFindTuIst` (aFindTuIst, «ТУ по источнику») — выбор источника из
  `tehnicheskie_usloviya` (схема `doc`), маркировка зданий слоя
  `zdaniya_tu` (схема `addr`) и переход к их охвату. A/B: 72 источника
  1:1, выборка зданий совпадает. Таблицы резолвятся через `search_path`
  gid8 (`addr`, `doc` уже входят, см. `db.cpp`).
* `onColorOnlyPts` (aColorOnlyPts) — переключение флага
  `m_isOnlyPTSColor` (используется отрисовкой в `gidr_draw.cpp`) +
  `repaint()`.
* `onHelpFinder` (aHelpFinder, F1) — открытие `help/index.html` /
  `gid8.chm` / `gid8.pdf` рядом с исполняемым файлом; при отсутствии —
  сообщение. Контента справки в обоих проектах нет и не было.

Замечание по данным: в `heatpipesections` фактически одно значение
коэффициента (1.0) в обеих БД — команда работоспособна, но выбор
тривиален до появления реальных КТИ в данных.

## Шаг 73: блок «Гидравлические расчёты» — 15/15 команд закрыты

Инвентаризация блока: `aDoIt` закрыт шагами 67–71; `aDoItDr`,
`aDoItList`, `aDoItListDr` используют ту же проверенную машинерию
запуска (`getDoItList*` + `run_bat`/`main_window->start`);
`aRasprMag`, `aSetTr`, `aSetOpenGvsT`, `aSetKodRs`, `aSavePjezo`,
`aPjezo`, `aListPjezo` уже реализованы в gid8 — для них выполнена
A/B-проверка SQL и наличия таблиц:

* `onRasprMag` (externalCodes+fragments+heatSources+objectTypes):
  24 строки, содержимое 1:1 со старой БД;
* `savePjezo` пишет в `directions`/`deployedDirections` — таблицы
  есть в `ref`;
* экспорт гидравлики: `us_out`/`ut_out` последнего расчёта фр. 1 —
  4592/3459 строк в обеих БД (свои calc id: 35 против 161 — истории
  расчётов разные, это ожидаемо);
* колонки `calc.calculation`/`us_out`/`ut_out` идентичны старой БД
  (включая `calculationid`, `fileid`) — legacy SQL работает через
  `search_path` без изменений.

Четыре команды были пустыми заглушками и реализованы:

* `onClearOut` (aClearOut) — перенос из gid6: права администратора +
  `DELETE FROM calculation`, оставляющий последний расчёт каждой пары
  фрагмент/пользователь. FK-каскадов на `calc.calculation` нет ни в
  одной из БД — поведение паритетное (дочерние `*_out` чистятся как в
  legacy, отдельных сирот не создаём: тот же SQL, та же семантика).
* `onRasList` (aRasList) — вместо MFC-дерева `CCalcTree`: таблица
  `calc.calculation` + имя фрагмента через `DbWindow` (идиома gid8).
* `onInfoGid` (aInfoGid) — реализован `GidWidget::info_gid(CFPoint)`
  (в `GidWidget.h` был inline-стаб, карточка никогда не открывалась):
  поиск узла/линии в точке и диспетчер на `info_gid(db,…)`, как в
  контекстном меню (`viewNodeCalc`/`viewLineCalc`); режим карты
  `aInfoGid` в `gidr_find.cpp` уже был подключён.
* `onExport` (aExport, «Экспорт гидравлики в TXT») — в gid6 объявлен,
  но никогда не был реализован (нет тела и привязки к сообщениям).
  Реализован заново по названию: последний расчёт активного фрагмента,
  дамп `us_out` + `ut_out` (tab-separated, с заголовками колонок) в
  выбранный TXT и открытие файла.

## Шаг 74: рассинхрон деревьев gid8 устранён + блок 4 «Запросы и отчёты» (часть 1)

**Найден дефект процесса**: работа шагов 65–73 шла в корневой копии
`gid8/` (отдельный git-репозиторий, база «qgis 4» от 24.06), а все
сборки (`gid8-net-direct`, `gid8-tgid-gis-*`) и коммиты шагов 59–64 —
в каноническом дереве `tgid_gis/gid8/gid8`. Деревья разошлись в
противоположных направлениях: в корневой копии не было шагов 59–64
(прямой `net.v_*`), в каноне — шагов 65–73. Проверка хэшей 12 ключевых
файлов: 11 различаются.

**Устранение**: `git apply --3way` не сошёлся (разные базы +
uncommitted-изменения в каноне), перенос выполнен вручную по патчу
`tools/_steps65_73.patch` (9 файлов, +411/−11): блоки 2–3 шагов 72–73
и блок 4 (ниже). Защита `isFragmentList` + `SET search_path` в каноне
уже присутствовала (шаги 70–71 были перенесены ранее). Сборка
`gid8-tgid-gis-20260802` после переноса успешна; отдельно решена
проблема окружения: `CMakeLists.txt:229` жёстко ссылается на
`C:/cpp/proj-9.4.1/src` — создан с заглушкой `H:\cpp\stub_include\proj.h`
(`proj_coord.cpp` использует Boost.Geometry, реальный PROJ не нужен).
Коммит `6cbf27eb`.

**Блок 4 (продолжение шагов 17, 21–27)**: A/B-проверка SQL
(`tools/_block4_probe.py`): `zap2/zap7/zap71` — расхождение сумм
объяснено составом `fragments` (конвертер восстановил фрагменты,
которых не было в `almatygid`, `tools/_frag_probe.py`); `zapNezak`,
`zapVnCx` (find_node_vn.sql), `us_big8/ut_big8` (onPtsTable) — 1:1.
Реализованы заглушки:

* `onTableDan` (aTableDan) — редактор таблиц: список из
  `information_schema.tables` по рабочим схемам, `DbWindow` с
  `setEdit(true)`.
* `onTuTableNeiz` (aTuTableNeiz) — ТУ без привязки к зданиям
  (`zdanie IS NULL` или битая ссылка; A/B: 177 ≡ 177).
* `onTuIst` (aTuIst) — отчёт по источникам, порт `tu/1.sql`
  (A/B 1:1 по 6 источникам, суммы нагрузок совпадают).
* `print_tu_itog` (aTuExcel) — год из диалога (был захардкожен 2011),
  убран `limit 100`, добавлена колонка ГВС средняя = макс/coef,
  исправлено имя колонки «Пар» → «Присоединенная мощность Пар».

## Шаг 75: onCreateSortNode — блок 4 закрыт (34/34)

Последний стаб блока. Перенесён из gid6 `temp.cpp` в новый файл
`gidview/sort_node.cpp`:

* `sortNodeDb` — для каждого участка МС/РС (ops.uchastok_ms 60 /
  uchastok_rs 34) граф обрезается (`vydMS`/`vydRS` +
  `copyVydGraph_new`), сортируется (`sort_line_rs_new`), порядок
  строгих узлов ПС пишется в `ops.sortnodesforuchastok`;
* `insertSortLinesToDb2` — «большие линии» (подача/обратка между
  строгими узлами) в `ops.sortlinesforuchastok` (orderID,
  pipeSectionID, lineID, nodeID1/2, totalLength, fileID, участок);
* `isPS` портирован как `isPS_sort` (в `db/temp.cpp` живёт заглушка
  `isPSstrict()==true` для `create_vyd_node_table0_BIG` — не тронута);
* `get_first_last`, `copy_ut`, `BigLine` — 1:1 из gid6;
* `isPjezo` сохраняется/восстанавливается (`save_pjezo`/
  `restore_pjezo`), прогресс `QProgressDialog`, вход — только
  администратор, подтверждение перед перестроением.

Отличия от gid6 продиктованы новой моделью «одна таблица — один
объект»: legacy `pipeSections` мёртв (`attic`, `compat.pipesections` —
read-only view), поэтому ветка «pipeSectionID==0 → INSERT INTO
pipeSections + UPDATE heatPipeSections» убрана — в новой модели
`pipesectionid` системный (= id heatpipesection), при нуле в
`sortlinesforuchastok` пишется NULL. `copyPTS` (апдейт legacy
pipeSections) и `DELETE FROM iznos` (attic-пустая) не переносились.
`map_big`/`set_map_big` в gid6 пусты — опущены.

Сборка успешна (коммит `411823cd`). Целевые таблицы до запуска пустые
(0 строк) — заполнение проверяется запуском команды из приложения.

## Шаг 76: блок 5 «Техусловия (ТУ)» — 10/10 закрыт

A/B-проба (`tools/_tu5_probe.py`): структуры `zhile` (addr, 25 кол.),
`organizatsii` (org, 37 кол.), `statetu` (ref), `tehnicheskie_usloviya`
(doc, 162 кол.), `prisoedinennaya_nagruzka_istochnikov` (ref) — 1:1 с
`almatygid`; «неизвестные» запросы дают идентичные числа
(zhile 653≡653, organizatsii 4225≡4225), годы присоединённой нагрузки
2011–2019 совпадают.

Реализованы заглушки в `gidrSlot.cpp`:

* `onNagrZd` (aNagrZd) — редактор `zhile` (физические лица), DbWindow
  с `setEdit(true)`.
* `onNagrOrg` (aNagrOrg) — редактор `organizatsii` (юридические лица).
* `onNagrZdNeiz` (aNagrZdNeiz) — zhile без привязки к зданиям
  (`zdanie=0 OR NULL`), read-only, русские алиасы колонок.
* `onNagrOrgNeiz` (aNagrOrgNeiz) — organizatsii без привязки, read-only.
* `onTuTable` (aTuTable) — редактор `tehnicheskie_usloviya` с подменой
  `sostoyanie_dogovora` на `statetu.name` (LEFT JOIN), `setEdit(true)`.
* `onPrisNagrEdit` (aPrisNagrEdit) — выбор года (MMenuDial из
  `SELECT DISTINCT god`), редактируемая таблица
  `prisoedinennaya_nagruzka_istochnikov` JOIN `istochniki_tepla`.

Уже работали и подтверждены:

* `onTuFind` (aTuFind) — поиск ТУ/договоров (`tu/tu.cpp`,
  `init_tu_find` через `transl()` + переход к зданию `zdaniya_tu`).
* `aTuOnOff` — переключатель панели ТУ: общий механизм
  `map_toolbar` (`m_barTu` наполнена: aTuNew/aTuLine/aTuInfo/aTuDel/
  aTuFind/aTuSost/aTuExcel/aTuSvod/aTuZhurnal); пустой стаб
  `onTuOnOff()` — мёртвый код, слот не подключён.
* `aPotNagr0` — шаг 24.

`aViewToolbarControlTu` — панель «Контроль технического состояния» в
gid6 состоит из команд ремонтов (ID_REMONT_INFO/CONTROL_TU/DEL/OTCHET),
которых в gid8 пока нет — переносится вместе с блоком 6 «Ремонты»
(action осознанно закомментирован в `gidrMenu.cpp:166`).

Сборка `H:\build\gid8-tgid-gis\gid8.exe` успешна.

## Шаг 77: блок 6 «Ремонты и дефекты» — обязательные команды закрыты

Инвентаризация блока (`maintenance`, 29 команд): 9 инициализированы в
legacy UI (`IM`), 20 — мёртвые кнопки (`--`, слоты пустые и не
подключены). Таблицы эксплуатации уже в `ops` (шаги 174–175); контуры
`opres`/`remont2` пусты и в `almatygid`, и в `tgid_gis` (0≡0).

### БД

* [`sql/181_return_iznos.sql`](../sql/181_return_iznos.sql) — возврат
  `calculation_iznos` и `iznos` из `attic` в `ops` (нужны для `aIznos`).
  Тест [`test_iznos_return.sql`](../sql/tests/test_iznos_return.sql) OK.

### Системный фикс SQL

* `ispr_q` / `SelectTop` (`db/db.cpp`): для PostgreSQL корректно срезается
  `TOP N` из запросов gid6/MSSQL. Без этого `ListOpres` /
  `aListOpres`/`aListRemont2` падали на синтаксисе
  `SELECT DISTINCT TOP 1000000 ...`.

### Реализованные слоты

* `setDate` + `onSetDate1/2/3` — массовая установка дат на выделенных
  участках через `heatPipeSections`
  (`lastTransDate` / `firstPICdateHP` / `repairDatePlanTP`), порт
  gid6 `set_obl.cpp::setDate`.
* `aSetPipeRemontType` — уже работал через `setSomething` (`pipeRemontTypeID`).
* `openRemont2List` + `onRemontPlan` / `onRemontCurrent` /
  `onRemontProcess` / `onRemontVypolneno` / `onRemontAll2` — редактируемые
  списки `remont2` с фильтрами по `remontTypeID`/`stateID` (вместо
  MSSQL-журнала `remonts_journal.sql` с `STUFF`/`FOR XML`).
* `aListOpres` / `aListRemont2` — уже были; теперь SQL проходит на PG
  после фикса `TOP`.
* `aBottomRemont` — toggle нижнего dock (уже работал).
* `aEditor2` / `aFilePrintPreview` — чистый UI, к БД не привязаны.
* `onIznos` — создание/обновление `ops.calculation_iznos` + заготовки
  строк в `ops.iznos` из `pipesections`, таблица с join на
  `heatpipesections`. Полный %‑отчёт `remont_sql/tgid_iznos.sql`
  остаётся на MySQL-диалекте (`IF`/`TIMESTAMPDIFF`) — отдельный порт
  SQL при появлении реальных данных износа.

### Мёртвые в legacy (не трогаем)

`aNaprOpres/Remont2`, `aSaveOpres*`, `aSaveRemont2*`, `aRemonts*`,
`aZhurnalDefect/Remont/Diag`, `aViewToolbar*Remont/Opressovka` и т.п. —
объявлены, но не инициализированы в старом UI (`--` в инвентаризации).

`aViewToolbarControlTu` — по-прежнему закомментирован в меню: отдельной
панели CONTROL_TU в gid8 нет; рабочие панели ремонтов —
`m_barRemont` / `aToolbarRemont2*`.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 78: блок 7 «Коррозия» — обязательные команды закрыты

Инвентаризация блока (`corrosion`, 17 команд). Таблицы
`ops.indikator_korrozii` / `indikator_korrozii_po_godam` и справочник
сезонов уже в схеме (шаги 174–175); `org.responsibles_korrozia` пуст
и в `almatygid`, и в `tgid_gis`.

### Реализованные слоты

* `openKorrozTable` — общий PG-native SELECT по `indikator_korrozii`
  с фильтром отопительного сезона (`Heating_seasons` /
  `m_sezon_korrozia`) и дополнительным WHERE.
* Журналы/таблицы: `onKorroziaUpdate`, `onKorrozDoc3`, `onKorrozTable`,
  `onKorroziaZhurnal1/2/3`, `onKorroziaCurrent`, `onKorrozDoc1/2` —
  через `openKorrozTable` + фильтры по датам/оценке.
* Карта: `onKorrozAdd` (`addGeo`), `onKorrozDel` / `onKorrozInfo`
  (`m_nRegim` + `pts_del`/`pts_info` в `gidr_find.cpp`),
  `onFragmentPolyKorroziaAr` — подсказка (массовое удаление через
  таблицу/контекстное меню).
* `onSetKorrozia` — дата планирования для индикаторов у выделенных
  участков (`ST_DWithin` + `create_vyd_line_table`).
* `onKorroziaOnoff` — `geoOnOffFile("remont", ...)`.
* `onKorroziaSezon` — делегирует в `onRemontPovrOtop()` (общий выбор
  сезона).
* `aViewToolbarKorrozia` — toggle через `map_toolbar` /
  `onToolbarToggleExcl` (панель собирается в `gidrToolBar.cpp`,
  ribbon — в `mainribbon.cpp`). Пустой stub
  `onViewToolbarKorrozia` в закомментированном блоке не используется.

### Не переносим как отдельные MSSQL-отчёты

Полные отчёты `sql/korrozia/*` / `indikator_korrozii.sql` (IIF/ISNULL/
TOP) заменены упрощёнными редактируемыми таблицами. Word-шаблоны
оценки — отдельный порт при появлении реальной потребности в
docx-выгрузке.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 79: блок 8 «Паспортизация» — обязательные команды закрыты

Инвентаризация блока (`passport`, 3 команды) + рабочие слоты панели
ПТС, без которых блок бесполезен.

### Команды блока

* `aViewToolbar2PtsNew` — уже работал через `map_toolbar` /
  `m_barPts` (`gidrToolBar.cpp`).
* `onAlma` — checkable toggle режима паспортизации: `m_bIsPts`
  (скрывает стрелки потоков при отрисовке), сохранение в
  `flags/isPts` / `flags/isPTS`.
* `onPaspNew` / `onMsPassport` — формирование паспорта МС/РС:
  текущий выбор из `DockPTS` (`currentMsRs`) либо диалог выбора
  `uchastok_ms` / `uchastok_rs`, затем `Passport()` →
  `passport_ps/p.py` (xlsx).

### Панель ПТС (сопутствующие слоты)

* `onPtsAdd` / `onPtsTable` / `onPassportOnOff` — уже были.
* `onPtsInfo` / `onPtsDel` — запасной путь в режим клика
  (`m_nRegim`; основной путь — `regimGroup` / `onRegim`).
* `onPtsZhurnal` — выбор объекта ПТС и редактируемая таблица через
  `getGeoSelect` (вместо gid6 `initPassport` только для heatPoint).

### Деплой Python

POST_BUILD в `CMakeLists.txt` копирует
`gid8/python/docs/passport_ps` рядом с exe
(`python/docs/passport_ps`), куда смотрит `str_python()`.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 80: блок 9 «Теплопотери» — команды уже на месте

Инвентаризация (`heat_losses`, 3 команды) — слоты не пустые:

* `onPoteri` (`zaprosy.cpp`) — сумма `tpot` по системе / выделенному
  фрагменту (сообщение).
* `onSetOpenRez` — `setSomething` для `circHLosOpen` /
  `avgHLcompOpen` на потребителях.
* `onTeplopoteri` — диалог сезона → запуск
  `poteriNewPg/tp_main.py` (таблицы отчётов возвращены в шаге 175).

POST_BUILD копирует `gid8/python/poteriNewPg` рядом с exe
(`python/poteriNewPg`).

## Шаг 81: блок 10 «GIS-обмен» — основные пустые слоты закрыты

Были уже на месте: `onBmp`, `onExportFragment`/`Vyd`, `onImportFragment`/
`Mdb`, `onQuery`, `onZapNullGeo`, `onExportDxf`; `aGeobaza` —
`addToggleValue(&m_bIsGeo)`.

### Заполнены

* `onExportFragmentsMulty` — массовый экспорт открытых фрагментов в
  каталог (`.tgid` zip).
* `onBmpFind` / `onPicture` — центр DXF-подложки; toggle
  `m_bIsPicture` (отрисовка в `gidr_draw.cpp`).
* `onEditCopyPng` / `onSavePng` — grab схемы в буфер / файл.
* `onGeodz` — массовая установка `geoMarkTopTube` /
  `geoMarkNodeArea` на выделенных узлах.
* `onFindGeoAddr` — показать `DockAddr`; `onFindGeo` — проводник
  карты (полный `find/findNext` геобазы gid6 не в `GeoFile`).
* `onGeoInfo` — режим клика `aGeoInfo`.
* `onImportPts` → `onImportFragment`; `onFileOpenOld` →
  `onImportFragmentMdb`.
* `onResetGeo` — перерисовка.

### Отложено (заглушки с сообщением)

* `onExportShape` — нет порта `cxema->exportShape`.
* `onGeoFindNext` — нужен API поиска в `GeoFile`.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 82: блок 11 «Администрирование» + `aIspravl`

### Уже работало

* `aCatalog` → `MainWindow::onCatalog` (PropertyDial путей документов).
* `aAddUser` / `aBdDel` / `aPassword*` / `aSetupGid9` /
  `aGoogleElevationSet` / `aWmsCust`.
* `aSwap` — режим `regimGroup` + `swap(line)` в `gidr_find.cpp`.
* `aSetOrg` — `setSomething(organizationID)`.

### Заполнено

* `onUpdateSetup` / `onUpdate` (`mainSlot.cpp`) — код предприятия в
  `QSettings("UpdateSetup")`; повтор обновления открывает info-URL
  (полный `UpdateProgr`/zip с lan.avto-glass.kz не переносим).
* `GidWidget::onCatalog` / `onUpdate*` — делегируют в `MainWindow`
  (слоты сделаны `public slots`).
* `onIspravl` — пересоздание внутренних схем реальных потребителей
  через `create_b5_new` + progress.
* `onOpc` — явное сообщение: COM OPC DA в Qt-порт не входит.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 83: блок «Прочее» — первая порция пустых слотов

Заполнены (порты gid6 / Qt-аналоги):

* `onSqlSave` — для PostgreSQL: `CREATE DATABASE … WITH TEMPLATE`
  (служебное подключение к `postgres`, terminate backends, краткое
  закрытие рабочей сессии); для MSSQL — `BACKUP`/`RESTORE` как раньше.
* `onInvert` — инверсия `isPjezo` на узлах и участках.
* `onSetAvtoOn` / `onSetAvtoOff` — `automDegID` + `pr_avtomat` на
  выделенных потребителях.
* `onAppExit` — `QApplication::quit()`.
* `onViewStatusBar` — показать/скрыть status bar главного окна.
* `onSetAddr` → панель адресов (`onFindGeoAddr`).
* `on1001` — URL переименования полей (как gid6).

Остаются в «Прочее»: ТУ/электрика/C3, повороты, зона, часть запросов
и служебных команд — следующий проход. Спорные потребители — в конце.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 84: блок «Прочее» — вторая порция

Рабочие порты / аналоги gid6:

* `onPovorot` / `onPovorot2` — угол через `set_coord` / сброс.
* `onF5` — перерисовка (`Scroll::onF5`).
* `onZona` — `WS1`/`WS1_h` + сообщение + переход к узлу.
* `onSetTp` — `heatPointID` через `setSomething`.
* `onSetIst` — меню узлов (`mark>1`) + `moveXY`.
* `onQGvs` — `QSettings("coef24")`.
* `onPodpPo` / `onPopupPodp*` / `onNaprGid` / `onPribor` /
  `onPsAstanaName` — флаги отображения.
* `onTuYear` / `onTuZav` — таблица ТУ по году; статус «Завершён».
* `onElectroAdd/Info/Del/Table` — слой `electro` (+ клики в
  `gidr_find`).
* `onUtNapr` / `onUtNapr2` / `onC3Add` → `onCreateSortNode` /
  `onIspravl`.
* `onMapPath` — каталог карт.

С явным сообщением (пока не портятся целиком): `onZhurnalElectro`,
`onPrPo`, `onSetPsMap`, `onUchList`, `onCheckPo`, `onAsyncCheck`,
`on1000`/`on1002`, `onTrio`/`onC3Del`.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 85: отчёты ТУ — свод и журнал (Excel)

Вместо HTML/`print_doc` из gid6 (класса `HTML` в Qt нет) —
`tu/tu_reports.cpp` на QXlsx (как `tu_itog`; подсказка журнала — Excel):

* `onTuSvod` → `print_tu_svod` — листы по годам, раскраска
  статусов З/А/М, легенда.
* `onTuZhurnal` → выбор года → `print_tu_zhurnal` — листы по
  районам эксплуатации, разбивка нагрузок, итоги, `coef24` для ГВС ср.

Файлы: `%TEMP%/tgid_tu_reports/`, открытие через `QDesktopServices`.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 86: карточки труб/узлов — колонки view

`get_table_columns` (PG) смотрел только `relkind = 'r'`, а
`nodes`/`heatPipeSections`/… после миграции — `net.v_*` (views).
Интроспекция возвращала пусто → `read_tab` не матчил whitelist из
`tab/*.txt` → SELECT без полей подтипа → в PropertyDial подписи без
редакторов.

Исправление в `db/init_db.cpp`: резолв имени через `tbl_sql`,
`relkind IN ('r','v','m')`.

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 87: создание участка — `reset_shape` на PG

При добавлении линии `gidr_add` → `reset_shape_line` падал на SQL вида
`WHERE l.id=linesobj.id`: целевая таблица `net.v_linesobj`, алиаса
`linesobj` нет (то же в `reset_shape_node_pg` и `setNodeXY`:
`nodes.id` / `linesobj.id`).

На PG geom всё равно пересчитывает INSTEAD OF-триггер из x/y и
coords (`050_write_triggers.sql`), колонка `shape` в SET не пишется.
PG-ветка `reset_shape_*` переписана: `UPDATE … AS l/n SET coords/x = …`
с корректными алиасами, чтобы сработал rebuild. Заодно `setNodeXY` и
`getNodeQ` (`AS nodes`).

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 87: реестр команд меню и два дефекта, которые он нашёл

Проверка «все ли команды переведены» до сих пор велась по блокам этого
документа, то есть по записям о проделанной работе. Это проверка
намерения, а не результата. Появился независимый реестр —
[`tools/menu_audit.py`](../tools/menu_audit.py): он идёт от разметки
меню к коду (`gidrMenu.cpp` → `gidrAction.cpp` → тело слота) и
классифицирует каждую команду. Признак дефекта — не вхождение имени в
список «старых», а фактический резолвинг в живой БД: имя, которое
резолвится в `ref` или `net`, правильное, а в `compat` — нет.
(`deployeddirections` живёт в `ref` и по списку выглядел бы дефектом.)

Всего команд меню — **387**.

### Дефект 1: 10 команд держались на compat

`onEditUndo`, `onFindKti`, `onSetKorrozia`, `onMagFragment`,
`onColorLine`, `onZap1`, `onZap2`, `onZap7`, `onZap71`, `onIznos`.
Переведены [`tools/subst_menu_names.py`](../tools/subst_menu_names.py) —
35 замен в 14 файлах, только имена в позиции таблицы, комментарии и
блоки `#if 0` пропущены (в `GidWidget.cpp` и `opres.cpp` внутри `#if 0`
лежат отключённые запросы, правка которых создала бы впечатление живого
кода). Совпадение колонок проверено до замены: 151:151, 26:26, 44:44,
16:16, 6:6, ни одной недостающей. Отчёт «Объём сети» на фрагменте 2
сверен с `almatygid`: 2338.8957492829254 против 2338.8957492829286 —
расхождение только в порядке суммирования.

Стало: `COMPAT` 10 → **2**, `NET` 10 → 18.

### Дефект 2: у уставок давления было два источника

`public.setpressnodes` (5740 строк, настоящая таблица) и
`net.node_press_setting` (5733). Кто какой увидит, зависело от
`search_path`: у gid8 первым идёт `compat` → он читал `net`; у `sety`,
`potr5` и инструментов `compat` в пути нет → они читали `public`.
**Уставка, сохранённая в программе, до расчёта не доходила**, и ошибки
при этом не возникало.

[`sql/182`](../sql/182_setpressnodes_net.sql): каноническое
`net.v_setpressnodes` с тремя триггерами `INSTEAD OF` по образцу
остальных `net.v_*`, `compat.setpressnodes` — тонкая обёртка над ним,
исходник переведён в `attic`. Семантика не менялась: «id» — это
`src_id`. Приём из `sql/179` (`COALESCE(src_id, id)`) здесь неприменим —
`id` тут обычный serial 1..5733, а `src_id` доходит до 7825, диапазоны
пересекаются.

Движок переведён на резолвер: `net_mode.tbl(conn, 'setpressnodes')` в
`read_gid.py` и `read_zn.py` (**обе** копии sety). Разница в 7 строк —
сироты, их узлов нет в `net`; соединение с узлами отсекает их в обоих
вариантах. Проверено построчно (`EXCEPT` в обе стороны — 0) и
контрольным прогоном: расчёт через `net` и через `attic` совпал по всем
полям всех таблиц.

Сторож — `sql/tests/test_setpressnodes_edinstvennyy.sql`; тестов стало
**29 из 29**.

`potr5` (4 ссылки) не собран, вне репозитория, последняя правка 24 июня —
та же категория, что `gid6`: донор, а не продукт. Если его будут
оживлять, потребуется та же однострочная правка.

### Что реестр показал сверх этого

`ПУСТО` 62 (44 из них в меню «Ремонт»), `НЕТ СЛОТА` 73, `SQL?` 28 —
запросы, где имя подставляется в рантайме и статически не проверяется.
Не разобрано: пустой слот может быть и задуманным (команда не
реализована и в исходной программе), и потерянным при переносе.

### Побочная находка: сдвиг температур

Фрагмент 2, параметры прогонов идентичны (`calc_params` совпадают
побайтно): расчёт 158 от 02.08 — средняя `t` 125.75, максимум 131.94;
расчёт 164 от 03.08 — 114.76 и 120.81, разошёлся 3421 узел. К переводу
`setpressnodes` это отношения не имеет (контрольный прогон 165 совпал с
164 полностью). Причина — в изменениях шагов 68–86. По эталону
`almatygid` не рассудить: там `t` для реальных узлов NULL.

## Шаг 88: пилот карты — тайлы HTTPS + поиск геобазы

Без смены SoT на WGS84 (рабочий `geom` остаётся SRID 9998).

* Подложка: `User-Agent` `tgid-gid8/1.0` в `download.cpp`/`download2.cpp`;
  Google/Yandex → `https://…`; Nominatim → https.
* `GeoFile::find` / `findNext` — выбор слоя, ILIKE по id и текстовым
  колонкам (до 500), `onFindGeo`/`onGeoFindNext` → `moveGeo` (+ fallback
  центроид из `ST_Centroid`).

Сборка `H:\build\gid8-tgid-gis-20260802\gid8.exe` успешна.

## Шаг 88: растровые карты MapTiler

Добавлены четыре подложки по просьбе заказчика: `streets-v4`, `topo-v4`,
`hybrid-v4` и `openstreetmap` — все с суффиксом `@2x`. Первые две
приходят в PNG, гибридная и OSM запрошены в JPEG, поэтому обе
добавлены в `isJpeg` (иначе файл лёг бы на диск с чужим расширением).

Почему растр, а не вектор. Векторные тайлы (`.pbf` + `style.json`) —
это не «ещё один провайдер», а замена всего пути отрисовки: сейчас
`drawMap` кладёт готовые картинки через `QPainter`, а вектор требует
раскодировать protobuf, интерпретировать стиль Mapbox GL, самому
расставить подписи и подтянуть глифы со спрайтами. Практически это
MapLibre Native — GL-контекст, который надо синхронизировать с
внутренней системой координат (сантиметры, перевёрнутый Y, SRID 9998)
на каждом сдвиге. Отдельно: векторный слой зданий MapTiler заказчику не
нужен — в `net.building` уже 187 202 здания с геометрией, свои и точные.

`@2x` даёт картинку 512×512 на клетку схемы 256. Правок отрисовки это
не потребовало: `drawPic` масштабирует изображение под целевой
прямоугольник независимо от исходного размера
(`image.scaled(rect.Width(), rect.Height()+2, …)`), поэтому удвоенное
разрешение просто даёт более чёткий результат.

Тронуто шесть мест — ровно те, что описывают провайдера:

* [`maps/maps_id.h`](../gid8/gid8/maps/maps_id.h) — три кода **в конец**
  перечисления. Выбранная карта хранится в настройках числом
  (`flags/internetMap`), вставка в середину молча сменила бы подложку
  всем, кто уже пользуется программой;
* [`maps/maps_u.cpp`](../gid8/gid8/maps/maps_u.cpp) — `get_url`,
  `get_path` (каталоги кэша `MAPTILER_*`), `isJpeg` (гибридная приходит
  в JPEG), `isMap`, `maxNmap` (21; запросами проверено, что тайлы
  отдаются вплоть до z=22), плюс обе таблицы соответствий в `setMaps`
  и `onMaps` — без второй выбор пункта не давал бы ничего;
* `gidrAction1.h` / `gidrAction.cpp` / `gidrMenu.cpp` / `gidrSlot.cpp` —
  объявления, создание в `mapGroup`, пункты основного и контекстного меню.

**Ключ в исходники не попадает.** `maptiler_key()` читает
`maps/maptiler_key` из настроек, затем переменную `MAPTILER_KEY`; при
пустом ключе `get_url` возвращает пустую строку и провайдер просто не
запрашивается. Порядок тот же, что у пароля БД — в этом проекте уже был
случай, когда в коммит чуть не уехал токен Google
(`gid6/dop/converter_old32`).

## Побочно: состояние интернет-карт

Проверено запросами и запуском программы:

* работают: Google (карта/спутник/гибрид), OpenStreetMap, 2ГИС, ESRI,
  VISICOM. Программа скачала 128 настоящих PNG 256×256, запросив
  тайлы `11/1462/750` — это Алматы, привязка верна;
* **«Карта WMS» не работает в принципе** — тело построителя адреса
  целиком закрыто `#if 0` (`maps_u.cpp`, около строки 470), URL всегда
  пустой;
* «Семей (город)» и «Семей (тепловые сети)» — `vkomap.kz` отдаёт 301,
  затем 403. Мертво на стороне сервера;
* Яндекс не отвечает, но его и нет в меню.

Перенос БД к картам отношения не имеет: привязка берётся из
`heatSystem` → `ref.heatsystem`, и параметры проекции совпадают с
эталоном `almatygid` побайтно (центральный меридиан 76.9166666666667,
false_easting −3021). Сквозная проверка через PostGIS: узлы
пересчитываются в 76.84 / 43.21.

Не подтверждено: сама отрисовка тайлов на экране — окно программы
маскируется на снимке экрана, скачивание проверено, показ глазами нет.

## Шаг 89: скорость и плавность карты

Растровые тайлы `@2x` вчетверо тяжелее прежних, поэтому две давние
особенности отрисовки из терпимых стали заметными.

**Кэш плиток не имел вытеснения.** `images` был
`std::map<QString, QImage>` без единой чистки: каждая once показанная
плитка оставалась в памяти до закрытия программы. При 256×256 это
сходило с рук, при 512×512 разобранная плитка занимает около мегабайта,
и прогулка по городу набирает гигабайты. Заменён на `QCache` с
вытеснением по давности, потолок 256 МБ. Замер после правки: рабочий
набор процесса держится около 200 МБ вместо неограниченного роста.
Заодно это снимает подозрение с аварийных дампов `gid8.exe` от 3
августа (21:33 и 21:38) — причину они не доказывают, но исчерпание
памяти было правдоподобным кандидатом.

**Масштабирование повторялось на каждом кадре.** `drawPic` выполнял
`image.scaled(..., Qt::SmoothTransformation)` для каждой плитки при
каждой перерисовке, хотя при сдвиге карты целевой размер не меняется.
Добавлен второй кэш — уже отмасштабированных плиток, ключ «имя файла +
размер на экране», потолок 128 МБ. Теперь сдвиг попадает в кэш целиком,
а пересчёт остаётся только при смене масштаба.

Тонкости, которые пришлось учесть:

* `QCache::insert` при неудаче (запись дороже всего кэша) **удаляет**
  переданный объект, поэтому и `getImage`, и `drawPic` рисуют из
  локальной копии `QImage`, а не из указателя кэша. `QImage` неявно
  разделяемый — копия не копирует пиксели;
* стоимость записи не может быть нулевой, иначе `QCache` её не
  вытесняет: `image_cost_kb` возвращает минимум 1;
* сброс масштабированных плиток при перечитывании делается один раз в
  `redrawMap`, а не в `getImage`: последний вызывается на каждую
  плитку, и чистка всего кэша повторялась бы десятки раз за кадр;
* `remove_file` (удаление плитки с диска) чистит оба кэша — иначе на
  экране осталась бы картинка, которой на диске уже нет.

Проверено запуском: `MAPTILER_OSM` — 68 плиток JPEG, `MAPTILER_STREETS`
— 248 плиток PNG 512×512, нулевых файлов нет.

Не измерено: сам прирост плавности в кадрах — окно программы
маскируется на снимке экрана, оценить глазами не удалось. Объективно
подтверждены только устранение неограниченного роста памяти и
исчезновение повторного масштабирования.

## Шаг 90: вместо белых квадратов — плитка соседнего масштаба

Пока плитка нужного масштаба качается, `getImage` возвращал «нет», и
`drawPic` не рисовал ничего — на месте плитки оставался белый
прямоугольник. Карты так не делают: они показывают уже загруженную
картинку соседнего масштаба, растянутую на это место, и подменяют её,
когда придёт настоящая.

Реализовано в `drawPic` двумя запасными путями:

* `drawParentTile` — поднимается вверх до пяти уровней и рисует кусок
  родительской плитки. Искомая занимает 1/2^k родительской, её место
  внутри — остаток от деления координат. Работает при **увеличении**
  масштаба: плитки прежнего вида ещё в кэше;
* `drawChildTiles` — собирает место из четырёх дочерних плиток. Работает
  при **уменьшении**, когда более крупного масштаба ещё не существует.
  Глубже одного уровня не идём: там уже 16 плиток на одно место, и
  собирать дороже, чем дождаться загрузки.

Глубина подъёма ограничена пятью уровнями осознанно: каждый шаг
растягивает картинку вдвое, после пятого это мыло крупнее экрана, от
которого пользы меньше, чем от пустого места.

При перечитывании (`reread`) оба пути отключены: соседние масштабы тогда
такие же устаревшие, а `getImage` их ещё и вычистит из кэша.

`drawPic` получил параметр `map_typ` — без него не собрать имя файла
соседней плитки (`get_path` + `get_name` зависят от провайдера, в том
числе расширением: у гибридной и OSM это `.jpg`).

Проверено запуском с пустым кэшем — случай, когда соседних плиток нет
вовсе: программа не падает, плитки качаются, память 146 МБ.

Не проверено: как это выглядит на экране. Окно программы маскируется на
снимке, поэтому подтверждена логика и отсутствие падения, а не картинка.

## Шаг 91: чистка чердака и русские подписи слоёв

**Удалять в живых схемах оказалось нечего.** Разбор
[`tools/dead_tables.py`](../tools/dead_tables.py) по 558 таблицам схем
net/ref/ops/org/doc/el/meta/addr/calc: ноль кандидатов, все либо с
данными, либо на них кто-то ссылается.

Первая версия инструмента выдала 77 «кандидатов» — включая `ops.shurfy`
(11 файлов gid8), `ops.iznos` (`gidrSlot.cpp`), `ops.osmotr`,
`ops.sortnodesforuchastok`. Причина: `code_refs()` принял ключи
`sql`/`words`/`made`, которые возвращает `db_consumers.scan()`, за имена
таблиц и не нашёл ссылок вообще ни у кого. Поймано сверкой с
`db_consumers --table`. Удаление по тому списку снесло бы живые таблицы.

**Чердак сокращён со 167 таблиц до 6** ([`sql/183`](../sql/183_drop_attic.sql)),
208 МБ освобождено. Остались те, что читают представления `compat`:
`magistrali`, `internalnodes`, `pavilions`, `overgroundnodes`,
`undergroundnodes`, `uninstallednodes`. Перед удалением снят полный дамп
(105 МБ), его оглавление проверено: 5786 записей, 334 по attic.

Проверено до применения: внешних ключей на attic извне — ноль,
представлений вне attic, читающих attic, — ровно шесть, все исключены.

**Чего разбор не учёл:** каталог `sql/tests` читает attic как эталон, с
которым сверяется перенос. После удаления упало 9 тестов. Девять таблиц
восстановлены из дампа (`fragments`, `nodes_legacy`, `linesobj_legacy`,
`realconsumers_legacy`, `generalizedconsumers_legacy`,
`heatpipesections_legacy`, `setpressnodes` и др.), чердак — 13 таблиц.

**Два теста упали не из-за удаления.** `test_disconnected_consumers` и
`test_zero_load_consumers` сверяют живой `net` с замороженным снимком, а
3 августа в 20:44 через программу завели потребителя 605612 — журнал
`meta.object_change_log` показывает `application_name = Tgid-07`, INSERT
в `net.node_plain` и `net.consumer_real`, затем UPDATE `schemenum`.
Это подтверждение, что путь записи работает вживую. Тесты исправлены:
новая сторона ограничена узлами, присутствующими в снимке
(`EXISTS ... attic.nodes_legacy`). Столбец `id_old` для этого не годится
— он нулевой у 91 491 узла из 91 532. Итог — **29 из 29**.

**Русские подписи** ([`sql/184`](../sql/184_layer_names_ru.sql)):
`meta.layer_catalog.display_name_ru` существовал, но был заполнен копией
латинского имени. Наполнен по-настоящему для всех 41 слоя, названия
взяты из терминологии самой программы (`pipe_section` — «Участки
трубопроводов», а не «секции труб»).

Решение заказчика: русскими делаются **только подписи**, имена таблиц
остаются латиницей. Поэтому весь SQL в gid8, sety, converter,
инструментах и 29 тестов работает без единой правки.

Осталось: научить gid8 читать `meta.layer_catalog` — сейчас он к нему не
обращается вовсе, а русские названия объектов зашиты по коду
(`cxema/cxema1.cpp`, `gidview/analiz.cpp`).

## Шаг 94: подписи слоёв из БД + починка журнала

`initTableRusNameFromCatalog()` читает `meta.layer_catalog` и вливает
`display_name_ru` в ту же карту, из которой берёт `findTableRusName`.
Ни одно место интерфейса править не пришлось: экспорт фрагмента,
проводник геобазы и диалоги невидимых объектов уже спрашивают подпись
через неё. Вызов — из `configureTgidPostgreSqlSession`, единственной
точки, через которую проходят все подключения.

Проверено запуском `--db-smoke`: «подписи слоёв из meta.layer_catalog:
41», search_path верный, 91 491 узел, 25 фрагментов.

Каталог перекрывает файловые подписи `kls/gid.txt` осознанно — это
единственный источник, правимый без пересборки. Пересечений почти нет: в
файле имена дореформенной модели, в каталоге классы net.

### Две ошибки в собственной правке журнала (шаг 92), пойманные проверкой

Отказ от сброса на диск на каждой строке дал экономию, но:

* при аварийном снятии процесса терялся хвост журнала — а gid8 падал,
  дампы лежат в CrashDumps. Теперь сброс каждые 64 сообщения и всегда на
  предупреждениях и ошибках: потерять можно не больше 63 строк;
* при **обычном** выходе хвост терялся тоже: поток создан в куче,
  деструктор не вызывается. Обнаружено на `--db-smoke` — в файле
  оставалась одна строка вместо вывода самопроверки. Добавлен
  `qAddPostRoutine` со сбросом и закрытием файла.

## Шаг 95: замер чтения всех фрагментов сразу

[`tools/measure_fragments.py`](../tools/measure_fragments.py) выполняет
те же два запроса, что и открытие схемы (`sql3/us.sql` — узлы,
`sql3/ut.sql` — участки), с подстановкой списка фрагментов. Прогрев
отбрасывается, берётся медиана повторов.

| Что открываем | Узлы | Участки | Объектов |
|---|---|---|---|
| один фрагмент | ~200 мс | ~39 мс | 4 026 |
| **все 25 фрагментов** | **~1 040 мс** | **~550 мс** | **167 321** |

То есть вся схема целиком читается из БД примерно за **1,6 секунды**.

Разбор плана запроса узлов (один фрагмент): время выполнения 284 мс, из
них **61 мс — только планирование**. Запрос огромен: `net.v_nodes` —
UNION ALL двадцати с лишним таблиц-классов, и каждая соединяется с
результатами расчёта. В плане 21 последовательное сканирование против 5
по индексу; целиком читаются таблицы результатов `us_out` (дважды,
52 900 и 33 363 строки) и `pt_out` (20 564).

### Индексы на таблицах результатов не помогли

Напрашивалось: соединение идёт по паре «расчёт + узел», а индексы были
только по `calculationid`. Составные `(calculationid, nodeid)` на
`us_out` и `pt_out` дали на первом замере 409 → 247 мс, и это выглядело
как выигрыш 40%.

**Парная сверка это опровергла.** Прогон обоих состояний подряд, по 5
повторов: без индексов 201/39/1036/551 мс, с индексами 213/46/1089/540.
Разница в пределах шума. Первое «улучшение» было холодным кэшем страниц,
а не эффектом индекса. Индексы удалены — они стоили бы времени на записи
и места на диске без пользы.

Правило подтвердилось на практике: сравнивать только парными прогонами в
одном сеансе, одиночный замер до/после здесь ничего не значит.

### Где остаётся запас

Не в индексах, а в форме запроса: 61 мс планирования на каждое открытие
и полное чтение таблиц результатов. Уменьшить это можно, только сузив
сам запрос — например, не тянуть результаты расчёта, когда режим не
отображается. Это отдельная работа с проверкой на совпадение выводимых
данных.
