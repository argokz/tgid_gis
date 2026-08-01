# Закрытые участки (`onUtZakr`)

Старая команда `onUtZakr` перенесена в новый Qt-клиент без изменения
бизнес-условия. Отчёт не зависит от гидравлического расчёта: состояние хранится
непосредственно в объекте трубопровода.

## Условие отбора

В результат входит участок, для которого одновременно выполняются условия:

- объект `net.pipe_section` активен (`removed_at IS NULL`);
- оба конечных узла активны;
- начальный узел не является внутренним (`internalnodeid IS NULL`);
- `pipesectstateidflow = 2` для подающей трубы;
- `pipesectstateidret = 2` для обратной трубы;
- при выборе фрагмента используется `fileid` начального узла, как в старом
  SQL.

Новый запрос читает только канонические `net.pipe_section`, `net.v_nodes` и
`ref.externalcodes`. `public.linesobj`, `public.heatpipesections` и
`public.nodes` приложению не требуются.

## Проверка эквивалентности

Тест [test_closed_pipe_sections.sql](../sql/tests/test_closed_pipe_sections.sql)
сравнивает полный набор полей старого и нового запросов в обе стороны через
`EXCEPT`. На рабочей базе оба варианта возвращают 722 строки, различий нет.

## Qt и CLI

Вкладка **Закрытые участки** показывает ID, исходный ID, коды и имена обоих
узлов, паспортную длину, внутренний диаметр и фрагмент. Есть фильтр по
фрагменту, текстовый поиск, переход в карточку `pipe_section` и UTF-8 CSV.

```powershell
.\tgid_app\build\Release\tgid_app.exe --closed-pipe-sections
.\tgid_app\build\Release\tgid_app.exe `
  --closed-pipe-sections --closed-pipe-fragment 72
.\tgid_app\build\Release\tgid_app.exe `
  --closed-pipe-sections --closed-pipe-search Абая
```

Некорректный `--closed-pipe-fragment` завершает CLI с кодом 48, ошибка запроса
к БД — с кодом 49. Вывод ограничен 2 000 строк.
