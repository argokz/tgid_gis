"""Инвентаризация функций и SQL-наследия старого ТГИД.

Скрипт не решает, какие функции нужны бизнесу. Он создаёт воспроизводимую
техническую основу для такого решения:

* находит QAction старого Qt-клиента и их обработчики;
* считает места обращения C++ к БД;
* собирает строковые ссылки на внешние SQL-файлы;
* группирует физические SQL-файлы по содержимому;
* извлекает приблизительные зависимости от таблиц;
* формирует JSON и Markdown без абсолютных путей разработчика.

Пример из корня репозитория:

    python tools/inventory_legacy.py
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


REPO_ROOT = Path(__file__).resolve().parents[1]
WORKSPACE_ROOT = REPO_ROOT.parent

CATEGORY_TITLES = {
    "network_edit": "Редактирование сети",
    "map_navigation": "Карта и навигация",
    "calculations": "Гидравлические расчёты",
    "queries_reports": "Запросы и отчёты",
    "technical_conditions": "Технические условия и нагрузки",
    "maintenance": "Ремонты, дефекты и опрессовки",
    "corrosion": "Коррозия и шурфы",
    "passport": "Паспорта",
    "heat_losses": "Тепловые потери",
    "gis_exchange": "GIS и обмен данными",
    "administration": "Администрирование",
    "reference_data": "Справочники",
    "other": "Прочее / требует классификации",
}

ACTION_DECL_RE = re.compile(
    r"^\s*QAction\s*\*\s*(a[A-Za-z0-9_]+)\s*;"
    r"(?:\s*//\s*(.*?))?\s*$",
    re.MULTILINE,
)
ACTION_ASSIGN_RE = re.compile(
    r"gidrAction\.(a[A-Za-z0-9_]+)\s*=",
)
ACTION_TRIGGER_RE = re.compile(
    r"gidrAction\.(a[A-Za-z0-9_]+)\s*=\s*addTrigger\s*\("
    r".*?tr\(\"((?:\\.|[^\"\\])*)\"\).*?"
    r"SLOT\((\w+)\(\)\)",
    re.DOTALL,
)
ACTION_USE_RE = re.compile(r"gidrAction\.(a[A-Za-z0-9_]+)")
ACTION_MENU_RE = re.compile(
    r"(?:addAction|createButton|set_menu)\s*\([^;\n]*?"
    r"gidrAction\.(a[A-Za-z0-9_]+)",
)
SQL_LITERAL_RE = re.compile(r"""["']([^"']+\.sql)["']""", re.IGNORECASE)
DB_ACCESS_RE = re.compile(
    r"\bQSql(?:Query|Database|TableModel|QueryModel)\b"
    r"|\bquery_exec\b|\breadQ\b|\breadFile\b"
    r"|\b(?:select|insert|update|delete)\b",
    re.IGNORECASE,
)
SQL_OPERATION_RE = re.compile(
    r"\b(select|insert|update|delete|create|alter|drop|truncate)\b",
    re.IGNORECASE,
)
TABLE_REFERENCE_RE = re.compile(
    r"\b(?:from|join|update|into|delete\s+from|truncate\s+table)\s+"
    r"((?:\"[^\"]+\"|\[[^\]]+\]|[A-Za-z_][A-Za-z0-9_$]*)"
    r"(?:\s*\.\s*(?:\"[^\"]+\"|\[[^\]]+\]|[A-Za-z_][A-Za-z0-9_$]*))?)",
    re.IGNORECASE,
)


@dataclass(frozen=True)
class SqlFile:
    path: Path
    root_name: str
    relative_path: str
    content_hash: str
    size: int
    operations: tuple[str, ...]
    tables: tuple[str, ...]
    category: str


def read_text(path: Path) -> str:
    data = path.read_bytes()
    for encoding in ("utf-8-sig", "cp1251", "latin1"):
        try:
            return data.decode(encoding)
        except UnicodeDecodeError:
            continue
    return data.decode("utf-8", errors="replace")


def repair_mojibake(value: str) -> str:
    """Выбирает наиболее похожий на русский вариант испорченной строки."""

    value = value.strip()
    candidates = {value}
    for source, target in (("latin1", "utf-8"), ("latin1", "cp1251")):
        try:
            candidates.add(value.encode(source).decode(target))
        except (UnicodeEncodeError, UnicodeDecodeError):
            pass

    def score(text: str) -> int:
        cyrillic = sum("А" <= char <= "я" or char in "Ёё" for char in text)
        mojibake = sum(text.count(token) for token in ("Ð", "Ñ", "Р", "С", "Ç"))
        replacement = text.count("\ufffd")
        return cyrillic * 4 - mojibake * 3 - replacement * 20

    return max(candidates, key=lambda candidate: (score(candidate), candidate))


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(WORKSPACE_ROOT.resolve()).as_posix()
    except ValueError:
        return resolved.as_posix()


def decode_cpp_string(value: str) -> str:
    try:
        value = bytes(value, "utf-8").decode("unicode_escape")
    except UnicodeDecodeError:
        pass
    return repair_mojibake(value)


def classify_action(name: str, label: str, comment: str) -> str:
    text = f"{name} {label} {comment}".lower()
    rules = (
        ("corrosion", r"korroz|corros|корроз|shurf|шурф"),
        (
            "maintenance",
            r"remont|repair|defect|opres|diag|osmotr|iznos|"
            r"ремонт|дефект|опресс|диагност|осмотр|износ",
        ),
        (
            "technical_conditions",
            r"(?:^|[^a-z])tu(?:[^a-z]|$)|nagr|dogovor|"
            r"техническ|нагруз|договор",
        ),
        ("passport", r"passport|pasport|pasp|паспорт"),
        (
            "heat_losses",
            r"teplopoter|poteri|heatloss|теплопотер|тепловые потери",
        ),
        (
            "calculations",
            r"doit|calculation|raschet|rasch|pjezo|gidrav|"
            r"расч[её]т|пьез|гидравл",
        ),
        (
            "gis_exchange",
            r"export|import|shape|dxf|geo|osm|bmp|png|"
            r"экспорт|импорт|геобаз|подлож",
        ),
        (
            "administration",
            r"password|right|role|user|update|setup|catalog|"
            r"парол|прав|пользов|обновлен|настрой",
        ),
        (
            "queries_reports",
            r"zap|query|report|out|excel|word|print|table|prot|"
            r"запрос|отч[её]т|таблиц|печать",
        ),
        (
            "reference_data",
            r"sprav|standard|organization|справоч|стандарт|организац",
        ),
        (
            "network_edit",
            r"node|line|fragment|move|swap|rename|delete|del|"
            r"openclose|setcoord|узел|лини|фрагмент|перенести|"
            r"удалить|направлен",
        ),
        (
            "map_navigation",
            r"zoom|masall|masshtab|navigator|find|color|font|"
            r"масштаб|навига|поиск|цвет|шрифт",
        ),
    )
    for category, pattern in rules:
        if re.search(pattern, text, re.IGNORECASE):
            return category
    return "other"


def classify_query(path: str, tables: Iterable[str]) -> str:
    text = f"{path} {' '.join(tables)}".lower()
    rules = (
        ("corrosion", r"korroz|corros|indicator|shurf|корроз|шурф"),
        (
            "maintenance",
            r"remont|repair|defect|opres|diag|osmotr|iznos|"
            r"ремонт|дефект|опресс|диагност|осмотр|износ",
        ),
        (
            "technical_conditions",
            r"(?:^|[/_.-])tu(?:[/_.-]|$)|nagr|dogovor|"
            r"техническ|нагруз|договор",
        ),
        ("passport", r"passport|pasport|certification|паспорт"),
        (
            "heat_losses",
            r"poter|heat.?loss|teplo_out|teplo_t_out|"
            r"теплопотер|потери|температур",
        ),
        (
            "calculations",
            r"calculation|_out|out_|pjezo|gidrav|raschet|"
            r"расч[её]т|пьез|гидравл",
        ),
        (
            "gis_exchange",
            r"shape|dxf|geo|osm|map|qgis|geoserver|"
            r"экспорт|импорт|геобаз",
        ),
        (
            "administration",
            r"user|right|role|password|update_index|"
            r"пользов|прав|парол",
        ),
        (
            "reference_data",
            r"sprav|standard|organization|nodetypes|tubingtypes|"
            r"справоч|стандарт|организац",
        ),
        (
            "network_edit",
            r"(?:^|[/_.-])(us|ut|nodes?|linesobj|fragment)"
            r"(?:[/_.-]|$)|heatpipesection|realconsumer",
        ),
        (
            "queries_reports",
            r"report|excel|word|docx|journal|itog|zapros|"
            r"отч[её]т|журнал|итог",
        ),
    )
    for category, pattern in rules:
        if re.search(pattern, text, re.IGNORECASE):
            return category
    return "other"


def normalized_sql_hash(text: str) -> str:
    normalized = "\n".join(line.rstrip() for line in text.splitlines()).strip()
    return hashlib.sha256(normalized.encode("utf-8")).hexdigest()


def normalize_identifier(identifier: str) -> str:
    parts = []
    for part in re.split(r"\s*\.\s*", identifier):
        part = part.strip().strip('"[]')
        if part:
            parts.append(part.lower())
    return ".".join(parts)


def sql_metadata(text: str) -> tuple[tuple[str, ...], tuple[str, ...]]:
    without_comments = re.sub(r"--.*?$", " ", text, flags=re.MULTILINE)
    without_comments = re.sub(r"/\*.*?\*/", " ", without_comments, flags=re.DOTALL)
    operations = tuple(sorted({m.lower() for m in SQL_OPERATION_RE.findall(without_comments)}))
    ignored = {
        "select",
        "values",
        "set",
        "where",
        "generate_series",
        "unnest",
        "jsonb_each",
        "jsonb_each_text",
    }
    tables = {
        normalize_identifier(match)
        for match in TABLE_REFERENCE_RE.findall(without_comments)
    }
    tables = {
        table
        for table in tables
        if table and table.split(".")[-1] not in ignored and "$" not in table
    }
    return operations, tuple(sorted(tables))


def iter_cpp_files(root: Path) -> list[Path]:
    return sorted(
        path
        for path in root.rglob("*")
        if path.is_file() and path.suffix.lower() in {".cpp", ".h", ".hpp"}
    )


def collect_actions(cpp_root: Path, cpp_texts: dict[Path, str]) -> list[dict]:
    header = cpp_root / "gidrAction1.h"
    declarations = {
        match.group(1): repair_mojibake(match.group(2) or "")
        for match in ACTION_DECL_RE.finditer(read_text(header))
    }

    joined = "\n".join(cpp_texts.values())
    assigned = set(ACTION_ASSIGN_RE.findall(joined))
    menu_actions = set(ACTION_MENU_RE.findall(joined))
    usage_counts = Counter(ACTION_USE_RE.findall(joined))
    trigger_data = {}
    for match in ACTION_TRIGGER_RE.finditer(joined):
        trigger_data[match.group(1)] = {
            "label": decode_cpp_string(match.group(2)),
            "handler": match.group(3),
        }

    actions = []
    for name in sorted(declarations):
        trigger = trigger_data.get(name, {})
        comment = declarations[name]
        label = trigger.get("label") or comment
        actions.append(
            {
                "name": name,
                "label": label,
                "comment": comment,
                "category": classify_action(name, label, comment),
                "initialized": name in assigned,
                "menu_or_toolbar": name in menu_actions,
                "handler": trigger.get("handler", ""),
                "source_references": usage_counts[name],
                "review_status": "unreviewed",
            }
        )
    return actions


def collect_sql_files(sql_roots: list[tuple[str, Path]]) -> list[SqlFile]:
    result = []
    seen_paths = set()
    for root_name, root in sql_roots:
        for path in sorted(root.rglob("*.sql")):
            resolved = path.resolve()
            if resolved in seen_paths or not path.is_file():
                continue
            seen_paths.add(resolved)
            text = read_text(path)
            operations, tables = sql_metadata(text)
            relative = path.relative_to(root).as_posix()
            result.append(
                SqlFile(
                    path=path,
                    root_name=root_name,
                    relative_path=relative,
                    content_hash=normalized_sql_hash(text),
                    size=path.stat().st_size,
                    operations=operations,
                    tables=tables,
                    category=classify_query(relative, tables),
                )
            )
    return result


def match_reference(reference: str, sql_files: list[SqlFile]) -> list[SqlFile]:
    normalized = reference.replace("\\", "/").lstrip("/").lower()
    basename = Path(normalized).name
    dynamic = any(token in normalized for token in ("%1", "%s", "{", "$"))
    matches = [
        item
        for item in sql_files
        if item.relative_path.lower().endswith(normalized)
    ]
    if not matches and not dynamic:
        matches = [
            item
            for item in sql_files
            if Path(item.relative_path).name.lower() == basename
        ]
    return matches


def build_inventory(
    cpp_root: Path,
    sql_roots: list[tuple[str, Path]],
) -> dict:
    cpp_files = iter_cpp_files(cpp_root)
    cpp_texts = {path: read_text(path) for path in cpp_files}
    actions = collect_actions(cpp_root, cpp_texts)
    sql_files = collect_sql_files(sql_roots)

    static_references = defaultdict(lambda: {"files": set(), "occurrences": 0})
    db_files = []
    sql_keyword_occurrences = 0
    for path, text in cpp_texts.items():
        if DB_ACCESS_RE.search(text):
            db_files.append(display_path(path))
        sql_keyword_occurrences += len(
            re.findall(r"\b(?:select|insert|update|delete)\b", text, re.IGNORECASE)
        )
        for match in SQL_LITERAL_RE.finditer(text):
            reference = match.group(1).replace("\\", "/")
            static_references[reference]["files"].add(display_path(path))
            static_references[reference]["occurrences"] += 1

    reference_rows = []
    referenced_hashes = set()
    for reference in sorted(static_references, key=str.lower):
        matches = match_reference(reference, sql_files)
        referenced_hashes.update(item.content_hash for item in matches)
        row = static_references[reference]
        reference_rows.append(
            {
                "reference": reference,
                "dynamic_template": any(
                    token in reference for token in ("%1", "%s", "{", "$")
                ),
                "occurrences": row["occurrences"],
                "cpp_files": sorted(row["files"]),
                "matched_query_ids": sorted(
                    {f"q_{item.content_hash[:12]}" for item in matches}
                ),
                "matched_paths": sorted(
                    {display_path(item.path) for item in matches}
                ),
            }
        )

    groups = defaultdict(list)
    for item in sql_files:
        groups[item.content_hash].append(item)

    query_rows = []
    table_counter = Counter()
    category_counter = Counter()
    for content_hash, copies in sorted(
        groups.items(),
        key=lambda item: (
            classify_query(item[1][0].relative_path, item[1][0].tables),
            Path(item[1][0].relative_path).name.lower(),
            item[0],
        ),
    ):
        all_tables = sorted({table for copy in copies for table in copy.tables})
        for table in all_tables:
            table_counter[table] += 1
        category = Counter(copy.category for copy in copies).most_common(1)[0][0]
        category_counter[category] += 1
        query_rows.append(
            {
                "id": f"q_{content_hash[:12]}",
                "category": category,
                "basename": min(
                    (Path(copy.relative_path).name for copy in copies),
                    key=lambda value: (len(value), value.lower()),
                ),
                "sha256": content_hash,
                "physical_copies": len(copies),
                "paths": sorted(display_path(copy.path) for copy in copies),
                "operations": sorted(
                    {operation for copy in copies for operation in copy.operations}
                ),
                "tables": all_tables,
                "referenced_from_cpp": content_hash in referenced_hashes,
                "review_status": "unreviewed",
            }
        )

    action_categories = Counter(action["category"] for action in actions)
    root_counts = Counter(item.root_name for item in sql_files)
    root_unique = {
        root_name: len(
            {
                item.content_hash
                for item in sql_files
                if item.root_name == root_name
            }
        )
        for root_name, _ in sql_roots
    }

    return {
        "generator": "tools/inventory_legacy.py",
        "scope": {
            "cpp_root": display_path(cpp_root),
            "sql_roots": [
                {"name": name, "path": display_path(path)}
                for name, path in sql_roots
            ],
        },
        "summary": {
            "cpp_files": len(cpp_files),
            "cpp_lines": sum(text.count("\n") + 1 for text in cpp_texts.values()),
            "db_access_cpp_files": len(db_files),
            "inline_sql_keyword_occurrences": sql_keyword_occurrences,
            "actions_declared": len(actions),
            "actions_initialized": sum(action["initialized"] for action in actions),
            "actions_in_menu_or_toolbar": sum(
                action["menu_or_toolbar"] for action in actions
            ),
            "static_sql_references": len(reference_rows),
            "dynamic_sql_reference_templates": sum(
                row["dynamic_template"] for row in reference_rows
            ),
            "physical_sql_files": len(sql_files),
            "unique_sql_contents": len(query_rows),
            "duplicate_sql_copies": len(sql_files) - len(query_rows),
            "cpp_referenced_unique_queries": sum(
                row["referenced_from_cpp"] for row in query_rows
            ),
        },
        "action_categories": dict(sorted(action_categories.items())),
        "query_categories": dict(sorted(category_counter.items())),
        "sql_roots": [
            {
                "name": name,
                "path": display_path(path),
                "physical_files": root_counts[name],
                "unique_contents_within_root": root_unique[name],
            }
            for name, path in sql_roots
        ],
        "db_access_cpp_files": sorted(db_files),
        "actions": actions,
        "static_sql_references": reference_rows,
        "table_dependencies": [
            {"table": table, "unique_queries": count}
            for table, count in table_counter.most_common()
        ],
        "queries": query_rows,
    }


def markdown_table(headers: list[str], rows: Iterable[Iterable[object]]) -> str:
    result = [
        "| " + " | ".join(headers) + " |",
        "|" + "|".join("---" for _ in headers) + "|",
    ]
    result.extend(
        "| " + " | ".join(str(value).replace("|", "\\|") for value in row) + " |"
        for row in rows
    )
    return "\n".join(result)


def render_markdown(inventory: dict) -> str:
    summary = inventory["summary"]
    actions = inventory["actions"]
    missing_actions = [
        action for action in actions if not action["initialized"]
    ]
    unmatched_refs = [
        row
        for row in inventory["static_sql_references"]
        if not row["matched_query_ids"]
    ]

    sections = [
        "# Инвентаризация функций и SQL старого ТГИД",
        "",
        "Файл сгенерирован командой `python tools/inventory_legacy.py`.",
        "Классификация автоматическая и не заменяет решения заказчика о том,",
        "какие функции должны войти в новый продукт.",
        "",
        "## Масштаб",
        "",
        markdown_table(
            ["Показатель", "Количество"],
            [
                ("C++/header-файлы в обследованном дереве", summary["cpp_files"]),
                ("Строки C++/header", summary["cpp_lines"]),
                ("C++-файлы с признаками доступа к БД", summary["db_access_cpp_files"]),
                ("Объявленные QAction", summary["actions_declared"]),
                ("Инициализированные QAction", summary["actions_initialized"]),
                (
                    "QAction, найденные в меню/панелях",
                    summary["actions_in_menu_or_toolbar"],
                ),
                ("Статические ссылки C++ на *.sql", summary["static_sql_references"]),
                (
                    "Динамические шаблоны путей SQL",
                    summary["dynamic_sql_reference_templates"],
                ),
                ("Физические SQL-файлы", summary["physical_sql_files"]),
                ("Уникальные SQL по содержимому", summary["unique_sql_contents"]),
                ("Копии одинакового SQL", summary["duplicate_sql_copies"]),
                (
                    "Уникальные SQL, сопоставленные со ссылками C++",
                    summary["cpp_referenced_unique_queries"],
                ),
            ],
        ),
        "",
        "## Команды по функциональным блокам",
        "",
        markdown_table(
            ["Блок", "Объявлено", "Инициализировано", "В меню/панели"],
            (
                (
                    CATEGORY_TITLES.get(category, category),
                    len([a for a in actions if a["category"] == category]),
                    len(
                        [
                            a
                            for a in actions
                            if a["category"] == category and a["initialized"]
                        ]
                    ),
                    len(
                        [
                            a
                            for a in actions
                            if a["category"] == category
                            and a["menu_or_toolbar"]
                        ]
                    ),
                )
                for category in CATEGORY_TITLES
                if any(a["category"] == category for a in actions)
            ),
        ),
        "",
        "## Источники SQL",
        "",
        markdown_table(
            ["Источник", "Физические файлы", "Уникальные внутри источника"],
            (
                (
                    f"`{row['path']}`",
                    row["physical_files"],
                    row["unique_contents_within_root"],
                )
                for row in inventory["sql_roots"]
            ),
        ),
        "",
        "Одинаковые запросы объединены по нормализованному SHA-256. Полный",
        "реестр физических путей, операций и зависимостей находится в",
        "`docs/schema/legacy_inventory.json`.",
        "",
        "## Наиболее частые зависимости SQL",
        "",
        markdown_table(
            ["Таблица/отношение", "Уникальных запросов"],
            (
                (f"`{row['table']}`", row["unique_queries"])
                for row in inventory["table_dependencies"][:40]
            ),
        ),
        "",
        "Извлечение таблиц основано на синтаксических шаблонах `FROM/JOIN/UPDATE`",
        "и является отправной точкой. Динамический SQL нужно подтвердить вручную.",
        "",
        "## Несопоставленные ссылки C++ на SQL",
        "",
    ]

    if unmatched_refs:
        sections.append(
            markdown_table(
                ["Ссылка", "Вхождений", "Динамическая"],
                (
                    (
                        f"`{row['reference']}`",
                        row["occurrences"],
                        "да" if row["dynamic_template"] else "нет",
                    )
                    for row in unmatched_refs
                ),
            )
        )
    else:
        sections.append("Все статические ссылки сопоставлены.")

    sections.extend(
        [
            "",
            "## Объявленные, но не инициализированные команды",
            "",
        ]
    )
    if missing_actions:
        sections.append(
            markdown_table(
                ["Команда", "Подпись/комментарий", "Категория"],
                (
                    (
                        f"`{action['name']}`",
                        action["label"] or "—",
                        CATEGORY_TITLES.get(action["category"], action["category"]),
                    )
                    for action in missing_actions
                ),
            )
        )
    else:
        sections.append("Таких команд не найдено.")

    sections.extend(
        [
            "",
            "## Как использовать реестр",
            "",
            "1. Для каждой команды установить бизнес-статус: `required`,",
            "   `optional`, `customer-specific`, `obsolete`.",
            "2. Связать обязательные команды с SQL из JSON и целевой схемой",
            "   `net`, `ref`, `calc`, `maintenance`, `tu` или `report`.",
            "3. Для каждого переносимого запроса зафиксировать параметры, колонки",
            "   результата и эталонное сравнение со старой БД.",
            "4. Переносить не файлы по одному, а законченные пользовательские",
            "   сценарии: команда → запрос → таблица/карта/отчёт.",
            "",
            "## Следующий технический этап",
            "",
            "Сначала закрывается основной сценарий редактора сети: полноценная",
            "топология, специальные карточки и справочники. Параллельно из этого",
            "реестра выделяется минимальный набор расчётных и эксплуатационных",
            "запросов для схемы `calc` и первого модуля отчётов.",
            "",
        ]
    )
    return "\n".join(sections)


def existing_directory(value: str) -> Path:
    path = Path(value).resolve()
    if not path.is_dir():
        raise argparse.ArgumentTypeError(f"Каталог не найден: {path}")
    return path


def parse_sql_root(value: str) -> tuple[str, Path]:
    if "=" not in value:
        raise argparse.ArgumentTypeError(
            "SQL-корень задаётся в формате name=path"
        )
    name, path_value = value.split("=", 1)
    if not name.strip():
        raise argparse.ArgumentTypeError("Пустое имя SQL-корня")
    return name.strip(), existing_directory(path_value)


def main() -> None:
    default_cpp = WORKSPACE_ROOT / "gid8" / "gid8"
    default_sql_roots = [
        ("gid8", WORKSPACE_ROOT / "gid8"),
        ("gid6_postgresql", WORKSPACE_ROOT / "gid6" / "gidr" / "postgresql"),
        ("gid6_sql", WORKSPACE_ROOT / "gid6" / "gidr" / "sql"),
    ]

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--cpp-root",
        type=existing_directory,
        default=default_cpp,
        help="Корень исходников старого Qt-приложения",
    )
    parser.add_argument(
        "--sql-root",
        action="append",
        type=parse_sql_root,
        dest="sql_roots",
        help="Источник SQL в формате name=path; можно повторять",
    )
    parser.add_argument(
        "--out-json",
        type=Path,
        default=REPO_ROOT / "docs" / "schema" / "legacy_inventory.json",
    )
    parser.add_argument(
        "--out-md",
        type=Path,
        default=REPO_ROOT / "docs" / "12-legacy-function-inventory.md",
    )
    args = parser.parse_args()

    sql_roots = args.sql_roots or [
        (name, path.resolve())
        for name, path in default_sql_roots
        if path.is_dir()
    ]
    if not sql_roots:
        parser.error("Не найдено ни одного каталога SQL")

    inventory = build_inventory(args.cpp_root, sql_roots)
    args.out_json.parent.mkdir(parents=True, exist_ok=True)
    args.out_md.parent.mkdir(parents=True, exist_ok=True)
    args.out_json.write_text(
        json.dumps(inventory, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )
    args.out_md.write_text(render_markdown(inventory), encoding="utf-8")

    summary = inventory["summary"]
    print(
        "actions={actions_declared} initialized={actions_initialized} "
        "sql_files={physical_sql_files} unique_sql={unique_sql_contents} "
        "static_refs={static_sql_references}".format(**summary)
    )
    print(display_path(args.out_json.resolve()))
    print(display_path(args.out_md.resolve()))


if __name__ == "__main__":
    main()
