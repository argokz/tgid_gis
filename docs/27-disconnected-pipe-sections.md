# Отключённые участки (`onUtZakrAll`)

Старая команда `onUtZakrAll` перенесена в новый Qt-клиент как отдельный
законченный сценарий. Она показывает трубопроводные участки, отсутствующие в
расчётной выдаче `UT_OUT`.

## Фактическое условие старого SQL

В старом файле `UtZakrAll.sql` был подзапрос последнего расчёта фрагмента, но
он соединялся после `LEFT JOIN UT_OUT`, а итоговый фильтр требовал
`UT_OUT.id IS NULL`. Поэтому номер последнего расчёта не влиял на результат:
участок попадал в список, только если для него вообще не существовало строки
`UT_OUT` ни одного расчёта.

Новый запрос намеренно сохраняет это поведение:

- `net.pipe_section` активен;
- оба конечных узла активны;
- начальный узел не является внутренним;
- для `pipe.src_id` отсутствует любая строка `calc.ut_out`;
- фильтр фрагмента применяется к `fileid` начального узла.

Приложение читает только `net.pipe_section`, `net.v_nodes`,
`ref.externalcodes` и `calc.ut_out`. Legacy-представления используются только
регрессионным тестом.

## Проверка эквивалентности

[test_disconnected_pipe_sections.sql](../sql/tests/test_disconnected_pipe_sections.sql)
сравнивает семь полей исходного и нового запросов в обе стороны через
`EXCEPT`. На рабочей базе оба результата содержат 76 161 строку, различий нет.

## Qt и CLI

Вкладка показывает ID, исходный ID, коды и имена обоих узлов, паспортную
длину, внутренний диаметр и фрагмент. Доступны фильтры, переход в карточку
`pipe_section` и UTF-8 CSV.

```powershell
.\tgid_app\build\Release\tgid_app.exe --disconnected-pipe-sections
.\tgid_app\build\Release\tgid_app.exe `
  --disconnected-pipe-sections --disconnected-pipe-fragment 72
.\tgid_app\build\Release\tgid_app.exe `
  --disconnected-pipe-sections --disconnected-pipe-search Абая
```

Некорректный `--disconnected-pipe-fragment` возвращает код 50, ошибка запроса
к БД — код 51. Один запрос возвращает не более 5 000 строк.
