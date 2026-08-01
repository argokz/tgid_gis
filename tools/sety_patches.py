"""Реестр наших правок в движке расчёта sety и их проверка.

Движок живёт вне этого репозитория — в `gid8/python/sety`, 103 файла и
24 679 строк чужого кода. Копировать его сюда нельзя: получится форк,
который начнёт расходиться с оригиналом. Но и оставлять правки
неучтёнными нельзя: при переустановке или обновлении sety они молча
исчезнут, а обнаружится это в лучшем случае замедлением расчёта,
в худшем — расчётом по старым запросам к несуществующим таблицам.

Здесь перечислено, что именно изменено и по какому признаку это видно
в исходнике. Проверка не заменяет регрессию расчёта, она отвечает
на другой вопрос: «наши правки вообще на месте?»

    python tools/sety_patches.py            # проверить
    python tools/sety_patches.py --list     # просто показать реестр
"""
import argparse
import os
import sys

SETY_DIR = r'H:\projects\tgid-app-new\gid8\python\sety\sety'

# (файл, признак в тексте, что это и зачем)
PATCHES = [
    ('net_mode.py', 'def line_query',
     'Новый модуль: запросы напрямую к схеме net, без слоя совместимости. '
     'Генерируется tools/gen_sety_net_mode.py.'),
    ('net_mode.py', 'def is_net',
     'Определение, что БД уже переведена на net — иначе движок работает '
     'по старым запросам.'),
    ('net_mode.py', "table_schema = 'public' AND table_name = 'nodes'",
     'is_net требует, чтобы public.nodes был ПРЕДСТАВЛЕНИЕМ. Одного '
     'наличия схемы net мало: в исходной БД она остаётся после пробной '
     'конвертации и устаревает, а чтение из неё даёт неполный расчёт '
     'без единой ошибки.'),
    ('net_mode.py', 'NODE_CLASS.get(tn.lower())',
     'Ядро передаёт имя таблицы как в исходнике (realConsumers), а ключи '
     'отображения в нижнем регистре. Без .lower() 14 типов из 18 молча '
     'читались старыми запросами к представлениям совместимости.'),
    ('net_mode.py', 'LINE_CLASS.get(tn.lower())',
     'То же для линейных объектов.'),
    ('read_gid.py', 'net_mode.is_net(conn)',
     'Чтение схемы: при переведённой БД берутся запросы net_mode, иначе '
     'старые. Именно эта развилка даёт совместимость со старой БД.'),
    ('w_calc.py', 'def _diag_positions',
     'Позиции диагонали в CSC считаются один раз на подсеть.'),
    ('w_calc.py', 'matrix0._csc',
     'Матрица собирается один раз, на итерациях переписывается только '
     'массив data по готовым индексам. Ускорение расчёта ~10 %.'),
    ('any/insert2.py', 'self.cols = set()',
     'cols стал полем экземпляра. Как атрибут класса он был общим на все '
     '11 таблиц *_OUT, и проверка колонки в db_write пропускала колонку '
     'чужой таблицы.'),
    ('any/insert2.py', 'self.db_write(col)',
     'db_write0 вызывала db_write как свободную функцию — такой в модуле '
     'нет, любой вызов упал бы с NameError.'),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--sety', default=os.environ.get('SETY_DIR', SETY_DIR))
    ap.add_argument('--list', action='store_true', help='показать реестр')
    args = ap.parse_args()

    if args.list:
        for rel, marker, why in PATCHES:
            print('%-18s %s' % (rel, why))
        return 0

    if not os.path.isdir(args.sety):
        print('Каталог sety не найден: %s' % args.sety)
        print('Укажите --sety или переменную окружения SETY_DIR.')
        return 2

    cache = {}
    missing = []
    for rel, marker, why in PATCHES:
        path = os.path.join(args.sety, rel.replace('/', os.sep))
        if path not in cache:
            try:
                with open(path, encoding='utf-8') as f:
                    cache[path] = f.read()
            except OSError as e:
                cache[path] = None
                print('НЕТ ФАЙЛА  %-18s %s' % (rel, e))
        text = cache[path]
        if text is None:
            missing.append((rel, why))
            continue
        if marker in text:
            print('на месте   %-18s %s' % (rel, marker))
        else:
            print('ПОТЕРЯНА   %-18s %s' % (rel, marker))
            missing.append((rel, why))

    print()
    if missing:
        print('Потеряно правок: %d из %d' % (len(missing), len(PATCHES)))
        for rel, why in missing:
            print('  %s — %s' % (rel, why))
        print('\nВосстановить: net_mode.py генерируется '
              'tools/gen_sety_net_mode.py, остальное — из истории репозитория '
              'и docs/08-performance-work.md.')
        return 1

    print('Все %d правок на месте.' % len(PATCHES))
    return 0


if __name__ == '__main__':
    sys.exit(main())
