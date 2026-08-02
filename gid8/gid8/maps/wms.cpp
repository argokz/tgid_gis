#include <QtGui>
#include <QMessageBox>
#include <gidview/GidWidget.h>
#include <QPointF>
#include <QtXml>

#include <dialog/ChMenuDial.h>

QString getWMS_File_Name(const QString & r);
QString get_wms_xml_name(const QString & r);

QString getWMS_File_Name(const QString & r0)
{
    QString r = r0;

    r.replace("/arcgis", "");
    r.replace("/services", "");
    r.replace("/MapServer", "");
    r.replace("/WmsServer", "");
    r.replace("http://", "");
    r.replace("https://", "");
    r.replace(":", "_");
    r.replace("/", "_");
    r.replace("\\", "_");
    r.replace(".", "_");

    return r;
}

#if 1

bool read_wms_layer(QDomElement &layers, QString parentName, QString parentTitle, std::list<Layer> & list_layer)
{
    bool ret = false;
    QDomElement childElement = layers.firstChildElement();

    while (!childElement.isNull()) {
        if (childElement.tagName() == "Layer") {
            // Здесь вы можете работать с каждым элементом Layer, который является непосредственным дочерним элементом Capability

            QString nm = childElement.firstChildElement("Name").firstChild().nodeValue();
            QString tl = childElement.firstChildElement("Title").firstChild().nodeValue();

            ret = true;

            if (!read_wms_layer(childElement, nm, tl, list_layer)) {

//                if (parentName != "") nm = parentName + "." + nm;
                if (parentTitle != "") tl = parentTitle + " : " + tl;
//                qDebug() << "Layer" << nm << " - " << tl;

                QDomNode box = childElement.firstChildElement("EX_GeographicBoundingBox");
                if (!box.isNull()) {
                    Layer layer;


                    layer.wx1 = box.firstChildElement("westBoundLongitude").firstChild().nodeValue().toDouble();
                    layer.wx2 = box.firstChildElement("eastBoundLongitude").firstChild().nodeValue().toDouble();
                    layer.wy1 = box.firstChildElement("southBoundLatitude").firstChild().nodeValue().toDouble();
                    layer.wy2 = box.firstChildElement("northBoundLatitude").firstChild().nodeValue().toDouble();
                    layer.name = nm;

                    layer.title = layer.name;

                    if (!tl.isNull()) {
                        layer.title = tl;
                    }
                    list_layer.push_back(layer);
                }
            }
        }
        childElement = childElement.nextSiblingElement();
    }
    return ret;
}

#endif

bool GidWidget::read_wms_ask(const QString & txt)
{
    if (txt == "") return false;

    QSettings settings;
    ChMenuDial dlg(this, QObject::tr("Выберите слои"), true);

    QDomDocument dom;
    dom.setContent(txt);

    QDomElement layerElement = dom.firstChildElement("WMS_Capabilities").firstChildElement("Capability").firstChildElement("Layer");

    std::list<Layer> list_layer;

    read_wms_layer(layerElement, "", "", list_layer);

    for (auto & layer: list_layer) {
        QVariant variant;
        variant.setValue(layer);

        bool on = settings.value(QString("wms/%1/%2").arg(rovno, layer.name), true).toBool();
        dlg.Add(layer.title, variant, on);
    }

    if (dlg.cnt() == 0) {
        QMessageBox::warning(nullptr, "", "В выбранной карте нет слоев");
    }
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

        settings.setValue("WMS_host", rovno);

        for (auto & layer: list_layer) {
            settings.setValue(QString("wms/%1/%2").arg(rovno, layer.name), false);
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



bool GidWidget::read_wms(const QString & txt)
{
    if (txt == "") return false;

    QSettings settings;
    m_layers.clear();

    QDomDocument dom;
    dom.setContent(txt);
    QDomElement layerElement = dom.firstChildElement("WMS_Capabilities").firstChildElement("Capability").firstChildElement("Layer");

    std::list<Layer> list_layer;

    read_wms_layer(layerElement, "", "", list_layer);

    for (auto & layer: list_layer) {
        QVariant variant;
        variant.setValue(layer);

        bool on = settings.value(QString("wms/%1/%2").arg(rovno, layer.name), true).toBool();

        if (on) {
            m_layers.push_back(layer);
        }
    }

    return true;
}
