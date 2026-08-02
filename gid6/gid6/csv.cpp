#include "stdafx.h"
#include "csv.h"

enum class CSVState {
    UnquotedField,
    QuotedField,
    QuotedQuote
};

//vector<CString> csv_read(CString s)


std::vector<CString> csv_read(const CString & row)
{
    CSVState state = CSVState::UnquotedField;
    std::vector<CString> fields{ "" };
    std::vector<bool> is_q{ false };

    size_t i = 0; // index of the current field
//    for (char c : row) {
    for (int k = 0; k < row.GetLength(); k++) {
        char c = row[k];
        switch (state) {
        case CSVState::UnquotedField:
            switch (c) {
            case ',': // end of field
                fields.push_back(""); 
                is_q.push_back(false); 
                i++;
                break;
            case '"': state = CSVState::QuotedField;
                is_q[i] = true;
                break;
            default:  fields[i] += c;
                break;
            }
            break;
        case CSVState::QuotedField:
            switch (c) {
            case '"': state = CSVState::QuotedQuote;
                break;
            default:  fields[i] += c;
                break;
            }
            break;
        case CSVState::QuotedQuote:
            switch (c) {
            case ',': // , after closing quote
                fields.push_back(""); 
                is_q.push_back(false);
                i++;
                state = CSVState::UnquotedField;
                break;
            case '"': // "" -> "
                fields[i] += '"';
                state = CSVState::QuotedField;
                break;
            default:  // end of quote
                state = CSVState::UnquotedField;
                break;
            }
            break;
        }
    }

    for (int k = 0; k < fields.size(); k++) {
        if (!is_q[k] && fields[k] == "") fields[k] = NULL_VALUE;
    }

    return fields;
}


std::vector<CString> csv_read(FILE *f)
{
    CSVState state = CSVState::UnquotedField;
    std::vector<CString> fields{ "" };
    std::vector<bool> is_q{ false };


    size_t i = 0; // index of the current field

    while (1) {
        char c = getc(f);

        if (feof(f)) break;

        switch (state) {
        case CSVState::UnquotedField:
            switch (c) {
            case ',': // end of field
                fields.push_back(""); 
                is_q.push_back(false); 
                i++;
                break;
            case '"': state = CSVState::QuotedField;
                is_q[i] = true;
                break;
            default:  fields[i] += c;
                break;
            }
            break;
        case CSVState::QuotedField:
            switch (c) {
            case '"': state = CSVState::QuotedQuote;
                break;
            default:  fields[i] += c;
                break;
            }
            break;
        case CSVState::QuotedQuote:
            switch (c) {
            case ',': // , after closing quote
                fields.push_back(""); 
                is_q.push_back(false);
                i++;
                state = CSVState::UnquotedField;
                break;
            case '"': // "" -> "
                fields[i] += '"';
                state = CSVState::QuotedField;
                break;
            default:  // end of quote
                state = CSVState::UnquotedField;
                break;
            }
            break;
        }
    }

    for (int k = 0; k < fields.size(); k++) {
        if (!is_q[k] && fields[k] == "") fields[k] = NULL_VALUE;
    }

    return fields;
}
