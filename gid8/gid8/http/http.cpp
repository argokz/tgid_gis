#include <QApplication>
#include <QtGui>

#include <mainwindow.h>
#include <gidview/GidWidget.h>
//#include <QtHttpServer>
#include <db/db.h>
#include <lib/json.h>

#if 0

#include "Webserver_dialog2Dialog.h"

static QString m_tiles = "";


QString getTiles()
{
    return m_tiles;
}

void setTiles(const QString & t)
{
    m_tiles = t;
}


enum LAYERS {
    L_ALL = 0,
    L_GID = 1,
    L_GEO,
    L_PTS,
    L_ACT,
    L_IND,
    L_DEFECT,
    L_REMONT,
    L_SHURFY,
    L_DIAG,
    L_OPRESSOVKA_SEKTSIONIRUYUSCHAYA_ZADVIZHKA,
};


struct HTMLLayers {
  long id;
  QString code;
  QString name;
};


static HTMLLayers layers[] =
{
    L_GID, "gid", "Гидравлика",
    L_GEO, "geo", "Геобаза",
    L_PTS, "pts", "ПТС",
    L_ACT, "act", "",
    L_IND, "indikator_korrozii", "",
    L_DEFECT, "defect", "",
    L_REMONT, "remont", "",
    L_SHURFY, "shurfy", "",
    L_DIAG, "diag", "",
    L_OPRESSOVKA_SEKTSIONIRUYUSCHAYA_ZADVIZHKA, "opressovka_sektsioniruyuschaya_zadvizhka", "",
};


std::vector<int> ParseStringToNumbers(const QString& str)
{
    std::vector<int> numbers;
/*
    QString delimiter = _T(",");
    int start = 0;

    QString token = str.Tokenize(delimiter, start);
    while (token != _T(""))
    {
        int number = _ttoi(token);  // Преобразуем строку в целое число
        numbers.push_back(number);  // Добавляем число в вектор
        token = str.Tokenize(delimiter, start);  // Получаем следующий токен
    }
*/
    return numbers;
}


QString row_col_name(int r, int c)
{
    QString ss = QString("%1/%2").arg(c).arg(r);
    return ss;
}


QString tileName(int m, int r, int c, int internalNodeID, int layer, int gid, int geo, int ms, int rs)
{
    int id = 0;
    QString filename;
/*
    string m_tiles = getTiles();

    if (m_tiles == "") {
        char path1[512];
        HRESULT ret = SHGetSpecialFolderPath(NULL,path1,CSIDL_APPDATA, 1);
        m_tiles = QString(path1)+"\\Sirius\\tgid\\tiles\\";
        setTiles(m_tiles.c_str());
    }
*/

    QString gidgeo = "";

    if (gid) gidgeo += "gid";
    if (geo) gidgeo += "geo";

    QString row_col = row_col_name(r, c);

    if (layer > 0) {
        gidgeo = "error";
        for (int i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
            if (layers[i].id == layer) {
                gidgeo = layers[i].code;
            }
        }
    }


    filename = QString("%1tiles/%2/%3/%4/%5.png").arg(argpath_2()).arg(gidgeo).arg(internalNodeID).arg(m).arg(row_col);


#if 0
    if ((ms > 0 || rs > 0) && (layer == L_ALL || layer == L_GID)) {
        QString ms_rs = "";
        CFRect r0;

        if (ms > 0) {
            id = ms;
            ms_rs = "ms";
            r0 = getMsRect(ms);
        }
        else {
            id = rs;
            ms_rs = "rs";
            r0 = getRsRect(rs);
        }

        CFRect rect = gidr_view->getMaxRect();
        double dmas = DMAS96;

        double masx, masy, alpha;
        CFRect rr;

        tile_coord(1, 1, dmas, r, c, r, masx, masy, alpha, rr);

        CFRect rrr;

        if (rrr.IntersectRect(rr, r0)) {
            filename.Format("%s%s/%s/%d/%d/%d/%d/%s.png", m_tiles.c_str(), gidgeo, ms_rs, id, geo, internalNodeID, m, row_col);
        }
    }
#endif

    return filename;
}


