/*
 * Under MIT license
 * Author: Amir Mohamadi (@amiremohamadi)
 * DuckX is a free library to work with docx files.
 */

#ifndef DUCKX_H
#define DUCKX_H

#include <cstdio>
#include <stdlib.h>
#include <string>

#include <constants.hpp>
#include <duckxiterator.hpp>
#include <pugixml.hpp>
#include <zip.h>

// TODO: Use container-iterator design pattern!

namespace duckx {
// Run contains runs in a paragraph
class Run {
  private:
    friend class IteratorHelper;
    // Store the parent node (a paragraph)
    pugi::xml_node parent;
    // And store current node also
    pugi::xml_node current;

  public:
    Run();
    Run(pugi::xml_node, pugi::xml_node);
    void set_parent(pugi::xml_node);
    void set_current(pugi::xml_node);

    std::wstring get_text() const;
    bool set_text(const std::wstring &) const;
    bool set_text(const wchar_t *) const;

    Run &next();
    bool has_next() const;
};

// Paragraph contains a paragraph
// and stores runs
class Paragraph {
  private:
    friend class IteratorHelper;
    // Store parent node (usually the body node)
    pugi::xml_node parent;
    // And store current node also
    pugi::xml_node current;
    // A paragraph consists of runs
    Run run;

  public:
    Paragraph();
    Paragraph(pugi::xml_node, pugi::xml_node);
    void set_parent(pugi::xml_node);
    void set_current(pugi::xml_node);

    Paragraph &next();
    bool has_next() const;

    Run &runs();
    Run &add_run(const std::wstring &, duckx::formatting_flag = duckx::none);
    Run &add_run(const wchar_t *, duckx::formatting_flag = duckx::none);
    Paragraph &insert_paragraph(const std::wstring &,
                                      duckx::formatting_flag = duckx::none);
    Paragraph& insert_next_page();
    
};

// TableCell contains one or more paragraphs
class TableCell {
  private:
    friend class IteratorHelper;
    pugi::xml_node parent;
    pugi::xml_node current;

    Paragraph paragraph;

  public:
    TableCell();
    TableCell(pugi::xml_node, pugi::xml_node);

    void set_parent(pugi::xml_node);
    void set_current(pugi::xml_node);

    TableCell& set_width(float width);
    TableCell& set_merge(int widthCellByCount = 0, int verticalMerge = -1);

    Paragraph &paragraphs();

    TableCell &next();
    bool has_next() const;
};

// TableRow consists of one or more TableCells
class TableRow {
    friend class IteratorHelper;
    pugi::xml_node parent;
    pugi::xml_node current;

    TableCell cell;

  public:
    TableRow();
    TableRow(pugi::xml_node, pugi::xml_node);
    void set_parent(pugi::xml_node);
    void set_current(pugi::xml_node);
    TableCell &insert_cell(const wchar_t *,duckx::formatting_flag = duckx::none, float widthCellByCount = 0, int vericalMerge = -1);
    TableCell &cells();

    TableRow& set_child(const wchar_t* text);

    bool has_next() const;
    TableRow &next();
};

// Table consists of one or more TableRow objects
class Table {
  private:
    friend class IteratorHelper;
    pugi::xml_node parent;
    pugi::xml_node current;

    TableRow row;

  public:
    Table();
    Table(pugi::xml_node, pugi::xml_node);
    void set_parent(pugi::xml_node);
    void set_current(pugi::xml_node);
    Table &insert_table(duckx::formatting_flag f = duckx::none);
    TableRow &insert_row(int height = -1);
    Table &next();
    bool has_next() const;

    TableRow &rows();
};

// Document contains whole the docx file
// and stores paragraphs
class Document {
  private:
    friend class IteratorHelper;
    std::string directory;
    Paragraph paragraph;
    Table table;
    pugi::xml_document document;

  public:
    Document();
    Document(std::string);
    void file(std::string);
    void open();
    void save() const;
    std::string get_file() const 
        {return directory;}

    void set_size(float size);

    Paragraph &paragraphs();
    Table &tables();
};
} // namespace duckx

#endif
