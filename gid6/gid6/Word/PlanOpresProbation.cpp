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

CString sezon_name(int y);

#include "PropertyGrid\test\PropGridDlg.h"
void PlanOpresProbation(CWnd* wnd, int m_sezon_korrozia)
{
	struct DetailStruct {

		CString name, typeProbation, warmNet, type;
		double diametr, lenght;
	};
	struct ContourStruct {
		CString nameContour;
		double lenght;
		CString date, fio, position, subdivision;
		vector<DetailStruct> details;
	};
	struct HeatSourceStruct {
		CString heatSource;
		map<CString, ContourStruct*> contours;
	};



	COleVariant v;
	CString ss;
	CString doc_name;
	doc_name.Format("%s\\%s", getenv("TMP"), _TR("План гидравлических испытаний (опрессовки) тепловых сетей.docx"));
	CString fname;

	if (!checkFileOpen(doc_name))
		return;


	fname.Format("%s\\excel2\\docx\\empty_doc.docx", argpath());

	CopyFile(fname, doc_name, 0);

	CString q, fStr;

	CAdoFile* ado = getAdo(getPsAdoName());
	fStr.Format("%ssql\\docxsql\\plan_gidravlicheskih_ispytanij.sql", argpath());
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		void ispr_q(CString & q);
		ispr_q(q);

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

		duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
		duckx::formatting_flag fNorm11 = duckx::align_left | duckx::tnr_style | duckx::size_11;
		duckx::formatting_flag fBold12 = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
		duckx::formatting_flag fItalic12 = duckx::align_center | duckx::tnr_style | duckx::italic | duckx::size_12;
		duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::italic | duckx::bold | duckx::size_12;
		duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
		duckx::formatting_flag fNorm = duckx::align_center | duckx::size_8 | duckx::tnr_style;
		duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;


		if (!ado->isEOF()) {
			//			doc.paragraphs().insert_paragraph(L"Сведения по участку тепловой сети", fBoldLeft);


            COleDateTime v = COleDateTime::GetCurrentTime();
            CString energosist = readTableValue(ado, "SELECT TOP 1 kratkoe_naimenovanie_es FROM energosistemy");

            CString  s;
            s.Format("%s\n%s", energosist, v.Format("%d.%m.%Y"));   

//			doc.paragraphs().insert_paragraph(energosist.AllocSysString(), fNormRight);
			doc.paragraphs().insert_paragraph(s.AllocSysString(), fNormRight);


//			doc.paragraphs().insert_paragraph(L"Наименование теплоснабжающей организации", fBoldRight);
//			doc.paragraphs().insert_paragraph(L"Дата на ПК", fBoldRight);



			for (int i = 0; i < 2; i++)
				doc.paragraphs().insert_paragraph(L"", fBoldRight);
			doc.paragraphs().insert_paragraph(L"ПЛАН", fBold12);
			doc.paragraphs().insert_paragraph(L"гидравлических испытаний (опрессовки) тепловых сетей", fBold12);

			
//            doc.paragraphs().insert_paragraph(L"отопительный сезон/период", fItalic12);

//            s.Format("%s\nна «%s»", sezon_name(m_sezon_korrozia), v.Format("%d.%m.%Y"));   
//            doc.paragraphs().insert_paragraph(L"отопительный сезон/период", fItalic12);
            doc.paragraphs().insert_paragraph(sezon_name(m_sezon_korrozia).AllocSysString(), fItalic12);


			duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
			duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");;
			rowHeader.insert_cell(L"п/п", fBold);
			rowHeader.insert_cell(L"Наименование участка", fBold);
			rowHeader.insert_cell(L"Вид испытания", fBold);
			rowHeader.insert_cell(L"Тепловая сеть", fBold);
			rowHeader.insert_cell(L"Диаметр усл, мм", fBold);
			rowHeader.insert_cell(L"Тип", fBold);
			rowHeader.insert_cell(L"Протяженность, м", fBold);


			map<CString, HeatSourceStruct*> data;


			while (!ado->isEOF()) {

				CString currentHeatSource(getWideString(ado->read("Источник тепла ")));
				CString currentContourName(getWideString(ado->read("Наименование/Адрес контура")));
				if (currentHeatSource.IsEmpty())
					currentHeatSource = "**empty**";
				if (currentContourName.IsEmpty())
					currentContourName = "**empty**";


				HeatSourceStruct* structHeatSource;
				if (data.find(currentHeatSource) == data.end())
				{
					structHeatSource = new HeatSourceStruct();
					structHeatSource->heatSource = currentHeatSource;
					data[currentHeatSource] = structHeatSource;
				}
				else
				{
					structHeatSource = data[currentHeatSource];
				}

				ContourStruct* structContour;
				if (structHeatSource->contours.find(currentContourName) == structHeatSource->contours.end())
				{
					structContour = new ContourStruct();
					structContour->nameContour = currentContourName;
					structContour->date = getWideString(ado->read("date_opres"));
					structContour->fio = getWideString(ado->read("ФИО"));
					structContour->subdivision = getWideString(ado->read("Подразделение"));
					structContour->position = getWideString(ado->read("Должность"));
					structContour->lenght = ado->read_double("Протяженность контура, м");
					structHeatSource->contours[currentContourName] = structContour;
				}
				else
				{
					structContour = structHeatSource->contours[currentContourName];
				}
				DetailStruct* detail;
				CString s1, s2, s3;
				detail = new DetailStruct();
				detail->name = getWideString(ado->read("Наименование участка"));
				detail->typeProbation = getWideString(ado->read("Вид испытания"));
				s2 = getWideString(ado->readStr("Начальная камера"));
				s3 = getWideString(ado->readStr("Конечная камера"));
				if (s2.IsEmpty()) {
					s1 = "-";
				}
				else
					s1.Format("от %s, до %s", s2, s3);
				detail->warmNet = s1;
				detail->type = getWideString(ado->read("Признак сети"));
				detail->diametr = ado->read_double("Диаметр условный, мм");
				detail->lenght = ado->read_double("Протяженность, м");
				structContour->details.push_back(*detail);

				ado->MoveNext();
			}
			int number = 1;
			auto addOne = [&](HeatSourceStruct* value1) {
				duckx::TableRow row = table1.insert_row();

				duckx::TableCell cell0 = row.insert_cell(L"", fBold12, 7);
				if (strcmp(value1->heatSource, "**empty**") == 0)
					cell0.paragraphs().insert_paragraph(L"-", fBold12);
				else
					cell0.paragraphs().insert_paragraph(value1->heatSource.AllocSysString(), fBold12);
				cell0.paragraphs().insert_paragraph(L"", fBold12);


				for (const auto& [key2, value2] : value1->contours) {
					duckx::TableRow row1 = table1.insert_row();
					CString info;
					info.Format("Дата опресовки: %s %s", value2->date, value2->nameContour);
					duckx::TableCell cell1 = row1.insert_cell(info.AllocSysString(), fNorm11, 7);
					info.Format("Руководитель испытаний: %s %s %s", value2->position, value2->subdivision, value2->fio);
					cell1.paragraphs().insert_paragraph(info.AllocSysString(), fNorm11);
					info.Format("Протяженность контура: %.2f м", value2->lenght);
					cell1.paragraphs().insert_paragraph(info.AllocSysString(), fNorm11);

					for (const auto& value3 : value2->details) {
						duckx::TableRow row2 = table1.insert_row();
						CString numberText;
						numberText.Format("%i", number);
						row2.insert_cell(numberText.AllocSysString(), fNormLeft, 1);
						row2.insert_cell(value3.name.AllocSysString(), fNormLeft, 1);
						row2.insert_cell(value3.typeProbation.AllocSysString(), fNormLeft, 1);
						row2.insert_cell(value3.warmNet.AllocSysString(), fNormLeft, 1);

						numberText.Format("%.2f", value3.diametr);
						row2.insert_cell(numberText.AllocSysString(), fNormLeft, 1);

						row2.insert_cell(value3.type.AllocSysString(), fNormLeft, 1);
						numberText.Format("%.2f", value3.lenght);
						row2.insert_cell(numberText.AllocSysString(), fNormLeft, 1);
						number++;
					}
				}
			};

			vector<CString> emptyData;
			for (const auto& [key1, value1] : data) {
				if (strcmp(value1->heatSource, "**empty**") == 0)
				{
					emptyData.push_back(key1);
				}
				else
					addOne(value1);
			}

			for (int i = 0; i < emptyData.size(); i++) {
				addOne(data[emptyData[i]]);
			}

			doc.save();

			HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);

		}
		else {
			AfxMessageBox("Нет данных");
		}
	}
}

