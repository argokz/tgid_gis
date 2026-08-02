#include "stdafx.h"
#include "duckx.hpp"
#include <cctype>

// Hack on pugixml
// We need to write xml to std string (or char *)
// So overload the write function
struct xml_string_writer : pugi::xml_writer {
	std::string result;

	virtual void write(const void* data, size_t size) {
		result.append(static_cast<const char*>(data), size);
	}
};

duckx::Run::Run() {}

duckx::Run::Run(pugi::xml_node parent, pugi::xml_node current) {
	this->set_parent(parent);
	this->set_current(current);
}

void duckx::Run::set_parent(pugi::xml_node node) {
	this->parent = node;
	this->current = this->parent.child(L"w:r");
}

void duckx::Run::set_current(pugi::xml_node node) { this->current = node; }

std::wstring duckx::Run::get_text() const {
	return this->current.child(L"w:t").text().get();
}

bool duckx::Run::set_text(const std::wstring& text) const {
	return this->current.child(L"w:t").text().set(text.c_str());
}

bool duckx::Run::set_text(const wchar_t* text) const {
	return this->current.child(L"w:t").text().set(text);
}

duckx::Run& duckx::Run::next() {
	this->current = this->current.next_sibling();
	return *this;
}

bool duckx::Run::has_next() const { return this->current != 0; }

// Table cells
duckx::TableCell::TableCell() {}

duckx::TableCell::TableCell(pugi::xml_node parent, pugi::xml_node current) {
	this->set_parent(parent);
	this->set_current(current);
}

void duckx::TableCell::set_parent(pugi::xml_node node) {
	this->parent = node;
	this->current = this->parent.child(L"w:tc");

	this->paragraph.set_parent(this->current);
}

void duckx::TableCell::set_current(pugi::xml_node node) {
	this->current = node;
}

bool duckx::TableCell::has_next() const { return this->current != 0; }

duckx::TableCell& duckx::TableCell::next() {
	this->current = this->current.next_sibling();
	return *this;
}

duckx::Paragraph& duckx::TableCell::paragraphs() {
	this->paragraph.set_parent(this->current);
	return this->paragraph;
}

// Table rows
duckx::TableRow::TableRow() {}

duckx::TableRow::TableRow(pugi::xml_node parent, pugi::xml_node current) {
	this->set_parent(parent);
	this->set_current(current);
}

void duckx::TableRow::set_parent(pugi::xml_node node) {
	this->parent = node;
	this->current = this->parent.child(L"w:tr");

	this->cell.set_parent(this->current);
}

void duckx::TableRow::set_current(pugi::xml_node node) { this->current = node; }

duckx::TableRow& duckx::TableRow::next() {
	this->current = this->current.next_sibling();
	return *this;
}

duckx::TableCell& duckx::TableRow::cells() {
	this->cell.set_parent(this->current);
	return this->cell;
}

bool duckx::TableRow::has_next() const { return this->current != 0; }

// Tables
duckx::Table::Table() {}

duckx::Table::Table(pugi::xml_node parent, pugi::xml_node current) {
	this->set_parent(parent);
	this->set_current(current);
}

void duckx::Table::set_parent(pugi::xml_node node) {
	this->parent = node;
	this->current = this->parent.child(L"w:tb");

	this->row.set_parent(this->current);
}

bool duckx::Table::has_next() const { return this->current != 0; }

duckx::Table& duckx::Table::next() {
	this->current = this->current.next_sibling();
	this->row.set_parent(this->current);
	return *this;
}

void duckx::Table::set_current(pugi::xml_node node) { this->current = node; }

duckx::TableRow& duckx::Table::rows() {
	this->row.set_parent(this->current);
	return this->row;
}

void createEmptyBorder(pugi::xml_node tblBorders, const wchar_t* tagBorder) {
	pugi::xml_node myBorder = tblBorders.append_child(tagBorder);
	myBorder.append_attribute(L"w:val").set_value(L"none");
	myBorder.append_attribute(L"w:sz").set_value(L"0");
	myBorder.append_attribute(L"w:space").set_value(L"0");
	myBorder.append_attribute(L"w:color").set_value(L"auto");
}

