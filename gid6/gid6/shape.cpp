#include "stdafx.h"
#include "gid6.h"

#include "coordlis.h"

#include "graph2.h"

#include "ado.h"

void long_swp(long& l);

bool IsRezhim(const CNode2* n);
bool IsRezhim(const CLINE2* l);


//#include "Cxema.h"
//#include "Bgi.h"
#include "win.h"
//#include "shp_dbf.h"


void ShapeToCl(COleVariant& var, CCoordList& cl)
{
	//  is6 = 0;


	if (var.vt & VT_ARRAY) {
		COleSafeArray sa, sa1;

		sa = COleSafeArray((VARIANT)var);

		long r, r1, r2;

		DWORD dim = sa.GetDim();
		DWORD es = sa.GetElemSize();

		sa.GetLBound(1, &r1);
		sa.GetUBound(1, &r2);

		char* pb;

		sa.AccessData((LPVOID*)&pb);

		if (pb[1] != 0) {
			//      CodeDecode(r2-r1+1, &pb[r1]);
			//      isCod = 1;
		}

		if (pb[1] != 0) {
			//      throw 1;
		}

		int k = 0;

		r = r1;

		long l, n1, n2;
		double* d, x1, y1;
		double x, y;

		l = *(long*)&pb[r];  r += 4;


		if (l == 1) {
			x1 = *(double*)&pb[r];  r += sizeof(double);
			y1 = *(double*)&pb[r];  r += sizeof(double);

			cl.push_back(CFPoint(x1 * 100, -y1 * 100));

		}

		else if (l == 5 || l == 3 || l == 6) {
			/*
				  x1 = *(double *) &pb[r];  r += sizeof(double);
				  y1 = *(double *) &pb[r];  r += sizeof(double);
				  x2 = *(double *) &pb[r];  r += sizeof(double);
				  y2 = *(double *) &pb[r];  r += sizeof(double);
			*/

			r += sizeof(double) * 4;

			n1 = *(long*)&pb[r];  r += sizeof(long);
			n2 = *(long*)&pb[r];  r += sizeof(long);


			if (l == 6 && (n1 != 1 || n2 != 2)) {
				int qq;
				qq = 1;
				//        nl6 ++;
			}
			else {

				//        long qqq[256];
				long* qqq = new long[n1 + 1];

				int j;
				for (j = 0; j < n1; j++) {
					qqq[j] = *(long*)&pb[r];  r += sizeof(long);
				}
				qqq[j] = n2;

				if (n2) {

					d = (double*)&pb[r];  r += sizeof(double) * n2 * 2;

					for (j = 0; j < n1; j++) {
						for (int i = qqq[j]; i < qqq[j + 1]; i++) {
							x = d[i * 2] * 100.;
							y = d[i * 2 + 1] * 100.;

							if (x > 1000000000) {
								//                mx = x;
							}

							cl.push_back(CFPoint(x, -y));
						}
						if (j < n1 - 1) {
							cl.push_back(CFPoint(C_SPR, C_SPR));
						}
					}
				}
				delete[] qqq;
			}
		}



#if 0
		else if (l == 5 || l == 3/* || l == 6*/) {
			/*
				  x1 = *(double *) &pb[r];  r += sizeof(double);
				  y1 = *(double *) &pb[r];  r += sizeof(double);
				  x2 = *(double *) &pb[r];  r += sizeof(double);
				  y2 = *(double *) &pb[r];  r += sizeof(double);
			*/
			r += sizeof(double) * 4;

			n1 = *(long*)&pb[r];  r += sizeof(long);
			n2 = *(long*)&pb[r];  r += sizeof(long);

			long* qqq = new long[n1 + 1];
			int j;
			for (j = 0; j < n1; j++) {
				qqq[j] = *(long*)&pb[r];  r += sizeof(long);
			}
			qqq[j] = n2;

			if (n2) {

				d = (double*)&pb[r];  r += sizeof(double) * n2 * 2;

				for (j = 0; j < n1; j++) {
					for (int i = qqq[j]; i < qqq[j + 1]; i++) {
						x = d[i * 2] * 100.;
						y = d[i * 2 + 1] * 100.;
						cl.push_back(CFPoint(x, -y));
					}
					if (j < n1 - 1) {
						cl.push_back(CFPoint(C_SPR, C_SPR));
					}
				}
			}
			delete[] qqq;
		}
		else if (l == 6) {
			d = (double*)&pb[r];

			x = d[0] * 100.;
			y = d[1] * 100.;
			cl.push_back(CFPoint(x, -y));

			x = d[2] * 100.;
			y = d[3] * 100.;
			cl.push_back(CFPoint(x, -y));

			nl6++;
			is6 = 1;
		}
		else {
			nl99++;
			int qq = 0;
			qq = 0;
		}
#endif


		sa.UnaccessData();
	}
	//  cl.conv();
}


