#include "stdafx.h"
#include "MainFrm.h"
#include <locale.h>
#include "css.h"
#include "ado.h"

static CSS css;
void print_index1(FILE* f, const char* title);
void print_index2(FILE* f);
FILE* print_doc(FILE* fi, const char* fn, const char* title, void (*filtr) (CString& s1) = NULL);
void print_doc_end(FILE* f);
CString readQ(CAdoFile* ado, const char* tn, const char* tn2, int fileID);
//void printTd(CAdoFile* ado, FILE* f, const char* param, const int type, int count = 1, CString fragments);
void printTd(CAdoFile* ado, FILE* f, const char* param, const int type, int count , CString fragments);

void printDistrict(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printDistrictSite(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printDistrictSiteChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printDistrictSiteChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printMsRsChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printMsRsChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printMsChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printMsChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printRsChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printRsChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);

void printChiefMsRsCount(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printChiefMsRsDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printChiefMsCount(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printChiefMsDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printChiefRsCount(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printChiefRsDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);
void printSource(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments);

void CGidrView::OnProtPassport()
{
	CWaitCursor wait;

	CString tmpName;
	
	tmpName.Format("%s\\index_ext.html", getenv("TMP"));


	FILE* f = fopen(tmpName, "w");
	if (!f) {
		AfxMessageBox("Закройте предыдущий паспорт");
		return;
	}

	setlocale(LC_NUMERIC, "");

	print_index1(f, "");

	FILE* g = 0;

    CString fragments = m_cxema->getParAll();


	/*Протяженность по районам*/
	printDistrict(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по участкам*/
	printDistrictSite(g, f, m_cxema->m_ado, fragments);

	printChiefMsRsCount(g, f, m_cxema->m_ado, fragments);
	printChiefMsCount(g, f, m_cxema->m_ado, fragments);
	printChiefRsCount(g, f, m_cxema->m_ado, fragments);
	printSource(g, f, m_cxema->m_ado, fragments);

	/*Протяженность по районам, участку, начальнику*/
	printDistrictSiteChief(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по районам, участку, начальнику, диаметру*/
	printDistrictSiteChiefDiametr(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по МС РС начальникам*/
	printMsRsChief(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по МС РС начальникам, диаметру*/
	printMsRsChiefDiametr(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по начальникам Мс*/
	printMsChief(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по начальникам Мс, диаметру*/
	printMsChiefDiametr(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по начальникам Рс*/
	printRsChief(g, f, m_cxema->m_ado, fragments);
	/*Протяженность по начальникам Рс, диаметру*/
	printRsChiefDiametr(g, f, m_cxema->m_ado, fragments);

	
	printChiefMsRsDiametr(g, f, m_cxema->m_ado, fragments);
	
	printChiefMsDiametr(g, f, m_cxema->m_ado, fragments);
	
	printChiefRsDiametr(g, f, m_cxema->m_ado, fragments);
	

	print_index2(f);
	fclose(f);
	setlocale(LC_NUMERIC, "eng");

	tmpName.Format("%s\\index_ext.html", getenv("TMP"));

	CString cssName;
	cssName.Format("%s\\style.css", getenv("TMP"));
	css.print(cssName);


	HINSTANCE hRun = ShellExecute(GetSafeHwnd(), "open", "excel", "\"" + tmpName + "\"", NULL, SW_SHOW);

}

void printDistrict(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность по районам*/
		CString name;
		double p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_1.html", _TR("1.По районам"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_district('%s') order by flag, naimenovanie_rayona", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (1)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->name = mAdo->readStr("naimenovanie_rayona");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");
			if (tmpExt->name.IsEmpty())
				tmpExt->name = "Остальные";
			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");

				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printDistrictSite(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность по участкам*/
		CString name;
		double p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_2.html", _TR("2.По участкам"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_district_site('%s') order by flag, district_site", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (2)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");
			if (tmpExt->name.IsEmpty())
				tmpExt->name = "Остальные";
			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");

				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}


void printDistrictSiteChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность по районам участку, начальнику*/
		CString name, site, fio;
		double p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_3.html", _TR("7.Район, участок, начальник"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_district_site_chief('%s') order by flag, district_site, naimenovanie_uchastka, fio", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (3)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");
			if (tmpExt->name.IsEmpty())
				tmpExt->name = "Остальные";
			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");

				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}


void printDistrictSiteChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность по районам участку, начальнику, диаметру*/
		CString name, site, fio;
		double d, p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_4.html", _TR("8.Район,участок,нач,диам"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_district_site_chief_diameter('%s') where not fio is NULL order by flag, district_site, fio, diameterCondit", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (4)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->d = mAdo->read_double("diameterCondit");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");

				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%f</td>", tmpExt->d);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printMsRsChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность начальник МС РС*/
		CString name, site, fio;
		double p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_5.html", _TR("9.Начальники МС РС"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_ms_rs_chief('%s') where not fio is NULL order by fio", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (5)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");

				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printMsRsChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность начальник МС РС, диаметр*/
		CString name, site, fio;
		double d, p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_6.html", _TR("10.Начальники МС РС, диаметр"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_ms_rs_chief_diameter('%s') where not fio is null order by flag, fio, diameterCondit, district_site, naimenovanie_uchastka", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (6)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->d = mAdo->read_double("diameterCondit");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				
				fprintf(g, "<td>%f</td>", tmpExt->d);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}


void printMsChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность начальник МС*/
		CString name, site, fio;
		double p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_7.html", _TR("11.Начальники МС"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_ms_chief('%s') where not fio is null order by flag, fio, district_site, naimenovanie_uchastka", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (7)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printMsChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность начальник МС, диаметр*/
		CString name, site, fio;
		double d, p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_8.html", _TR("12.Начальники МС, диаметр"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_ms_chief_diameter('%s') where not fio is null order by flag, fio, diameterCondit, district_site, naimenovanie_uchastka", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (8)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->d = mAdo->read_double("diameterCondit");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				
				fprintf(g, "<td>%f</td>", tmpExt->d);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}


void printRsChief(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность начальник РС*/
		CString name, site, fio;
		double p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_9.html", _TR("13.Начальники РС"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_rs_chief('%s') where not fio is null order by flag, fio, district_site, naimenovanie_uchastka", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (9)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printRsChiefDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /*Протяженность начальник МС, диаметр*/
		CString name, site, fio;
		double d, p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_10.html", _TR("14.Начальники РС, диаметр"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_rs_chief_diameter('%s') where not fio is null order by flag, fio, diameterCondit, district_site, naimenovanie_uchastka", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (10)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->name = mAdo->readStr("district_site");
			tmpExt->site = mAdo->readStr("naimenovanie_uchastka");
			tmpExt->d = mAdo->read_double("diameterCondit");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%s</td>", tmpExt->name);
				fprintf(g, "<td>%s</td>", tmpExt->site);
				
				fprintf(g, "<td>%f</td>", tmpExt->d);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}


void printChiefMsRsCount(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /**/
		CString fio;
		double p, o, g;
		int countMs, countRs;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_11.html", _TR("3.Начальники МС,РС Кол-во"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_chief_ms_rs('%s') order by flag, fio", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (11)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->countMs = mAdo->read_long("ms_count");
			tmpExt->countRs = mAdo->read_long("rs_count");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%d</td>", tmpExt->countMs);
				fprintf(g, "<td>%d</td>", tmpExt->countRs);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}


void printChiefMsRsDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /**/
		CString fio;
		double p, o, g, d;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_12.html", _TR("15.ФИО нач-ка МС,РС, Диаметр"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_chief_ms_rs_diameter('%s') where fio != 'Без начальника' order by flag, fio, diameterCondit", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (12)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->d = mAdo->read_double("diameterCondit");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%f</td>", tmpExt->d);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printChiefMsCount(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /**/
		CString fio;
		double p, o, g;
		int count;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_13.html", _TR("4.Начальники МС Кол-во"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_chief_ms('%s') order by flag,fio", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (13)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->count = mAdo->read_long("Кол-во");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%d</td>", tmpExt->count);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}


void printChiefMsDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /**/
		CString fio;
		double p, o, g, d;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_14.html", _TR("16.ФИО нач-ка МС Диаметр"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_chief_ms_diameter('%s') where fio != 'Без начальника' order by flag, fio, diameterCondit", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (14)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->d = mAdo->read_double("diameterCondit");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%f</td>", tmpExt->d);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printChiefRsCount(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /**/
		CString fio;
		double p, o, g;
		int count;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_15.html", _TR("5.Начальники РС Кол-во"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_chief_rs('%s') order by flag, fio", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (15)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->count = mAdo->read_long("Кол-во");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%d</td>", tmpExt->count);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printChiefRsDiametr(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /**/
		CString fio;
		double p, o, g, d;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_16.html", _TR("17.ФИО нач-ка РС, Диаметр"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_chief_rs_diameter('%s') where fio != 'Без начальника' order by flag, fio, diameterCondit", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (16)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->fio = mAdo->readStr("fio");
			tmpExt->d = mAdo->read_double("diameterCondit");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");

			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->fio);
				fprintf(g, "<td>%f</td>", tmpExt->d);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}

void printSource(FILE* g, FILE* f, CAdoFile* mAdo, CString fragments) {
	struct Extension { /**/
		CString hs, hst;
		double p, o, g;
		Extension* next;
	};

	Extension ext;
	Extension* tmpExt = &ext;
	/*Заполняем данными из запроса*/
	g = print_doc(f, "ext_17.html", _TR("6.По источникам"));
	if (g) {
		CString q;

		q.Format("select * from get_pipe_laying_by_heatSource('%s') order by flag, hs_name", fragments);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе при формировании паспорта протяженности (17)");
			return;
		}

		bool extNotEmpty = false;
		while (!mAdo->isEOF()) {
			tmpExt->hs = mAdo->readStr("hs_name");
			tmpExt->hst = mAdo->readStr("hst_name");
			tmpExt->p = mAdo->read_double("Подача");
			tmpExt->o = mAdo->read_double("Обратка");
			tmpExt->g = mAdo->read_double("Общая");
			if (tmpExt->hs.IsEmpty())
				tmpExt->hs = "Без принадлежности";
			mAdo->MoveNext();
			if (mAdo->isEOF())
				tmpExt->next = nullptr;
			else
				tmpExt->next = new struct Extension;
			tmpExt = tmpExt->next;
			if (!extNotEmpty)
				extNotEmpty = true;
		}



		tmpExt = &ext;  /*Приравниваем указатель в начало структуры*/
		if (extNotEmpty)
		{
			while (tmpExt != nullptr) {
				fprintf(g, "<tr>");
				fprintf(g, "<td>%s</td>", tmpExt->hs);
				fprintf(g, "<td>%s</td>", tmpExt->hst);
				fprintf(g, "<td>%f</td>", tmpExt->p);
				fprintf(g, "<td>%f</td>", tmpExt->o);
				fprintf(g, "<td>%f</td>", tmpExt->g);
				fprintf(g, "</tr>");

				tmpExt = tmpExt->next;

			}
		}

		print_doc_end(g);
	}
}
