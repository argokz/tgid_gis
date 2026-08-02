#pragma once
#include "stdafx.h"
#include "SearchUtility.h"

string UTF8ToANSI(string s);

CString SearchUtility::GetValuesByText(CString text, const std::vector<int>& searchFields, int countFields, std::map<long, CString>& dataStorage, std::vector<long>& dataStorageId) {
    struct SearchResult {
        CString row;
        int score;
        bool operator<(const SearchResult& other) const {
            return score > other.score; // Для сортировки по убыванию
        }
    };

    std::vector<SearchResult> results;
    text.MakeLower();
    int fullMatchScore = searchFields.size() * 4;
    int partlyMatchScore = searchFields.size() * 2;
    CString text2;

    for (auto id : dataStorageId) {
        CString row = dataStorage[id];
        CString searchRow = row;
        searchRow.MakeLower();

        // Подсчет количества совпадений
        int score = 0;
        if (searchRow.Find(text) != -1) {
            // Разбиваем строку на поля
            
            std::vector<CString> fields = ExtractFields(row, countFields);
            
            /*
            std::vector<CString> fields;
            int start = 0, end = 0;
            while ((end = row.Find(',', start)) != -1) {
                fields.push_back(row.Mid(start, end - start));
                start = end + 1;
            }
            fields.push_back(row.Mid(start));*/

            // Оцениваем совпадения только в указанных полях
            bool found = false;
            for (int i = 0; i < fields.size(); ++i) {
                if (find(searchFields.begin(), searchFields.end(), i) != searchFields.end()) {
                    CString lowerField = fields[i];
                    int indexSearch = std::distance(searchFields.begin(), find(searchFields.begin(), searchFields.end(), i));
                    lowerField.MakeLower();
                    //text2.Format("\"col_%d\" : \"%s\"", i, text);
                    if (lowerField == text) {
                        score += fullMatchScore - indexSearch; // Полное совпадение
                        found = true;
                    }
                    else if (lowerField.Find(text) != -1) {
                        score += partlyMatchScore - indexSearch; // Частичное совпадение
                        found = true;
                    }
                }
            }

            if (found) {
                results.push_back({ row, score });
            }
        }
    }

    // Сортировка по степени соответствия
    std::sort(results.begin(), results.end());

    // Возвращаем первые 500 результатов
    CString query;
    int count = 0;
    for (const auto& result : results) {
        if (count == 500) break;
        query += result.row + _T("\n"); // Добавляем новую строку для отделения результатов
        count++;
    }

    return query;
}


CString SearchUtility::GetValuesByText(CString text, std::map<long, CString>& dataStorage, std::vector<long>& dataStorageId) {
    int c1 = 0;
    int count = 0;
    CString query;
    while (c1 != dataStorageId.size() && count != 500) {
        CString row;
        row = dataStorage[dataStorageId[c1]];
        CString searchRow = row;
        searchRow.MakeLower();
        text.MakeLower();
        if (searchRow.Find(text) != -1) {
            query += row;
            count++;
        }

        c1++;
    }
    return query;
}

int SearchUtility::ExtractFieldIndex(const CString& fieldName) {
    CString prefix = _T("col_");
    int prefixLength = prefix.GetLength();
    CString indexStr = fieldName.Mid(prefixLength);
    return _ttoi(indexStr);
}


std::vector<CString> SearchUtility::ExtractFields(const CString& jsonString, int countFields) {
    std::vector<CString> fields;
    CString cleanedJsonString = RemoveTrailingComma(jsonString);
    _bstr_t bstr(cleanedJsonString);
    std::wstring jsonStr(bstr, SysStringLen(bstr));
    json j = json::parse(jsonStr);

    for (long i = 0; i < countFields; i++) {
        string title = "col_" + std::to_string(i);
        if (j.contains(title) > 0) {
            auto j1 = j[title];
            if (j1.is_string())
                fields.push_back(CString(UTF8ToANSI(j1.get<std::string>().c_str()).c_str()));
            else if (j1.is_array()) {
                CString arrayString;
                bool firstElement = true;
                for (auto& element : j1) {
                    if (!firstElement) {
                        arrayString += _T(",");
                    }
                    else {
                        firstElement = false;
                    }
                    if (element.is_string()) {
                        arrayString += CString(UTF8ToANSI(element.get<std::string>().c_str()).c_str());
                    }
                    else {
                        arrayString += CString(UTF8ToANSI(element.dump().c_str()).c_str());
                    }
                }
                fields.push_back(arrayString);
            }
            else
            fields.push_back(CString(UTF8ToANSI(j1.dump().c_str()).c_str()));
           
        }
    }
   

    return fields;
}

CString SearchUtility::RemoveTrailingComma(const CString& jsonString) {
    CString result = jsonString;
    int length = result.GetLength();

    // Удаление запятой в конце строки, если она есть
    if (length > 0 && result[length - 1] == ',') {
        result = result.Left(length - 1);
    }

    return result;
}