#include "cxema.h"
#include "TaskProgressDlg.h"

#include "shp_dbf.h"
#include "Klassif.h"


char* writeCl(const CCoordList& cl, int typ, long& ind)
{
	char* pb;

	pb = new char[cl.size() * sizeof(double) * 4 + 200];

	long n = 0;

	ind = 0;
	long l;
	n++;

	l = typ;
	*(long*)&pb[ind] = l; ind += sizeof(long);

	double x1, x2, y1, y2, x, y, x0, y0;

	CFRect r = cl.GetFRect();

	x1 = 0.01 * r.left;   y1 = -0.01 * r.bottom;
	x2 = 0.01 * r.right;  y2 = -0.01 * r.top;

	*(double*)&pb[ind] = x1; ind += sizeof(double);
	*(double*)&pb[ind] = y1; ind += sizeof(double);
	*(double*)&pb[ind] = x2; ind += sizeof(double);
	*(double*)&pb[ind] = y2; ind += sizeof(double);

	long n1 = 0, n2 = 1, * p1, * p2;

	CCoordList::const_iterator it, it2;

	p1 = (long*)&pb[ind];  *(long*)&pb[ind] = 1; ind += sizeof(long);
	p2 = (long*)&pb[ind];  *(long*)&pb[ind] = 1; ind += sizeof(long);

	*(long*)&pb[ind] = 0; ind += sizeof(long);

	for (it = cl.begin(); it != cl.end(); ++it) {
		if (isBreakCoord(it->x)) {
			if (typ == 5) n1++;
			*(long*)&pb[ind] = n1; ind += sizeof(long);
			(*p1)++;
		}
		else {
			n1++;
		}
	}
	if (typ == 5) n1++;

	(*p2) = n1;

	it = cl.begin();
	if (it != cl.end()) {
		x0 = 0.01 * it->x;
		y0 = -0.01 * it->y;
	}

	for (it = cl.begin(); it != cl.end(); ++it) {
		x = 0.01 * it->x;
		y = -0.01 * it->y;
		if (!isBreakCoord(it->x)) {
			*(double*)&pb[ind] = x; ind += sizeof(double);
			*(double*)&pb[ind] = y; ind += sizeof(double);
		}
		else {
			if (typ == 5) {
				*(double*)&pb[ind] = x0; ind += sizeof(double);
				*(double*)&pb[ind] = y0; ind += sizeof(double);

				it2 = it;
				while (it2 != cl.end() && isBreakCoord(it2->x)) it2++;
				if (it2 != cl.end()) {
					x0 = 0.01 * it2->x;
					y0 = -0.01 * it2->y;
				}
			}
		}
	}

	if (typ == 5) {
		*(double*)&pb[ind] = x0; ind += sizeof(double);
		*(double*)&pb[ind] = y0; ind += sizeof(double);
	}

	return pb;
}

void ExportSHAPE1(CAdoFile* ado, long nom, int* dbflen, long& n, long& len,
	FILE* f, FILE* fx, FILE* fd, CCoordList& cl, int typ)
{
	BOOL is_full = TRUE;
	int k;

	long l;
	n++;
	long_swp(n);
	fwrite(&n, sizeof(long), 1, f);
	long_swp(n);

	int size = cl.size();
	if (typ == 5) size += 1;

	l = 24 + size * 8;

	long_swp(len);
	fwrite(&len, sizeof(long), 1, fx);
	long_swp(len);

	long lpb;

	char* pb = writeCl(cl, typ, lpb);
	if (pb) {
		l = lpb / 2;
		len += l + 4;

		long_swp(l);
		fwrite(&l, sizeof(long), 1, f);
		fwrite(&l, sizeof(long), 1, fx);
		long_swp(l);

		fwrite(pb, lpb, 1, f);

		delete[] pb;
	}

	char d = ' ';
	fwrite(&d, sizeof(char), 1, fd);

	int nc = ado->NFlds();

	if (ado->GotoKey2(nom)) {
		for (k = 0; k < nc; k++) {
			char s[1024], ss[512];
			s[0] = 0;
			memset(ss, ' ', 255); ss[255] = 0;

			CString str = "";

			CAdoField fieldInfo = ado->Field(k);

			if (fieldInfo.typ != dbMemo && dbflen[k] < 256) {
				str = ado->readStr(k);
				strcpy(s, str);
			}

			strcat(s, ss);

			fwrite(s, sizeof(char), dbflen[k], fd);
			//  		fwrite((const char*)str, sizeof(char), dbflen[k], fd);
		}
	}
}

