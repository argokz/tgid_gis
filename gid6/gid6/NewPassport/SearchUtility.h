#pragma once
#include "stdafx.h"
#include <vector>
#include <map>
#include <algorithm>
#include <atlstr.h> // Для CString
#include "json.hpp" // Подключаем библиотеку nlohmann/json

using json = nlohmann::json;
// Класс или функциональный объект для работы с поиском
class SearchUtility {
public:
    static CString GetValuesByText(CString text, const std::vector<int>& searchFields, int countFields, std::map<long, CString>& dataStorage, std::vector<long>& dataStorageId);

    static CString GetValuesByText(CString text, std::map<long, CString>& dataStorage, std::vector<long>& dataStorageId);

    static int ExtractFieldIndex(const CString& fieldName);

private:
    static std::vector<CString> ExtractFields(const CString& jsonString, int countFields);
    static CString RemoveTrailingComma(const CString& jsonString);
};