void createBorder(pugi::xml_node tblBorders, const wchar_t* tagBorder) {
	pugi::xml_node myBorder = tblBorders.append_child(tagBorder);
	myBorder.append_attribute(L"w:val").set_value(L"single");
	myBorder.append_attribute(L"w:sz").set_value(L"4");
	myBorder.append_attribute(L"w:space").set_value(L"0");
	myBorder.append_attribute(L"w:color").set_value(L"auto");
}



duckx::Table&
duckx::Table::insert_table(duckx::formatting_flag f) {
	pugi::xml_node new_table =
		this->parent.append_child(L"w:tbl");
	pugi::xml_node tblPr = new_table.append_child(L"w:tblPr");

	if (f & duckx::align_center) {
		pugi::xml_node my_node = tblPr.append_child(L"w:jc");
		my_node.append_attribute(L"w:val").set_value(L"center");
		pugi::xml_node my_node2 = tblPr.append_child(L"w:tblW");
		my_node2.append_attribute(L"w:w").set_value(L"5000");
		my_node2.append_attribute(L"w:type").set_value(L"pct");
	}

	pugi::xml_node tblBorders = tblPr.append_child(L"w:tblBorders");

	if (f & duckx::border) {
		createBorder(tblBorders, L"w:top");
		createBorder(tblBorders, L"w:left");
		createBorder(tblBorders, L"w:bottom");
		createBorder(tblBorders, L"w:right");
		createBorder(tblBorders, L"w:insideH");
		createBorder(tblBorders, L"w:insideV");
	}
	else {
		createEmptyBorder(tblBorders, L"w:top");
		createEmptyBorder(tblBorders, L"w:left");
		createEmptyBorder(tblBorders, L"w:bottom");
		createEmptyBorder(tblBorders, L"w:right");
		createEmptyBorder(tblBorders, L"w:insideH");
		createEmptyBorder(tblBorders, L"w:insideV");
	}

	Table* myTable = new Table();
	myTable->set_current(new_table);

	return *myTable;
}

duckx::TableRow&
duckx::Table::insert_row(int height)
{

	pugi::xml_node myRow = this->current.append_child(L"w:tr");
	if (height != -1) {
		pugi::xml_node trPr = myRow.append_child(L"w:trPr");
		trPr.append_child(L"w:trHeight")
			.append_attribute(L"w:val")
			.set_value(height);
	}
	TableRow* tR = new TableRow();
	tR->set_current(myRow);
	return *tR;

}

duckx::Paragraph* insert_paragraph_tag(duckx::Paragraph* p, const std::wstring& text, duckx::formatting_flag f)
{
	bool is_bold = false;
	bool is_italic = false;
	bool is_underline = false;
	bool is_strikethrough = false;

	bool is_superscript = false;
	bool is_subscript = false;
	bool is_smallcaps = false;
	bool is_shadow = false;

	int regim = 0;
	bool is_close = false;
	wchar_t tag;

	int n = 0;
	int nn = 0;
	int text_len = text.length();

	for (int i = 0; i < text_len; i++) {
		wchar_t c = text[i];

		switch (regim) {
		case 0:
			if (c == L'<') {
				nn = i;
				regim = 1;
			}
			break;
		case 1:
		case 2:
			if (c == L'/') regim = 2;
			else if (c == L'b' || c == L'i' || c == L'u' || c == L's' || c == L'!' || c == L'!' || c == L'!' || c == L'!') {
				is_close = (regim == 2);
				tag = c;
				regim = 3;
			}
			else {
				regim = 0;
			}
			break;
		case 3:
			if (c == L'>') {
				std::wstring text2 = text.substr(n, nn - n);
				duckx::formatting_flag f2 = f
					| (is_bold ? duckx::bold : 0)
					| (is_italic ? duckx::italic : 0)
					| (is_underline ? duckx::underline : 0)
					| (is_strikethrough ? duckx::strikethrough : 0)

					| (is_superscript ? duckx::superscript : 0)
					| (is_subscript ? duckx::subscript : 0)
					| (is_smallcaps ? duckx::smallcaps : 0)
					| (is_shadow ? duckx::shadow : 0)
					;

				p->add_run(text2, f2);

				if (tag == L'b') is_bold = !is_close;
				if (tag == L'i') is_italic = !is_close;
				if (tag == L'u') is_underline = !is_close;
				if (tag == L's') is_strikethrough = !is_close;

				if (tag == L'!') is_superscript = !is_close;
				if (tag == L'!') is_subscript = !is_close;
				if (tag == L'!') is_smallcaps = !is_close;
				if (tag == L'!') is_shadow = !is_close;

				n = i + 1;
			}
			regim = 0;
			break;
		}
	}

	if (text_len > n) {
		std::wstring text2 = text.substr(n, text.length() - n);
		p->add_run(text2, f);
	}

	return p;
}