#if 0
QString tileName(int m, int r, int c, int internalNodeID, int geo, int ms, int rs)
{
    int id = 0;
    QString m_tiles = getTiles();

    if (m_tiles == "") {
        setTiles(argpath_2());
    }

    QString filename = QString("%1%2/%3/%4/%5/%6.png").arg(m_tiles).arg(geo).arg(internalNodeID).arg(m).arg(r).arg(c);

#if 0
    if (ms > 0 || rs > 0) {

        QString ms_rs = "";

        CFRect r0;

        if (ms > 0) {
            id = ms;
            ms_rs = "ms";
            r0 = getMsRect(ms);
        }
        else {
            id = rs;
            ms_rs = "rs";
            r0 = getRsRect(rs);
        }

        CFRect rect = gidr_view->getMaxRect();
        double dmas = gidr_view->dmas;

        double mas = 200000;

        for (int i = 1; i < m; i++) {
            mas /= mas_coef;
        }

        double x = rect.left + mas * c*DD / dmas;
        double y = rect.top + mas * r*DD / dmas;

        CFRect rr;

        rr.left = (mas * c*DD ) / 1000000;
        rr.top = (mas * r*DD) / 1000000;

        rr.right = rr.left + mas*DD / 1000000;
        rr.bottom = rr.top + mas*DD / 1000000;

        CFRect rrr;

        if (rrr.IntersectRect(rr, r0)) {
            filename = QString("%1%2/%3/%4/%5/%6/%7/%8.png").arg(m_tiles).arg(ms_rs).arg(id).arg(geo).arg(internalNodeID).arg(m).arg(r).arg(c);
        }
    }
    else {
        filename = QString("%s%d/%d/%d/%d/%d.png").arg(m_tiles).arg(geo).arg(internalNodeID).arg(m).arg(r).arg(c);
    }
#endif
    return filename;
}
#endif


QString GidWidget::http_route(const QString & param)
{

    QString style = readFileUTF8("qss/style.qss");

    return "Hello world";
}

QString GidWidget::http_get_q(const QString & db, const QString & q, qint32 id1, qint32 id2)
{

//    return QString("%1 %2 %3 %4").arg(db, q).arg(id1).arg(id2);

    JSON json;

    return q;

#if 0

//    QString getTableNameFromSQL(const QString & q);
//    QString tn = getTableNameFromSQL(q);

//    QString tnr = findTableRusNameGid(tn);

//    if (tn == "nodes") tnr = _TR("Узел");

//    Klassif *kls = m_geofile->m_kl_list->findKlN(tn);
//    if (kls) {
//        tnr = kls->RusName();
//    }

//    MySQLColumns *col = ado->findMySQLColumns(tn, "id");
//    MySQLColumns2 *col2 = ado->findMySQLColumns2(tnr, "id");

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (query_exec(db, query, q)) {
        json.add("{");
        json.add("\"data\" : [");

        int kk = 0;

        QSqlRecord rec = query.record();

        while (query.next()) {

            kk++;

            json.add("{");

//            QString strID = ado->readStr(0);

            int nf = rec.count();

            for (long i = 0; i < nf; i++) {
                QString str = query.value(i).toString();
//                CAdoField f = ado->Field(i);

                QString name = rec.fieldName(i);
                QString title = name;

//                MySQLColumns *col = ado->findMySQLColumns(tn, name);
//                MySQLColumns2 *col2 = ado->findMySQLColumns2(tnr, name);


//                MySQLColumns2* colOld = ado->findMySQLOldColumns(tnr, name);


//                QString getFullName(const QString & cx, const QString & tn, const QString & fn);

                
//                QString fName = getFullName("AstanaGID", tn, name);

/*
                if (!col || col->strDATA_TYPE != "geometry") {
                    if (rus_name) {

                        if (colOld) {
                            if (colOld->nm != "")  title = colOld->nm;
                            if (colOld->stand_razmer != "") {
                                title += ", ";
                                title += colOld->stand_razmer;
                            }
                        }

                        if (col) {
                            if (col->strCOLUMN_COMMENT != "") {
                                title = col->strCOLUMN_COMMENT;
                            }
                        }

                        if (col2) {
                            title = col2->hlp;
                        }
                    }
                    if (f.typ != adLongVarBinary) {
                        title.replace(".", " ");
                        json.add(title, (const char *)str);
                    }
                }
*/
            }
            json.add("}");

        }

        json.add("]");
        json.add("}");
        return json.str();
    }

    json.add("{");
    json.add("\"data\" : [");
    json.add("]");
    json.add("}");
#endif
    return json.str();
}



QString GidWidget::http_get_config()
{
    JSON json;

    json.add("{");
    json.add("db", getDatabaseName(m_cxema.m_db));
    json.add("ps", getDatabaseName(m_cxema.m_db));
    json.add("}");

    return json.str();
}



