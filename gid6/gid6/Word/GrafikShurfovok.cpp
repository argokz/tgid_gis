#include "stdafx.h"
#include "gid6.h"

#include "gid6Doc.h"
#include "gidrView.h"

#include "win.h"
#include "geofile.h"

#include "maptabs.h"

#include "math.h"


#include "ado.h"


//#include "grid/msword8.h"
#include <fstream>


#include <duckx.hpp>


static COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
using pugi::xml_node;

BSTR getWideString(COleVariant v);

bool checkFileOpen(CString doc_name);

void addParagraph(duckx::Paragraph& ps, BSTR main, vector<BSTR> stringArray, duckx::formatting_flag f);

#include "PropertyGrid\test\PropGridDlg.h"
void GrafikShurfovki(CWnd* wnd, int year1)
{

	COleVariant v;
	CString ss;
	CString doc_name;
	doc_name.Format("%s\\%s", getenv("TMP"), _TR("График шурфовок.docx"));
	CString fname;

	if (!checkFileOpen(doc_name))
		return;
	

	fname.Format("%s\\excel2\\docx\\empty_doc.docx", argpath());

	CopyFile(fname, doc_name, 0);

	CString q, fStr;

	CAdoFile* ado = getAdo(getPsAdoName());
	fStr.Format("%ssql\\docxsql\\plan_shurf.sql", argpath());
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		void ispr_q(CString & q);
		ispr_q(q);

		//CString myId;
		//myId.Format("%d", id);
		//q.Replace("$fileID$", myId);

         ado->Execute("ALTER INDEX [PK_Individual_ShurfyID] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)");
         ado->Execute("ALTER INDEX [shurfy_shape_idx] ON [dbo].[shurfy] REBUILD PARTITION = ALL WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON)");

		bool ret = ado->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе");
			return;
		}

		duckx::Document doc(doc_name.GetString());

		doc.open();

		duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
		duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::bold | duckx::size_12;
		duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fNorm = duckx::align_center | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;


		if (!ado->isEOF()) {
			doc.paragraphs().insert_paragraph(L"«Утверждаю»", fBoldRight);
			addParagraph(doc.paragraphs(), getWideString(ado->read("Должность утверждающего")),
				{
				},
				fBoldRight);
			doc.paragraphs().insert_paragraph(L"АО «Астана-Теплотранзит»", fBoldRight);
			addParagraph(doc.paragraphs(), L" _____________________",
				{
					getWideString(ado->read("ФИО утверждающего"))

				},
				fBoldRight);


			CString day(getWideString(ado->read("День утверждения плана шурфовок")));
			CString year(getWideString(ado->read("Год утверждения плана шурфовок")));
			CString month(getWideString(ado->read("Месяц утверждения плана шурфовок")));


			CString data;
			data.Format("%s.%02d.%sг", day, atoi(month), year);

			//			addParagraph(doc.paragraphs(), L"«_____» _______________ 20__г", {}, fBoldRight);
			addParagraph(doc.paragraphs(), data.AllocSysString(), {}, fBoldRight);

			for (int i = 0; i < 4; i++)
				addParagraph(doc.paragraphs(), L"", {}, fBoldRight);

			addParagraph(doc.paragraphs(), L"ПЛАН ШУРФОВОК", {}, fBold);
			//			addParagraph(doc.paragraphs(), L"по текущему ремонту  2016г. на апрель месяц по  СЭТС №1", {}, fBold);

			CString s;
			s.Format("%d - %d", year1, year1 + 1);

			addParagraph(doc.paragraphs(), getWideString(ado->read("Назначение")),
				{ L", ", s.AllocSysString() }, fBold);

			addParagraph(doc.paragraphs(), L"", {}, fBoldRight);

			COleVariant varPosition, varFIO;
			varPosition = ado->read("Должность визирующего");
			varFIO = ado->read("ФИО визирующего");

			duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
			duckx::TableRow rowHeader = table1.insert_row();
			rowHeader.insert_cell(L"п/п", fBold);
			rowHeader.insert_cell(L"Месторасположение", fBold);
			rowHeader.insert_cell(L"Дата шурфа", fBold);
			rowHeader.insert_cell(L"Материалы и механизмы", fBold);
			rowHeader.insert_cell(L"Начальник участка", fBold);
			rowHeader.insert_cell(L"Примечание", fBold);
			int nCount = 1;
			while (!ado->isEOF()) {

				duckx::TableRow rowT1 = table1.insert_row();
				CString number;
				number.Format("%i", nCount);
				rowT1.insert_cell(number.AllocSysString(), fNormLeft);

				v = ado->read("Адрес");
				rowT1.insert_cell(getWideString(v), fNormLeft);

				//				v = ado->read("Дата утверждения плана шурфовок");
				v = ado->read("Дата шурфовки");
				rowT1.insert_cell(getWideString(v), fNorm);

				v = ado->read("Материалы и механизмы");
				rowT1.insert_cell(getWideString(v), fNorm);

				v = ado->read("Начальник участка");
				rowT1.insert_cell(getWideString(v), fNorm);

				v = ado->read("Примечание");
				rowT1.insert_cell(getWideString(v), fNorm);

				nCount += 1;
				ado->MoveNext();
			}
			for (int i = 0; i < 7; i++)
				addParagraph(doc.paragraphs(), L"", {}, fBoldRight);

			addParagraph(doc.paragraphs(), getWideString(varPosition),
				{
					L" _______________",
					getWideString(varFIO)

				},
				fBoldRight);
		}


		doc.save();

		HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
	}
}