void ExportSHAPE2(CGraph2* graph, int typUS, const char* tn,

	CAdoFile* m_dbf, int* dbflen, long& n, long& len,
	FILE* f, FILE* fx, FILE* fd, CCoordList& cl, int typ)
{
	CMapGraph::const_iterator pp = graph->map_node.begin();

	CLINE2* l;
	CLine2* ll;
	CNode2* n1, * n2;

	Percent2 pc;

	if (typ == 1) {
		map<int, CNode2*> map_n;

		while (pp != graph->map_node.end()) {
			CNode2* p = pp->second;

			if (p->node.typ == typUS && p->node.internalNodeID == 0) {
				if (!IsRezhim(p)) {
					map_n[p->node.nomP] = p;
				}
			}
			pp++;
		}

		int ii = 0;
		int nn = map_n.size();

		for (const auto it1 : map_n) {
			int num = it1.first;
			const CNode2* p = it1.second;

			cl.clear();
			cl.push_back(p->node.coord);

			ExportSHAPE1(m_dbf, num, dbflen, n, len, f, fx, fd, cl, typ);

			pc.SetValue(ii * 100 / nn);
			ii++;

			//			if (ii > 100) break;
		}
	}
	else {
		map<int, CLINE2*> map_l;

		int nl1 = 0;

		pp = graph->map_node.begin();
		while (pp != graph->map_node.end()) {
			CNode2* p = pp->second;

			if (p->node.internalNodeID == 0) {
				for (l = p->lines; l; l = next(l)) {
					if (!IsBegin(l)) continue;

					ll = bline(l);

					if (ll->line.typ == typUS) {
						if (!IsRezhim(ll)) {
							n1 = where(l);
							n2 = other(l);

							if (n1->node.coord.x != 0 && n2->node.coord.x != 0) {
								int nn = ll->line.nomP > 0 ? ll->line.nomP : ll->line.nomO;
								//								int nn = ll->line.idP2 > 0 ? ll->line.idP2 : ll->line.idO2;
								map_l[nn] = ll;
								nl1++;
							}
						}
					}
				}
			}
			pp++;
		}

		int ii = 0;
		int nn = map_l.size();

		for (const auto it1 : map_l) {
			int num = it1.first;
			CLINE2* l = it1.second;
			CLine2* ll = bline(l);
			const CNode2* n1 = where(l);
			const CNode2* n2 = other(l);

			cl.clear();
			cl.push_back(n1->node.coord);

			CCoordList::const_iterator it = ll->line.m_NP.begin();
			for (; it != ll->line.m_NP.end(); ++it) {
				cl.push_back(*it);
			}
			cl.push_back(n2->node.coord);

			//			ExportSHAPE1(m_dbf, ll->line.nomP > 0 ? ll->line.nomP : ll->line.nomO, dbflen, n, len, f, fx, fd, cl, typ);
			ExportSHAPE1(m_dbf, num, dbflen, n, len, f, fx, fd, cl, typ);

			pc.SetValue(ii * 100 / nn);
			ii++;
			//			if (ii > 100) break;
		}
	}
}