duckx::Paragraph* insert_paragraph_node(pugi::xml_node& node, const std::wstring& text, duckx::formatting_flag f)
{
	duckx::Paragraph* p = nullptr;

	wchar_t* text2 = _wcsdup(text.c_str());
	wchar_t* colstate;
	wchar_t* pwc = std::wcstok(text2, L"\n", &colstate);

	while (pwc != NULL) {
		pugi::xml_node new_para =
			node.append_child(L"w:p");

		p = new duckx::Paragraph();
		p->set_current(new_para);
		insert_paragraph_tag(p, pwc, f);
		//        p->add_run(pwc, f);
		pwc = std::wcstok(NULL, L"\n", &colstate);
	}
	free(text2);

	if (!p) {
		pugi::xml_node new_para =
			node.append_child(L"w:p");

		p = new duckx::Paragraph();
		p->set_current(new_para);
	}

	return p;
}


duckx::TableCell&
duckx::TableRow::insert_cell(const wchar_t* text, duckx::formatting_flag f, float widthCellByCount, int verticalMerge)
{
	pugi::xml_node myCell = this->current.append_child(L"w:tc");

	pugi::xml_node tcPr = myCell.append_child(L"w:tcPr");
	if (f & duckx::fill_green) {
		tcPr.append_child(L"w:shd")
			.append_attribute(L"w:fill")
			.set_value(L"00FF00");
	}
	if (f & duckx::vertical_aligment_text) {
		tcPr.append_child(L"w:textDirection")
			.append_attribute(L"w:val")
			.set_value(L"btLr");
	}
	if (widthCellByCount > 0) {
		CString wCell;
		wCell.Format("%f", widthCellByCount);
		tcPr.append_child(L"w:gridSpan")
			.append_attribute(L"w:val")
			.set_value(wCell.AllocSysString());
		if (verticalMerge == 1) {
			tcPr.append_child(L"w:vMerge")
				.append_attribute(L"w:val")
				.set_value(L"restart");
		}
		else
			if (verticalMerge == 2) {
				tcPr.append_child(L"w:vMerge");
			}
	}



	insert_paragraph_node(myCell, text, f);

	TableCell* tC = new TableCell();
	tC->set_current(myCell);
	return *tC;
}

duckx::Paragraph::Paragraph() {}

duckx::Paragraph::Paragraph(pugi::xml_node parent, pugi::xml_node current) {
	this->set_parent(parent);
	this->set_current(current);
}

void duckx::Paragraph::set_parent(pugi::xml_node node) {
	this->parent = node;
	this->current = this->parent.child(L"w:p");

	this->run.set_parent(this->current);
}

void duckx::Paragraph::set_current(pugi::xml_node node) {
	this->current = node;
}

duckx::Paragraph& duckx::Paragraph::next() {
	this->current = this->current.next_sibling();
	this->run.set_parent(this->current);
	return *this;
}

bool duckx::Paragraph::has_next() const { return this->current != 0; }

duckx::Run& duckx::Paragraph::runs() {
	this->run.set_parent(this->current);
	return this->run;
}

duckx::Run& duckx::Paragraph::add_run(const std::wstring& text,
	duckx::formatting_flag f) {
	return this->add_run(text.c_str(), f);
}