void GraphOpresProbation(CWnd* wnd, int m_sezon_korrozia)
{
	struct DetailStruct {
		map<int, int> mounths;
		CString name, typeProbation, warmNet, type;
		double diametr, lenght;
	};
	struct ContourStruct {
		CString nameContour;
		double lenght;
		CString date, fio, position, subdivision;
		vector<DetailStruct> details;
	};
	struct HeatSourceStruct {
		CString heatSource;
		map<CString, ContourStruct*> contours;
	};


	const char* mounths[12]
		= { "янв", "февр", "март", "апр", "май", "июнь", "июль", "авг" , "сент", "окт", "нояб", "дек" };
	COleVariant v;
	CString ss;
	CString doc_name;
	doc_name.Format("%s\\%s", getenv("TMP"), _TR("График гидравлических испытаний (опрессовки) тепловых сетей.docx"));
	CString fname;

	if (!checkFileOpen(doc_name))
		return;


	fname.Format("%s\\excel2\\docx\\empty_doc_horizontal.docx", argpath());

	CopyFile(fname, doc_name, 0);

	CString q, fStr;

	CAdoFile* ado = getAdo(getPsAdoName());
	fStr.Format("%ssql\\docxsql\\grafik_ispytanij.sql", argpath());
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		void ispr_q(CString & q);
		ispr_q(q);

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

		duckx::formatting_flag fBold = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_8;
		duckx::formatting_flag fNorm11 = duckx::align_left | duckx::tnr_style | duckx::size_11;
		duckx::formatting_flag fBold12 = duckx::align_center | duckx::tnr_style | duckx::bold | duckx::size_12;
		duckx::formatting_flag fItalic12 = duckx::align_center | duckx::tnr_style | duckx::italic | duckx::size_12;
		duckx::formatting_flag fBoldRight = duckx::align_right | duckx::tnr_style | duckx::italic | duckx::bold | duckx::size_12;
		duckx::formatting_flag fNormLeft = duckx::align_left | duckx::size_8 | duckx::tnr_style;
		duckx::formatting_flag fNorm = duckx::align_center | duckx::size_8 | duckx::tnr_style;
		duckx::formatting_flag fNormRight = duckx::align_right | duckx::size_12 | duckx::tnr_style;
		duckx::formatting_flag fBoldLeft = duckx::align_left | duckx::tnr_style | duckx::bold | duckx::size_12;


		if (!ado->isEOF()) {
			//			doc.paragraphs().insert_paragraph(L"Сведения по участку тепловой сети", fBoldLeft);

//			doc.paragraphs().insert_paragraph(L"Наименование теплоснабжающей организации", fBoldRight);
//			doc.paragraphs().insert_paragraph(L"Дата на ПК", fBoldRight);

            COleDateTime v = COleDateTime::GetCurrentTime();
            CString energosist = readTableValue(ado, "SELECT TOP 1 kratkoe_naimenovanie_es FROM energosistemy");

            CString  s;
            s.Format("%s\n%s", energosist, v.Format("%d.%m.%Y"));   

//			doc.paragraphs().insert_paragraph(energosist.AllocSysString(), fNormRight);
			doc.paragraphs().insert_paragraph(s.AllocSysString(), fNormRight);



			for (int i = 0; i < 2; i++)
				doc.paragraphs().insert_paragraph(L"", fBoldRight);
			doc.paragraphs().insert_paragraph(L"ГРАФИК", fBold12);
			doc.paragraphs().insert_paragraph(L"гидравлических испытаний (опрессовки) тепловых сетей", fBold12);
//			doc.paragraphs().insert_paragraph(L"отопительный сезон/период", fItalic12);
            doc.paragraphs().insert_paragraph(sezon_name(m_sezon_korrozia).AllocSysString(), fItalic12);


			duckx::Table table1 = doc.tables().insert_table(duckx::align_center | duckx::border);
			duckx::TableRow rowHeader = table1.insert_row().set_child(L"w:tblHeader");;
			rowHeader.insert_cell(L"п/п", fBold);
			rowHeader.insert_cell(L"Наименование участка", fBold);
			rowHeader.insert_cell(L"Вид испытания", fBold);
			rowHeader.insert_cell(L"Тепловая сеть", fBold);
			rowHeader.insert_cell(L"Диаметр усл, мм", fBold);
			rowHeader.insert_cell(L"Тип", fBold);
			rowHeader.insert_cell(L"Протяженность, м", fBold);

			for (int i = 0; i < 12; i++) {
				CString mName;
				mName.Format("%s", mounths[i]);
				rowHeader.insert_cell(mName.AllocSysString(), fBold);
			}

			map<CString, HeatSourceStruct*> data;


			while (!ado->isEOF()) {

				CString currentHeatSource(getWideString(ado->read("Источник тепла ")));
				CString currentContourName(getWideString(ado->read("Наименование/Адрес контура")));
				if (currentHeatSource.IsEmpty())
					currentHeatSource = "**empty**";
				if (currentContourName.IsEmpty())
					currentContourName = "**empty**";


				HeatSourceStruct* structHeatSource;
				if (data.find(currentHeatSource) == data.end())
				{
					structHeatSource = new HeatSourceStruct();
					structHeatSource->heatSource = currentHeatSource;
					data[currentHeatSource] = structHeatSource;
				}
				else
				{
					structHeatSource = data[currentHeatSource];
				}

				ContourStruct* structContour;
				if (structHeatSource->contours.find(currentContourName) == structHeatSource->contours.end())
				{
					structContour = new ContourStruct();
					structContour->nameContour = currentContourName;
					structContour->date = getWideString(ado->read("date_opres"));
					structContour->fio = getWideString(ado->read("ФИО"));
					structContour->subdivision = getWideString(ado->read("Подразделение"));
					structContour->position = getWideString(ado->read("Должность"));
					structContour->lenght = ado->read_double("Протяженность контура, м");
					structHeatSource->contours[currentContourName] = structContour;
				}
				else
				{
					structContour = structHeatSource->contours[currentContourName];
				}
				DetailStruct* detail;
				CString s1, s2, s3;
				detail = new DetailStruct();
				detail->name = getWideString(ado->read("Наименование участка"));
				detail->typeProbation = getWideString(ado->read("Вид испытания"));
				s2 = getWideString(ado->readStr("Начальная камера"));
				s3 = getWideString(ado->readStr("Конечная камера"));
				if (s2.IsEmpty()) {
					s1 = "-";
				}
				else
					s1.Format("от %s, до %s", s2, s3);
				detail->warmNet = s1;
				detail->type = getWideString(ado->read("Признак сети"));
				detail->diametr = ado->read_double("Диаметр условный, мм");
				detail->lenght = ado->read_double("Протяженность, м");
				for (int i = 0; i < 12; i++) {
					detail->mounths[i] = ado->read_long(mounths[i]);
				}
				structContour->details.push_back(*detail);




				ado->MoveNext();
			}
			int number = 1;
			auto addOne = [&](HeatSourceStruct* value1) {
				duckx::TableRow row = table1.insert_row();

				duckx::TableCell cell0 = row.insert_cell(L"", fBold12, 19);
				if (strcmp(value1->heatSource, "**empty**") == 0)
					cell0.paragraphs().insert_paragraph(L"-", fBold12);
				else
					cell0.paragraphs().insert_paragraph(value1->heatSource.AllocSysString(), fBold12);
				cell0.paragraphs().insert_paragraph(L"", fBold12);

		


				for (const auto& [key2, value2] : value1->contours) {
					duckx::TableRow row1 = table1.insert_row();
					CString info;
    				info.Format("Дата опресовки: %s %s", value2->date, value2->nameContour);
					duckx::TableCell cell1 = row1.insert_cell(info.AllocSysString(), fNorm11, 19);
					info.Format("Руководитель испытаний: %s %s %s", value2->position, value2->subdivision, value2->fio);
					cell1.paragraphs().insert_paragraph(info.AllocSysString(), fNorm11);
					info.Format("Протяженность контура: %.2f м", value2->lenght);
					cell1.paragraphs().insert_paragraph(info.AllocSysString(), fNorm11);

					for (const auto& value3 : value2->details) {
						duckx::TableRow row2 = table1.insert_row();
						CString numberText;
						numberText.Format("%i", number);
						row2.insert_cell(numberText.AllocSysString(), fNormLeft, 1);
						row2.insert_cell(value3.name.AllocSysString(), fNormLeft, 1);
						row2.insert_cell(value3.typeProbation.AllocSysString(), fNormLeft, 1);
						row2.insert_cell(value3.warmNet.AllocSysString(), fNormLeft, 1);

						numberText.Format("%.2f", value3.diametr);
						row2.insert_cell(numberText.AllocSysString(), fNormLeft, 1);

						row2.insert_cell(value3.type.AllocSysString(), fNormLeft, 1);
						numberText.Format("%.2f", value3.lenght);
						row2.insert_cell(numberText.AllocSysString(), fNormLeft, 1);
						number++;

						for (int i = 0; i < 12; i++) {

							if (value3.mounths.at(i) == 1) {
								row2.insert_cell(L"", fNormLeft | duckx::fill_green, 1);
							}
							else
							{
								row2.insert_cell(L"", fNormLeft, 1);
							}
						}
					}
				}
			};
			vector<CString> emptyData;
			for (const auto& [key1, value1] : data) {
				if (strcmp(value1->heatSource, "**empty**") == 0)
				{
					emptyData.push_back(key1);
				}
				else
					addOne(value1);
			}

			for (int i = 0; i < emptyData.size(); i++) {
				addOne(data[emptyData[i]]);
			}

			doc.save();

			HINSTANCE hRun = ShellExecute(wnd->GetSafeHwnd(), "open", doc_name, NULL, NULL, SW_SHOWMAXIMIZED);

		}
		else {
			AfxMessageBox("Нет данных");
		}
	}
}