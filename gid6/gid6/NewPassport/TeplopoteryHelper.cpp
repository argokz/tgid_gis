#include "stdafx.h"
#include "ado.h"
#include "win.h"
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>

using nlohmann::json;


/*Метод для получения Header в таблицах нормы тепловых потерь*/
CString getHeaderTeplopotery(int id, CAdoFile* mAdo, BOOL init)
{
	CString jsonTP, fStr;

	fStr.Format("%steplopotery\\headers\\%i.json", argpath(), id);
	ifstream f(fStr);
	if (f.good())
	{
		jsonTP = readFile(fStr);

		/*Запрос для header*/
		CString q;
		fStr.Format("%steplopotery\\headers\\%i.sql", argpath(), id);
		ifstream f(fStr);
		if (f.good())
		{
			q = readFile(fStr);
			bool ret = mAdo->openTable0(q);
			if (!ret)
			{
				AfxMessageBox("Ошибка в запросе Header для таблицы Нормы тепловых потерь");
				return "";
			}

			while (!mAdo->isEOF()) {
				for (int i = 0; i < mAdo->NFlds(); i++)
				{

					int data = mAdo->read_long(i);
					CString d1, d2;
					d1.Format("$data%i$", i + 1);
					d2.Format("%i", data);
					jsonTP.Replace(d1, d2);
				}
				break;
			}
		}
		else
			return "";
		/*Запрос для header*/
		if (init) {
			jsonTP = "let " + jsonTP + " window.MainComponent.$store.commit(\"setCustomHeaderList\",customHeaderList);";
		}
		else
			jsonTP += " window.MainComponent.$store.commit(\"setCustomHeaderList\",customHeaderList);";
		return jsonTP;
	}
	return "";
}

CString getSpecialHeader(CString tblName, BOOL init) {
	CString jsonTP, fStr;

	fStr.Format("%scustom_headers\\%s.json", argpath(), tblName);
	ifstream f(fStr);
	if (f.good())
	{
		jsonTP = readFile(fStr);


		if (init) {
			jsonTP = "let " + jsonTP + " window.MainComponent.$store.commit(\"setCustomHeaderList\",customHeaderList);";
		}
		else
			jsonTP += " window.MainComponent.$store.commit(\"setCustomHeaderList\",customHeaderList);";
		return jsonTP;
	}
	return "";
}

/*Метод для обновления данных в таблицах нормы тепловых потерь*/
int updateTeplopotery(int id, BSTR jsonString, CAdoFile* mAdo)
{
	CString q, fStr;
	fStr.Format("%steplopotery\\scripts\\update_%i.sql", argpath(), id);
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		/*Чтение данных с json строки и замена в запросе данных*/
		std::wstring ws(jsonString, SysStringLen(jsonString));

		auto myJson = json::parse(ws);
		int countFields = myJson.size();

		for (long i = 0; i < countFields; i++) {
			string title = "col_" + std::to_string(i);
			auto j1 = myJson[title];
			if (!j1.is_null()) {


				if (j1.is_number_integer()) {
					int data = j1.get<int>();
					CString d1, d2;
					d1.Format("$col_%i$", i);
					d2.Format("%i", data);
					q.Replace(d1, d2);

				}
				else
					if (j1.is_string()) {
						string w_value = j1.get<std::string>();
						CString d1;
						CString d2 = w_value.c_str();
						d1.Format("$col_%i$", i);
						q.Replace(d1, d2);
					}

			}
		}


		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе Update для таблицы Нормы тепловых потерь");
			return 404;
		}
		else
			return 200;
	}
	else
		return 404;
}


/*Метод для добавления данных в таблицах нормы тепловых потерь*/
int insertTeplopotery(int id, BSTR jsonString, CAdoFile* mAdo)
{
	CString q, q1, fStr;
	fStr.Format("%steplopotery\\scripts\\insert_%i.sql", argpath(), id);
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		/*Чтение данных с json строки и замена в запросе данных*/
		std::wstring ws(jsonString, SysStringLen(jsonString));

		auto myJson = json::parse(ws);
		int countFields = myJson.size();

		for (long i = 0; i < countFields; i++) {
			string title = "col_" + std::to_string(i + 1);
			auto j1 = myJson[title];

			if (!j1.is_null()) {


				if (j1.is_number_integer()) {
					int data = j1.get<int>();
					CString d1, d2;
					d1.Format("$col_%i$", i + 1);
					d2.Format("%i", data);
					q.Replace(d1, d2);

				}
				else
					if (j1.is_string()) {
						string w_value = j1.get<std::string>();
						CString d1;
						CString d2 = w_value.c_str();
						d1.Format("$col_%i$", i + 1);
						q.Replace(d1, d2);
					}

			}
		}


		fStr.Format("%steplopotery\\scripts\\select_insert_%i.sql", argpath(), id);
		ifstream f(fStr);
		if (f.good())
		{
			q1 = readFile(fStr);
			bool ret = mAdo->openTable0(q1);

			if (!ret)
			{
				AfxMessageBox("Ошибка в запросе Select Insert для таблицы Нормы тепловых потерь");
				return -1;
			}

			while (!mAdo->isEOF()) {
				for (int i = 0; i < mAdo->NFlds(); i++)
				{

					double data = mAdo->read_double(i);
					CString d1, d2;
					d1.Format("$col_d%i$", i + 1);
					d2.Format("%f", data);
					q.Replace(d1, d2);
				}
				break;
			}
		}

		long id = mAdo->ExecuteInsert(q, &id);
		if (!id)
		{
			AfxMessageBox("Ошибка в запросе Insert для таблицы Нормы тепловых потерь");
			return -1;
		}
		else
		{
			long id2 = mAdo->read_long(0);
			return id;
		}

	}
	else
		return -1;
}



