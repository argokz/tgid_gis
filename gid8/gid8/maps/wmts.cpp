#include <QtGui>
#include <QMessageBox>
#include <gidview/GidWidget.h>
#include <algorithm>
#include <math.h>
#include <QPointF>
#include <QtXml>

#include <dialog/ChMenuDial.h>

QString getWMS_File_Name(const QString & r);
QString get_wms_xml_name(const QString & r);

QString getWMS_File_Name(const QString & r);

bool GidWidget::read_wmts_ask(const QString & txt)
{
    if (txt == "") return false;

//    MMenuDial menu(this, QObject::tr("Выберите слой"));
//    MMenuDial menu(this, QObject::tr("Выберите слой"));
    ChMenuDial dlg(this, QObject::tr("Выберите слои"), true);

    QDomDocument dom;
    dom.setContent(txt);

//    QDomNodeList layers = dom.elementsByTagName("WMS_Capabilities")
//            .item(0).toElement().elementsByTagName("Capability")
//            .item(0).toElement().elementsByTagName("Layer");

//    QDomNodeList layers = dom.firstChildElement("Capabilities").firstChildElement("Contents")
//            .elementsByTagName("Layer");

    QDomNodeList layers = dom.firstChildElement("Capabilities").firstChildElement("Contents")
               .elementsByTagName("Layer");

    QSettings settings;

    for (int i = 0; i < layers.count(); ++i) {
        QDomNode layer = layers.item(i);
//        QDomNode nm = layer.firstChildElement("ows:Title");
        QDomNode nm = layer.firstChildElement("ows:Identifier");
        

        if (!nm.isNull()) {
            QDomNode box = layer.firstChildElement("ows:WGS84BoundingBox");
            if (!box.isNull()) {
                Layer layer;

                QString lo = box.firstChildElement("ows:LowerCorner").firstChild().nodeValue();
                QString up = box.firstChildElement("ows:UpperCorner").firstChild().nodeValue();

                QTextStream(&lo) >> layer.wx1 >> layer.wy1;
                QTextStream(&up) >> layer.wx2 >> layer.wy2;

                layer.name = nm.firstChild().nodeValue();
//                menu.Add(name, name);
                QVariant variant;
                variant.setValue(layer);

                bool on = settings.value(QString("wms/%1/%2").arg(rovno, layer.name), true).toBool();

                dlg.Add(layer.name, variant, on);
            }
        }
    }

    if (dlg.cnt() == 0) {
//    if (menu.cnt() == 0) {
        QMessageBox::warning(nullptr, "", "В выбранной карте нет слоев");
    }
//    else if (menu.exec() == QDialog::Accepted) {
//    else if (menu.exec() == QDialog::Accepted) {
    else if (dlg.exec() == QDialog::Accepted) {

        QString ini = get_wms_xml_name(rovno);
        mkdir_for_file(ini);

        QFile file(ini);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(nullptr, "", QString("Не могу открыть файл %1 для записи").arg(ini));
            return false;
        }

        file.write(txt.toUtf8());
        file.close();

//        rovno = rovno;
        m_wms = getWMS_File_Name(rovno);

        QSettings settings;
        settings.setValue("WMS_host", rovno);

        for (int i = 0; i < layers.count(); ++i) {
            QDomNode layer = layers.item(i);
            QDomNode nm = layer.firstChildElement("Name");
            QString name = nm.firstChild().nodeValue();
            settings.setValue(QString("wms/%1/%2").arg(rovno, name), false);
        }

        m_layers.clear();

        for (auto & it : dlg.out) {
        
//            QString layer = menu.value().toString();

            Layer layer = it.value<Layer>();
            
//            setWMS(rovno, layer);

            m_layers.push_back(layer);

            int n = m_layers.size();

            settings.setValue("WMS_layer_n", n);
            settings.setValue(QString("WMS_layer%1").arg(n), layer.name);
            settings.setValue(QString("wms/%1/%2").arg(rovno, layer.name), true);
        }
    }

//    update();

    return true;
}



bool GidWidget::read_wmts(const QString & txt)
{
    if (txt == "") return false;

    QDomDocument dom;
    dom.setContent(txt);


    QDomNodeList layers = dom.firstChildElement("Capabilities").firstChildElement("Contents")
               .elementsByTagName("Layer");

    QSettings settings;

    for (int i = 0; i < layers.count(); ++i) {
        QDomNode layer = layers.item(i);
//        QDomNode nm = layer.firstChildElement("ows:Title");
        QDomNode nm = layer.firstChildElement("ows:Identifier");
        

        if (!nm.isNull()) {
            QDomNode box = layer.firstChildElement("ows:WGS84BoundingBox");
            if (!box.isNull()) {
                Layer layer;

                QString lo = box.firstChildElement("ows:LowerCorner").firstChild().nodeValue();
                QString up = box.firstChildElement("ows:UpperCorner").firstChild().nodeValue();

                QTextStream(&lo) >> layer.wx1 >> layer.wy1;
                QTextStream(&up) >> layer.wx2 >> layer.wy2;

                layer.name = nm.firstChild().nodeValue();
//                menu.Add(name, name);
                QVariant variant;
                variant.setValue(layer);

                bool on = settings.value(QString("wms/%1/%2").arg(rovno, layer.name), true).toBool();
                if (on) {
                    m_layers.push_back(layer);
                }
            }
        }
    }

    return true;
}