void GrafikShurfovki(CWnd* wnd, int year1, int currentMonth)
{

	COleVariant v;
	CString ss;
	CString doc_name;
	doc_name.Format("%s\\%s", getenv("TMP"), _TR("График шурфовок за месяц.docx"));
	CString fname;

	if (!checkFileOpen(doc_name))
		return;

	fname.Format("%s\\excel2\\docx\\empty_doc.docx", argpath());

	CopyFile(fname, doc_name, 0);

	CString q, fStr;

	CAdoFile* ado = getAdo(getPsAdoName());
	fStr.Format("%ssql\\docxsql\\plan_shurf_mon.sql", argpath());
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		void ispr_q(CString & q);
		ispr_q(q);
		CString monthCondition;
		monthCondition.Format("AND MONTH(d.data_nachala_plan)=%i", currentMonth);
		q.Replace("$and_name_date_mon$", monthCondition);

		//CString myId;
		//myId.Format("%d", id);
		//q.Replace("$fileID$", myId);
		bool ret = ado->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе");
			return;
		}

		duckx::Document doc(doc_name.GetString());

		doc.open();

		duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
		duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::bold | duckx::size_12;
		duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fNorm = duckx::align_center | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fNormSmallRight = duckx::align_right | duckx::size_11 | duckx::tnr_style;
		duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;


		if (!ado->isEOF()) {
			doc.paragraphs().insert_paragraph(L"«Утверждаю»", fBoldRight);


			addParagraph(doc.paragraphs(), L"__________________________", {}, fBoldRight);
			addParagraph(doc.paragraphs(), L"__________________________", {}, fBoldRight);
			addParagraph(doc.paragraphs(), L"__________________________", {}, fBoldRight);

			CString day(getWideString(ado->read("День утверждения плана шурфовок")));
			CString year(getWideString(ado->read("Год утверждения плана шурфовок")));
			CString month(getWideString(ado->read("Месяц утверждения плана шурфовок")));


			addParagraph(doc.paragraphs(), L"«_____» _______________ 20__г", {}, fBoldRight);

			for (int i = 0; i < 4; i++)
				addParagraph(doc.paragraphs(), L"", {}, fBoldRight);

			addParagraph(doc.paragraphs(), L"ГРАФИК ШУРФОВОК", {}, fBold);
			//			addParagraph(doc.paragraphs(), L"по текущему ремонту  2016г. на апрель месяц по  СЭТС №1", {}, fBold);

			CString mon_name(int i);
			CString s;
			s.Format("на месяц %s, отопительного сезона %i-%iг", mon_name(currentMonth), year1, year1 + 1);

			addParagraph(doc.paragraphs(), s.AllocSysString(),
				{  }, fBold);

			addParagraph(doc.paragraphs(), L"", {}, fBoldRight);

			COleVariant varPosition, varFIO;
			varPosition = ado->read("Должность визирующего");
			varFIO = ado->read("ФИО визирующего");

			duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
			duckx::TableRow rowHeader = table1.insert_row();
			rowHeader.insert_cell(L"п/п", fBold);
			rowHeader.insert_cell(L"Месторасположение", fBold);
			rowHeader.insert_cell(L"Дата шурфа", fBold);
			rowHeader.insert_cell(L"Материалы и механизмы", fBold);
			rowHeader.insert_cell(L"Начальник участка", fBold);
			rowHeader.insert_cell(L"Примечание", fBold);
			int nCount = 1;
			while (!ado->isEOF()) {

				duckx::TableRow rowT1 = table1.insert_row();
				CString number;
				number.Format("%i", nCount);
				rowT1.insert_cell(number.AllocSysString(), fNormLeft);

				v = ado->read("Адрес");
				rowT1.insert_cell(getWideString(v), fNormLeft);

				//				v = ado->read("Дата утверждения плана шурфовок");
				v = ado->read("Дата шурфовки");
				rowT1.insert_cell(getWideString(v), fNorm);

				v = ado->read("Материалы и механизмы");
				rowT1.insert_cell(getWideString(v), fNorm);

				v = ado->read("Начальник участка");
				rowT1.insert_cell(getWideString(v), fNorm);

				v = ado->read("Примечание");
				rowT1.insert_cell(getWideString(v), fNorm);

				nCount += 1;
				ado->MoveNext();
			}
			for (int i = 0; i < 7; i++)
				addParagraph(doc.paragraphs(), L"", {}, fBoldRight);

			addParagraph(doc.paragraphs(), L"ФИО                                                                                                                    _______________",
				{
				},
				fBoldRight);
			duckx::Paragraph p = doc.paragraphs().insert_paragraph(L"Должность                                                                                                                       ", fBoldRight);
			p.add_run(L"(Подпись)", fNormSmallRight);


		}


		doc.save();

		HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);
	}
}