duckx::Run& duckx::Paragraph::add_run(const wchar_t* text,
	duckx::formatting_flag f) {

	pugi::xml_node meta2 = this->current.append_child(L"w:pPr");
	// Add new run
	pugi::xml_node new_run = this->current.append_child(L"w:r");
	// Insert meta to new run
	pugi::xml_node meta = new_run.append_child(L"w:rPr");

	if (f & duckx::bold)
		meta.append_child(L"w:b");

	if (f & duckx::italic)
		meta.append_child(L"w:i");

	if (f & duckx::underline)
		meta.append_child(L"w:u").append_attribute(L"w:val").set_value(L"single");

	if (f & duckx::strikethrough)
		meta.append_child(L"w:strike")
		.append_attribute(L"w:val")
		.set_value(L"true");

	if (f & duckx::superscript)
		meta.append_child(L"w:vertAlign")
		.append_attribute(L"w:val")
		.set_value(L"superscript");
	else if (f & duckx::subscript)
		meta.append_child(L"w:vertAlign")
		.append_attribute(L"w:val")
		.set_value(L"subscript");

	if (f & duckx::smallcaps)
		meta.append_child(L"w:smallCaps")
		.append_attribute(L"w:val")
		.set_value(L"true");

	if (f & duckx::shadow)
		meta.append_child(L"w:shadow")
		.append_attribute(L"w:val")
		.set_value(L"true");
	if (f & duckx::size_8)
		meta.append_child(L"w:sz").append_attribute(L"w:val").set_value(L"16");

	if (f & duckx::size_11)
		meta.append_child(L"w:sz").append_attribute(L"w:val").set_value(L"22");

	if (f & duckx::size_12)
		meta.append_child(L"w:sz").append_attribute(L"w:val").set_value(L"24");




	if (f & duckx::tnr_style) {
		pugi::xml_node my_node = meta.append_child(L"w:rFonts");
		my_node.append_attribute(L"w:hAnsi").set_value(L"Times New Roman");
		my_node.append_attribute(L"w:ascii").set_value(L"Times New Roman");
		my_node.append_attribute(L"w:cs").set_value(L"Times New Roman");
	}


	if (f & duckx::align_center) {
		pugi::xml_node my_node = meta2.append_child(L"w:jc");
		my_node.append_attribute(L"w:val").set_value(L"center");
	}

	if (f & duckx::align_left) {
		pugi::xml_node my_node = meta2.append_child(L"w:jc");
		my_node.append_attribute(L"w:val").set_value(L"left");
	}

	if (f & duckx::align_right) {
		pugi::xml_node my_node = meta2.append_child(L"w:jc");
		my_node.append_attribute(L"w:val").set_value(L"right");
	}
	//Удаление интервала после абзаца
	if (true) {
		pugi::xml_node my_node = meta2.append_child(L"w:spacing");
		my_node.append_attribute(L"w:after").set_value(L"0");
	}




	pugi::xml_node new_run_text = new_run.append_child(L"w:t");
	// If the run starts or ends with whitespace characters, preserve them using
	// the xml:space attribute

	if (*text != 0 && (iswspace(text[0]) || iswspace(text[wcslen(text) - 1])))
		new_run_text.append_attribute(L"xml:space").set_value(L"preserve");


	new_run_text.text().set(text);

	return *new Run(this->current, new_run);
}


duckx::Paragraph&
duckx::Paragraph::insert_paragraph(const std::wstring& text,
	duckx::formatting_flag f) {

	/*

		pugi::xml_node new_para =
			this->parent.append_child(L"w:p");

		Paragraph* p = new Paragraph();
		p->set_current(new_para);
		p->add_run(text, f);
	*/

	duckx::Paragraph* p = insert_paragraph_node(this->parent, text, f);

	return *p;


}

duckx::Paragraph&
duckx::Paragraph::insert_next_page() {



	pugi::xml_node new_para =
		this->parent.append_child(L"w:p");
	new_para.append_child(L"w:r").append_child(L"w:br").append_attribute(L"w:type").set_value(L"page");
	Paragraph* p = new Paragraph();
	p->set_current(new_para);
	




	return *p;


}

duckx::Document::Document() {
	// TODO: this function must be removed!
	this->directory = "";
}

duckx::Document::Document(std::string directory) {
	this->directory = directory;
}