void CWebserver_dialog2Dialog::onStart() {
//    QMessageBox::warning(this, "", "Start");
    read();

    httpServer.route("/", [&]() {
        qDebug() << "/";
        return gid->http_route("/");
    });

    httpServer.route("/get_q", [&](const QHttpServerRequest &request) {
        auto req = request.query();

        QString db = req.queryItemValue("db");
        QString q = req.queryItemValue("q");
        int id1 = req.queryItemValue("id1").toInt();
        int id2 = req.queryItemValue("id2").toInt();
        return gid->http_get_q(db, q, id1, id2);
    });

    httpServer.route("/get_config", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return gid->http_get_config();
    });


    httpServer.route("/add_object", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        double x = req.queryItemValue("x").toDouble();
        double y = req.queryItemValue("y").toDouble();
        double mas = req.queryItemValue("mas").toDouble();
        QString table = req.queryItemValue("table");

        return "";
    });

    httpServer.route("/delete_form", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString table = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
//        QString data = body;
        return "";
    });

    httpServer.route("/delete_object", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString table = req.queryItemValue("table");
        QString file = req.queryItemValue("db");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/edit_field", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString table = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/F7", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/find", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        double x = req.queryItemValue("x").toDouble();
        double y = req.queryItemValue("y").toDouble();
        double mas = req.queryItemValue("mas").toDouble();
        int internalNodeID = req.queryItemValue("internalNodeID").toInt();
        int geo = req.queryItemValue("geo").toInt();
        return "";
    });

    httpServer.route("/find_addr2", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString pat = req.queryItemValue("pat");
        return "";
    });

    httpServer.route("/find_dom", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString pat = req.queryItemValue("pat");
        return "";
    });

    httpServer.route("/find_street", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString pat = req.queryItemValue("pat");
        return "";
    });

    httpServer.route("/find_geo", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString file = req.queryItemValue("db");
        QString table = req.queryItemValue("table");
        int ID = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/find_line", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString file = req.queryItemValue("db");
        QString table = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/find_node", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString file = req.queryItemValue("db");
        QString table = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/getFile", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString file = req.queryItemValue("name");
        QString path0 = req.queryItemValue("path");
//        QString path = getRealPath(path0.c_str())+file.c_str();
        return "";
    });

    httpServer.route("/get_address", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/get_dropdown", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString file = req.queryItemValue("db");
        QString q = req.queryItemValue("id");
        return "";
    });

    httpServer.route("/get_file_q", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString db = req.queryItemValue("db");
        QString file = req.queryItemValue("file");
        int id1 = req.queryItemValue("id1").toInt();
        int id2 = req.queryItemValue("id2").toInt();
        return "";
    });

    httpServer.route("/get_form", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString table = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_fragments", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/get_list_analyse", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString db = req.queryItemValue("db");
        QString code = req.queryItemValue("code");
        QString table = req.queryItemValue("table");
        return "";
    });

    httpServer.route("/get_ms_rs", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString type = req.queryItemValue("type");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_node_id", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_node_xy", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        double x = req.queryItemValue("x").toDouble();
        double y = req.queryItemValue("y").toDouble();
        double mas = req.queryItemValue("mas").toDouble();
        int internalNodeID = req.queryItemValue("internalNodeID").toInt();
        return "";
    });

    httpServer.route("/get_rect", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        int internalNodeID = req.queryItemValue("internalNodeID").toInt();
        return "";
    });

    httpServer.route("/get_screen", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/get_table", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString db = req.queryItemValue("db");
        QString q = req.queryItemValue("table");
        QString type = req.queryItemValue("type");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_table_analiz_list", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/get_table_gid", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString db = req.queryItemValue("db");
        QString code = req.queryItemValue("code");
        QString table = req.queryItemValue("table");
        int id1 = req.queryItemValue("id1").toInt();
        int id2 = req.queryItemValue("id2").toInt();
        return "";
    });

    httpServer.route("/get_table_gid_position", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString db = req.queryItemValue("db");
        QString q = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_table_list", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/get_table_out", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString db = req.queryItemValue("db");
        QString code = req.queryItemValue("code");
        QString table = req.queryItemValue("table");
        int id1 = req.queryItemValue("id1").toInt();
        int id2 = req.queryItemValue("id2").toInt();
        return "";
    });

    httpServer.route("/get_table_out_list", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/get_table_position", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString db = req.queryItemValue("db");
        QString q = req.queryItemValue("table");
        QString type = req.queryItemValue("type");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_tg_graph", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_tg_table", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/get_tiles_names", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        int m = req.queryItemValue("m").toInt();
        int x1 = req.queryItemValue("x1").toInt();
        int x2 = req.queryItemValue("x2").toInt();
        int y1 = req.queryItemValue("y1").toInt();
        int y2 = req.queryItemValue("y2").toInt();

        int internalNodeID = req.queryItemValue("internalNodeID").toInt();
        int geo = req.queryItemValue("geo").toInt();

        return "";
    });

    httpServer.route("/get_tree_geo", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/get_tree_gid", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/gps", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        double x = req.queryItemValue("x").toDouble();
        double y = req.queryItemValue("y").toDouble();
        return "";
    });

    httpServer.route("/move_object", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        double x = req.queryItemValue("x").toDouble();
        double y = req.queryItemValue("y").toDouble();
        double mas = req.queryItemValue("mas").toDouble();
        QString table = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/openFile", [&](const QHttpServerRequest &request) {
        return "";
    });

    httpServer.route("/pro", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString file = req.queryItemValue("db");
        QString table = req.queryItemValue("table");
        int ID = req.queryItemValue("ID").toInt();
        QString login = req.queryItemValue("login");
        QString login2 = req.queryItemValue("login2");
        QString action = req.queryItemValue("action");
        QString query = req.queryItemValue("query");

        if (ID == 0) {
            ID = req.queryItemValue("id").toInt();
        }


//        file = http_get_file(file.c_str(), table.c_str());
        return "";
    });

    httpServer.route("/setFields", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString j = req.queryItemValue("data");
        return "";
    });

    httpServer.route("/set_form", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString table = req.queryItemValue("table");
        int id = req.queryItemValue("id").toInt();
//        QString data = body;
        return "";
    });

    httpServer.route("/set_fragment", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        int id = req.queryItemValue("id").toInt();
        return "";
    });

    httpServer.route("/test", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/tiles", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        int ms = req.queryItemValue("ms").toInt();
        int rs = req.queryItemValue("rs").toInt();

        int m = req.queryItemValue("m").toInt();
        int r = req.queryItemValue("row").toInt();
        int c = req.queryItemValue("col").toInt();
        int internalNodeID = req.queryItemValue("internalNodeID").toInt();
        int layer = req.queryItemValue("layer").toInt();

        int gid = req.queryItemValue("gid").toInt();
        int geo = req.queryItemValue("geo").toInt();

        QString filename = tileName(m, r, c, internalNodeID, layer, gid, geo, ms, rs);

//        filename = "C:/Users/gena1/AppData/Roaming/Sirius/tgid/tiles/geo/0/15/22882/10941.png";

        bool http_make_png(const QString &fn1, int zz, int yy, int xx, int internalNodeID, int layer, int n_layers, int *layers, int gid, int geo, int ms, int rs);

        http_make_png(filename, m, r, c, internalNodeID, layer, 0, nullptr, gid, geo, ms, rs);

        return QHttpServerResponse::fromFile(filename);

//        return filename;
    });

    httpServer.route("/tree_geo", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        return "";
    });

    httpServer.route("/tree_gid", [&](const QHttpServerRequest &request) {
        return "";
    });

    httpServer.route("/tree_ms", [&](const QHttpServerRequest &request) {
        return "";
    });

    httpServer.route("/tree_rs", [&](const QHttpServerRequest &request) {
        return "";
    });

    httpServer.route("/update", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString j = req.queryItemValue("j");
        return "";
    });

    httpServer.route("/uploadFile", [&](const QHttpServerRequest &request) {
        return "";
    });

    httpServer.route("/uploadFileForm", [&](const QHttpServerRequest &request) {
        return "";
    });

    httpServer.route("/validate_user", [&](const QHttpServerRequest &request) {
        auto req = request.query();
        QString login = req.queryItemValue("login");
        QString password = req.queryItemValue("password");
//        QString message = http_validate_user(login.c_str(), password.c_str());
        return "";
    });


    httpServer.afterRequest([](QHttpServerResponse &&resp) {
        resp.setHeader("Server", "Tgid HTTP Server");
        return std::move(resp);
    });

    const auto port = httpServer.listen(QHostAddress::Any, m_iPort);
    if (!port) {
        QMessageBox::warning(this, "", tr("Server failed to listen on a port."));
        return;
    }
}
void CWebserver_dialog2Dialog::onStop() {
    QMessageBox::warning(this, "", "Stop");
}
void CWebserver_dialog2Dialog::onReset() {
    QMessageBox::warning(this, "", "Reset");
}
void CWebserver_dialog2Dialog::onHomedirbrowse() {
    QMessageBox::warning(this, "", "Homedirbrowse");
}
void CWebserver_dialog2Dialog::onPrivatebrowse() {
    QMessageBox::warning(this, "", "Privatebrowse");
}
void CWebserver_dialog2Dialog::onPublicbrowse() {
    QMessageBox::warning(this, "", "Publicbrowse");
}
void CWebserver_dialog2Dialog::onTile_dir_browse() {
    QMessageBox::warning(this, "", "Tile_dir_browse");
}
void CWebserver_dialog2Dialog::onWeb_dir_browse() {
    QMessageBox::warning(this, "", "Web_dir_browse");
}

#endif

void GidWidget::onHttp2() // Веб-сервер
{
#if 0
    CWebserver_dialog2Dialog *dlg = new CWebserver_dialog2Dialog(this);
    dlg->show();
#endif
}