/*Метод для проверки существования пользовательских данных в таблице нормы тепловых потерь*/
int insertTeplopoteryDefault(CAdoFile* mAdo)
{
	CString q, q1, fStr;

	q1 = "select top 1 * from [39_normy_teplovyh_poter] where mode = 1";
	bool ret = mAdo->openTable0(q1);

	if (!ret)
	{
		AfxMessageBox("Ошибка в запросе Select Insert для таблицы Нормы тепловых потерь");
		return -1;
	}

	//если пользовательских данных нет, запустить скрипт который добавит по умолчанию
	if (mAdo->isEOF()) {
		//запуск скрипта для создания пользовательских данных
		CString fStr2;
		fStr2.Format("%steplopotery\\scripts\\custom_39_normy_teplovyh_poter.sql", argpath());
		ifstream f(fStr2);
		CString q2 = readFile(fStr2);
		ret = mAdo->openTable0(q2);

		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе Create Default Tables для таблицы Нормы тепловых потерь");
			return -1;
		}
	}


	return 0;

}

/*Метод для удаления данных в таблицах нормы тепловых потерь*/
pair<int, CString> deleteTeplopotery(int id, int idColumn, CAdoFile* mAdo)
{
	CString q, q1, fStr;
	fStr.Format("%steplopotery\\scripts\\delete_teplopotery.sql", argpath());
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);
		CString d1;
		d1.Format("%i", idColumn);
		q.Replace("$data", d1);

		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе Delete для таблицы Нормы тепловых потерь");
			return std::make_pair(404, "Ошибка удаления");
		}
		else
			return std::make_pair(200, "Успешно удалено");;
	}
	else
		return std::make_pair(404, "Ошибка удаления");
}



/*Метод для обновления данных в header нормы тепловых потерь*/
int updateHeaderTeplopotery(int id, BSTR jsonString, CAdoFile* mAdo)
{
	CString q, fStr;
	fStr.Format("%steplopotery\\scripts\\update_h_%i.sql", argpath(), id);
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		/*Чтение данных с json строки и замена в запросе данных*/
		std::wstring ws(jsonString, SysStringLen(jsonString));

		auto myJson = json::parse(ws);
		int countFields = myJson.size();

		for (long i = 0; i < countFields; i++) {
			string title = "h_col_" + std::to_string(i + 1);
			auto j1 = myJson[title];
			if (!j1.is_null()) {


				if (j1.is_number_integer()) {
					int data = j1.get<int>();
					CString d1, d2;
					d1.Format("$h_col_%i$", i + 1);
					d2.Format("%i", data);
					q.Replace(d1, d2);

				}
				else
					if (j1.is_string()) {
						string w_value = j1.get<std::string>();
						CString d1;
						CString d2 = w_value.c_str();
						d1.Format("$h_col_%i$", i + 1);
						q.Replace(d1, d2);
					}

			}
		}


		bool ret = mAdo->openTable0(q);
		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе Update Header для таблицы Нормы тепловых потерь");
			return 404;
		}
		else
			return 200;
	}
	else
		return 404;
}



/*Метод генерации файла заголовка для Excel в таблицах нормы тепловых потерь*/
CString getExcelHeaderTeplopotery(int id, CAdoFile* mAdo)
{
	CString jsonTP, fStr;

	fStr.Format("%steplopotery\\excel\\table_%i.html", argpath(), id);
	ifstream f(fStr);
	if (f.good())
	{
		jsonTP = readFile(fStr);

		/*Запрос для header*/
		CString q;
		fStr.Format("%steplopotery\\headers\\%i.sql", argpath(), id);
		ifstream f(fStr);
		if (f.good())
		{
			q = readFile(fStr);
			bool ret = mAdo->openTable0(q);
			if (!ret)
			{
				AfxMessageBox("Ошибка в запросе Header для таблицы Нормы тепловых потерь");
				return "";
			}

			while (!mAdo->isEOF()) {
				for (int i = 0; i < mAdo->NFlds(); i++)
				{

					int data = mAdo->read_long(i);
					CString d1, d2;
					d1.Format("$data%i$", i + 1);
					d2.Format("%i", data);
					jsonTP.Replace(d1, d2);
				}
				break;
			}
		}
		else
			return "";
		/*Запрос для header*/
		//Создать и сохранить файл в tmp и передать ссылку
		CString tmpName;
		CString resultName;
		tmpName.Format("%steplopotery\\excel\\table_tmp_%i.html", argpath(), id);
		resultName.Format("teplopotery\\excel\\table_tmp_%i.html", id);
		FILE* ff = fopen(tmpName, "w");
		fprintf(ff, jsonTP);
		fclose(ff);
		return resultName;
	}
	return "";
}



/*Метод для сброса пользовательских норм по умолчанию*/
int executeScriptTeplopotery(CAdoFile* mAdo, CString fileName)
{
	CString q, fStr;
	fStr.Format("%steplopotery\\scripts\\%s.sql", argpath(), fileName);
	ifstream f(fStr);
	if (f.good())
	{
		q = readFile(fStr);

		bool ret = mAdo->Execute(q);

		if (!ret)
		{
			AfxMessageBox("Ошибка в запросе Cброс таблицы Нормы тепловых потерь");
			return -1;
		}
		return 0;

	}
	else
		return -1;
}