void duckx::Document::file(std::string directory) {
	this->directory = directory;
}

void duckx::Document::open() {
	void* buf = NULL;
	size_t bufsize;

	// Open file and load "xm" content to the document variable
	zip_t* zip =
		zip_open(this->directory.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');

	zip_entry_open(zip, "word/document.xml");
	zip_entry_read(zip, &buf, &bufsize);

	zip_entry_close(zip);
	zip_close(zip);

	this->document.load_buffer(buf, bufsize);

	free(buf);

	this->paragraph.set_parent(document.child(L"w:document").child(L"w:body"));
	document.child(L"w:document").child(L"w:body").remove_child(L"w:p");

}

void duckx::Document::save() const {
	// minizip only supports appending or writing to new files
	// so we must
	// - make a new file
	// - write any new files
	// - copy the old files
	// - delete old docx
	// - rename new file to old file

	// Read document buffer
	xml_string_writer writer;
	this->document.print(writer);

	// Open file and replace "xml" content

	std::string original_file = this->directory;
	std::string temp_file = this->directory + ".tmp";

	// Create the new file
	zip_t* new_zip =
		zip_open(temp_file.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

	// Write out document.xml
	zip_entry_open(new_zip, "word/document.xml");

	const char* buf = writer.result.c_str();

	zip_entry_write(new_zip, buf, strlen(buf));
	zip_entry_close(new_zip);

	// Open the original zip and copy all files which are not replaced by duckX
	zip_t* orig_zip =
		zip_open(original_file.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');

	// Loop & copy each relevant entry in the original zip
	int orig_zip_entry_ct = zip_total_entries(orig_zip);
	for (int i = 0; i < orig_zip_entry_ct; i++) {
		zip_entry_openbyindex(orig_zip, i);
		const char* name = zip_entry_name(orig_zip);

		// Skip copying the original file
		if (std::string(name) != std::string("word/document.xml")) {
			// Read the old content
			void* entry_buf;
			size_t entry_buf_size;
			zip_entry_read(orig_zip, &entry_buf, &entry_buf_size);

			// Write into new zip
			zip_entry_open(new_zip, name);
			zip_entry_write(new_zip, entry_buf, entry_buf_size);
			zip_entry_close(new_zip);

			free(entry_buf);
		}

		zip_entry_close(orig_zip);
	}

	// Close both zips
	zip_close(orig_zip);
	zip_close(new_zip);

	// Remove original zip, rename new to correct name
	remove(original_file.c_str());
	rename(temp_file.c_str(), original_file.c_str());
}

duckx::Paragraph& duckx::Document::paragraphs() {
	this->paragraph.set_parent(document.child(L"w:document").child(L"w:body"));
	return this->paragraph;
}

duckx::Table& duckx::Document::tables() {
	this->table.set_parent(document.child(L"w:document").child(L"w:body"));
	return this->table;
}


duckx::TableCell&
duckx::TableCell::set_width(float width)
{
	pugi::xml_node tcW = this->current.append_child(L"w:tcW");
	tcW.append_attribute(L"w:w")
		.set_value(width);

	tcW.append_attribute(L"w:type")
		.set_value(L"pct");
	//       .set_value(pct);

	return *this;
}

duckx::TableCell&
duckx::TableCell::set_merge(int widthCellByCount, int verticalMerge)
{
	if (widthCellByCount > 0) {
		pugi::xml_node tcPr = current.append_child(L"w:tcPr");
		tcPr.append_child(L"w:gridSpan")
			.append_attribute(L"w:val")
			.set_value(widthCellByCount);

		if (verticalMerge == 1) {
			tcPr.append_child(L"w:vMerge")
				.append_attribute(L"w:val")
				.set_value(L"restart");
		}
		else
			if (verticalMerge == 2) {
				tcPr.append_child(L"w:vMerge");
			}
	}

	return *this;
}

duckx::TableRow&
duckx::TableRow::set_child(const wchar_t* text)
{
	pugi::xml_node tcW = this->current.append_child(text);

	return *this;
}


void duckx::Document::set_size(float size) {
	//  m_size = size;  
}

