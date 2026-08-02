#include "stdafx.h"


#define NONE ""


void split_sql_expressions(CString text, std::list<CString> &list_q)
{
    CString current = "";
    CString state = NONE;

//    for c in text:
    for (int i = 0; i < text.GetLength(); i++) {
        CString c = text.Mid(i, 1);

        if (state == NONE) {  // default state, outside of special entity
            current += c;
            if (c == "'" || c == "\"") {
                // quoted string
                state = c;
            }
            else if (c == "-") {
                // probably "--" comment
                state = "-";
            }
            else if (c == "$") {
                // probably $$"
                state = "$";
            }
            else if (c == "/") {
                // probably '/*' comment
                state = "/";
            }
            else if (c == ";") {
                // remove it from the statement
//                current = current[:-1].strip();
                current = current.Left(current.GetLength()-1).Trim();
                // and save current stmt unless empty
                if (current != "") {
                    list_q.push_back(current);
                }
                current = "";
             }
        }
        else if (state == "-") {
            if (c != "-") {
                // not a comment
                state = NONE;
                current += c;
                continue;
            }
            // remove first minus
//            current = current[:-1]
              current = current.Left(current.GetLength()-1);

            // comment until end of line
            state = "--";
        }
        else if (state == "$") {
            current += c;
            if (c != "$") {
                // not a $$
                state = NONE;
            }
            else {
                state = "$$";
            }
        }
        else if (state == "--") {
            if (c == "\n") {
                // end of comment
                // and we do include this newline
                current += c;
                state = NONE;
            // else just ignore
            }
        }
        else if (state == "/") {
            if (c != "*") {
                state = NONE;
                current += c;
                continue;
            }
            // remove starting slash
            current = current.Left(current.GetLength()-1);
            // multiline comment
            state = "/*";

        }
        else if (state == "$$") {
            current += c;
            if (c == "$") {
                // probably end of $$
                state = "$$$";
            }
        }
        else if (state == "$$$") {
            current += c;

            if (c == "$") {
                state = NONE;
            }
            else {
                // not an end
                state = "$$";
            }
        }
        else if (state == "/*") {
            if (c == "*") {
                // probably end of comment
                state = "/**";
            }
        }
        else if (state == "/**") {
            if (c == "/") {
                state = NONE;
            }
            else {
                // not an end
                state = "/*";
            }
        }
        else if (state.Left(1) == "'" ||  state.Left(1) == "\"") {
            current += c;
//            if (state.endsWith("\\")) {
            if (state.Right(1) == "\\") {
                    // prev was backslash, don't check for ender
                // just revert to regular state
                state = state.Left(1);
                continue;
            }
            else if (c == "\\") {
                // don't check next char
                state += "\\";
                continue;
            }
            else if (c == state.Left(1)) {
                // end of quoted string
                state = NONE;
            }
        }
        else {
            return;
//            raise Exception("Illegal state %s" % state);
        }
    }
    if (current != "") {
//        while (state.endsWith(";")) state.chop(1);
        while (state.Right(1) == ";") state = state.Mid(state.GetLength()-1);

        if (current != "") {
            list_q.push_back(current);
        }
    }
}
