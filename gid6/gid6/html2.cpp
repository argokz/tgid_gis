#include "stdafx.h"
#include "win.h"
#include "ado.h"

void print_index1(FILE *f, const char *title)
{
fprintf(f, "<html xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:x=\"urn:schemas-microsoft-com:office:excel\" xmlns=\"http://www.w3.org/TR/REC-html40\">\n");
fprintf(f, "<head>\n");
fprintf(f, "<meta name=\"Excel Workbook Frameset\">\n");
fprintf(f, "<meta http-equiv=Content-Type content=\"text/html; charset=windows-1251\">\n");
fprintf(f, "<meta name=ProgId content=Excel.Sheet>\n");
fprintf(f, "<xml>\n");
fprintf(f, " <x:ExcelWorkbook>\n");
fprintf(f, "  <x:ExcelWorksheets>\n");
}

void print_index(FILE *f, const char *title, const char *fn)
{
  fprintf(f, "<x:ExcelWorksheet><x:Name>%s</x:Name><x:WorksheetSource HRef=\"%s\"/></x:ExcelWorksheet>\n", title, fn);
}

void print_index2(FILE *f)
{
fprintf(f, "  </x:ExcelWorksheets>\n");
fprintf(f, "  <x:Stylesheet HRef=\"stylesheet.css\"/>\n");
fprintf(f, "  <x:ActiveSheet>0</x:ActiveSheet>\n");
fprintf(f, "  <x:ProtectStructure>False</x:ProtectStructure>\n");
fprintf(f, "  <x:ProtectWindows>False</x:ProtectWindows>\n");
fprintf(f, " </x:ExcelWorkbook>\n");
fprintf(f, "</xml>\n");
fprintf(f, "</head>\n");
fprintf(f, "</html>\n");
}

void print_doc_end(FILE *f)
{
  if (!f) return;
  fprintf(f, "</body>\n</html>\n");
  fclose(f);
}

void printTd(CAdoFile* ado, FILE* f, const char* param, const int type, int count = 1) {
	for (int i=0;i<count; i++)
	switch (type) {
	case 0://empty
		fprintf(f, "<td></td>");
		break;
	case 1://long
		fprintf(f, "<td>%d</td>", ado->read_long(param));
		break;
	case 2://double
		fprintf(f, "<td>%f</td>", ado->read_double(param));
		break;
	case 3://string
		fprintf(f, "<td>%s</td>", ado->readStr(param));
		break;

	}


}

void printTd(CAdoFile* ado, FILE* f, int id, const int type, int count = 1) {
	for (int i = 0;i < count; i++)
		switch (type) {
		case 0://empty
			fprintf(f, "<td></td>");
			break;
		case 1://long
			fprintf(f, "<td>%d</td>", ado->read_long(id));
			break;
		case 2://double
			fprintf(f, "<td>%f</td>", ado->read_double(id));
			break;
		case 3://string
			fprintf(f, "<td>%s</td>", ado->readStr(id));
			break;

		}


}

/*
FILE *print_doc(FILE *fi, const char *fn, const char *title, void (*filtr) (CString &s1) = NULL)
{
  return 0;
}
*/



FILE* print_doc_with_path(FILE* fi, const char* filePath, const char* title, void (*filtr) (CString& s1) = NULL) {
	FILE* f, * g;

	CString fn1;

	fn1 = filePath;
	fn1.Replace(".html", "");
	fn1.Replace(".htm", "");
	fn1 += "_";
	fn1 += title;
	fn1 += ".html";
	fn1.Replace("\\", "_");
	fn1.Replace("/", "_");

	print_index(fi, title, fn1);

	CString tmpName;
	//  tmpName.Format("%s\\%s", getenv("TMP"), fn);
	//  tmpName.Format("%shtml\\2\\%s", argpath(), fn);


	tmpName.Format("%s\\%s", getenv("TMP"), fn1);

	CopyFile(argpath() + CString("htmlA\\stylesheet.css"), CString(getenv("TMP")) + "\\stylesheet.css", false),

		g = fopen(tmpName, "w");
	if (!g) return NULL;



	f = fopenexe(filePath, "r");

	if (!f) return g;

	//  print_head(g, _TR("Паспорт распределительной сети"));

	/*
	  while (1) {
		int c = getc(f);

		if (feof(f)) break;

		fputc(c, g);
	  }
	*/

	char str[10240];

	while (!feof(f)) {
		fgetstr(str, 10230, f);

		CString s = str;

		if (filtr) {
			filtr(s);
		}

		fputs(s, g);
		fputs("\n", g);
	}



	fclose(f);
	return g;
}

FILE *print_doc(FILE *fi, const char *fn, const char *title, void (*filtr) (CString &s1) = NULL)
{
	
	return print_doc_with_path(fi, CString("html\\")+fn, title, filtr);
}




FILE *print_doc_map(FILE *fi, const char *fn, const char *title, const map<CString, CString> &map_fn)
{
  FILE *f, *g;

  CString fn1;
  
  fn1 = fn;;
  fn1.Replace(".html", "");
  fn1.Replace(".htm", "");
  fn1 += "_";
  fn1 += title;
  fn1 += ".html";
  fn1.Replace("\\", "_");
  fn1.Replace("/", "_");

  print_index(fi, title, fn1);

  CString tmpName;


  tmpName.Format("%s\\%s", getenv("TMP"), fn1);

  CopyFile(argpath()+CString("htmlA\\stylesheet.css"), CString(getenv("TMP"))+"\\stylesheet.css", false), 
  
  g = fopen(tmpName, "w");
  if (!g) return NULL;

  CString fn2 = CString("html\\")+fn;

  f = fopenexe(fn2, "r");

  if (!f) return g;

  char str[10240];

  while (!feof(f)) {
    fgetstr(str, 10230, f);

    CString s = str;

/*
    if (filtr) {
      filtr(s);
    }
*/

    for (auto it : map_fn) {
      s.Replace(it.first, it.second);
    }

    fputs(s, g);
    fputs("\n", g);
  }



  fclose(f);
  return g;
}