void CCxema::ExportSHAPE(const char* fn, const char* tn, int typUS, int loc, const char* KOD)
{
	BOOL is_full = TRUE;
	FILE* f, * fx, * fd;
	char s[256];
	SHP_HEAD hd;
	DBASE_HEAD dbf;
	DBASE_FIELD df;
	CCoordList cl;
	int j, typ;

	CAdoFile* ado = getAdo(getGidAdoName());
	if (!ado) return;

	if (loc == LOC3) {
		typ = 5;
	}
	else if (loc == LOC2 || loc == LOC12) {
		typ = 3;
	}
	else if (loc == LOC1) {
		typ = 1;
	}
	else return;


	memset(&hd, 0, sizeof(hd));
	memset(&dbf, 0, sizeof(dbf));
	memset(&df, 0, sizeof(df));

	//  strcpy(s, tn); strcat(s, ".prj");
	//  f = fopen(s, "wb");
	//  fprintf(f, "PROJCS[\"Pulkovo_1942_GK_Zone_14\",GEOGCS[\"GCS_Pulkovo_1942\",DATUM[\"D_Pulkovo_1942\",SPHEROID[\"Krasovsky_1940\",6378245,298.3]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Gauss_Kruger\"],PARAMETER[\"False_Easting\",14500000],PARAMETER[\"False_Northing\",0],PARAMETER[\"Central_Meridian\",81],PARAMETER[\"Scale_Factor\",1],PARAMETER[\"Latitude_Of_Origin\",0],UNIT[\"Meter\",1]]");
	//  fclose(f);

	sprintf(s, "%s_%s.shp", fn, tn);
	f = fopen(s, "wb");
	if (!f) return;

	sprintf(s, "%s_%s.shx", fn, tn);
	fx = fopen(s, "wb");
	if (!fx) return;

	sprintf(s, "%s_%s.dbf", fn, tn);
	fd = fopen(s, "wb");
	if (!fd) return;

	fwrite(&dbf, sizeof(dbf), 1, fd);


	int len_rec = 0;
	int dbflen[256];
	int isn[256];

	//  CDaoFieldInfo fieldInfo;
	//  CDbFile *m_dbf = gid_file->m_dbf;

	CString q;

	if (typ == 2) {
		q.Format("SELECT * FROM linesObj", tn);
	}
	else {
		q.Format("SELECT * FROM nodes", tn);
	}

	CString ss;
	ss.Format("%s\\sql\\%s.sql", argpath(), KOD);

	q = readFile(ss);

	q.Replace("n.fileID=$fileID$ AND ", "");
	q.Replace("n1.fileID=$fileID$ AND ", "");


	CString readQ(CAdoFile * ado, const char* tn, const char* tn2, const char* fileID);
	CString readQ(CAdoFile * ado, const char* tn, const char* tn2, int fileID);

//	q = readQ(ado, KOD, tn, -1);
	q = readQ(ado, KOD, tn, m_par);


	if (typ == 3) {
		q += " ORDER BY l.id";
	}


	/*
		switch (typUS) {
		case TIP_UT:
			q.Format("SELECT * FROM linesObj", tn);
			break;
		case TIP_US:
			break;
		case TIP_PR:
			q.Format("SELECT * FROM nodes", tn);
			break;
		case TIP_PO:
			q.Format("SELECT * FROM nodes", tn);
			break;
		}
	*/

	if (!ado->openQ(q, adOpenKeyset, adLockReadOnly, adCmdText)) return;


	//	if (!ado->openTable0(q)) return;


	int nf = ado->NFlds();

	for (j = 0; j < nf; j++) {
		//  		ado->rs->GetFieldInfo(j, fieldInfo);

		CAdoField fieldInfo = ado->Field(j);

		strncpy(df.name, fieldInfo.name, 10);
		df.name[10] = 0;
		isn[j] = 0;

		df.length = fieldInfo.DefinedSize;

		switch (fieldInfo.typ) {
		case adVarWChar:
			df.type = 'C';
			break;
		case adDate:
		case adDBDate:
			df.type = 'D';
			break;
		default:
			df.type = 'N';
			break;
		}

		if (fieldInfo.typ == adInteger) df.length = 7;
		if (fieldInfo.typ != adVarWChar) isn[j] = 1;

		/**
				if (is_full && ado->IsMenu(j+1)) {
				  df.type = 'C';
				  df.length = 100;
				}
		*/

		len_rec += df.length;
		df.dec_point = fieldInfo.typ == adDouble ? 2 : 0;
		fwrite(&df, sizeof(dbf), 1, fd);
		dbflen[j] = df.length;
	}

	char d = 0xD;
	fwrite(&d, sizeof(char), 1, fd);

	hd.File_Code = 9994;
	long_swp(hd.File_Code);

	hd.File_Length = 0;
	hd.Version = 1000;
	hd.Shape_Type = typ;

	CFRect rectBorder = m_graph->getBorder(0);

	hd.Xmin = rectBorder.left * 0.01;
	hd.Xmax = rectBorder.right * 0.01;
	hd.Ymin = -rectBorder.bottom * 0.01;
	hd.Ymax = -rectBorder.top * 0.01;

	fwrite(&hd, 100, 1, f);
	fwrite(&hd, 100, 1, fx);

	long len = 50;

	long n = 0;


	ExportSHAPE2(m_graph, typUS, tn, ado, dbflen, n, len, f, fx, fd, cl, typ);

	fseek(f, 0, 0);
	hd.File_Length = len;
	long_swp(hd.File_Length);
	fwrite(&hd, 100, 1, f);

	fseek(fx, 0, 0);
	hd.File_Length = 50 + 4 * n;
	long_swp(hd.File_Length);
	fwrite(&hd, 100, 1, fx);

	d = 0x1a;
	fwrite(&d, sizeof(char), 1, fd);

	fseek(fd, 0, 0);
	dbf.version = 3;
	dbf.count = n;


	//	int nf = 2;


	dbf.header = 32 + 32 * (nf - 0) + 1;


	dbf.lrecl = len_rec + 1;
	fwrite(&dbf, sizeof(dbf), 1, fd);

	fclose(f);
	fclose(fx);
	fclose(fd);
}
