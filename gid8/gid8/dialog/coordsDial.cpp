#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QSettings>
#include <QDoubleValidator>
#include <QIntValidator>

class CoordSystemDialog : public QDialog {
    Q_OBJECT
public:
    CoordSystemDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Новая База данных");

        auto *layout = new QFormLayout(this);

        nameCrsEdit = new QLineEdit(this);

        nameDbEdit = new QLineEdit(this);
        
        ellipsoidBox = new QComboBox(this);
//        ellipsoidBox->addItems({"WGS84", "Krassovsky", "GRS80"});
        ellipsoidBox->addItems({"WGS84", "Красовского"});

        centralMeridianEdit = new QLineEdit(this);
        latOriginEdit = new QLineEdit(this);
        falseEastingEdit = new QLineEdit(this);
        falseNorthingEdit = new QLineEdit(this);
        scaleEdit = new QLineEdit(this);

     // Валидаторы
        auto *angleValidator = new QDoubleValidator(-180.0, 180.0, 10, this);
        angleValidator->setNotation(QDoubleValidator::StandardNotation);

        auto *latValidator = new QDoubleValidator(-90.0, 90.0, 10, this);
        latValidator->setNotation(QDoubleValidator::StandardNotation);

        auto *positiveValidator = new QDoubleValidator(0.0, 1e9, 10, this);
        positiveValidator->setNotation(QDoubleValidator::StandardNotation);

        auto *scaleValidator = new QDoubleValidator(0.0, 10.0, 10, this);
        scaleValidator->setNotation(QDoubleValidator::StandardNotation);

        centralMeridianEdit->setValidator(angleValidator);
        latOriginEdit->setValidator(latValidator);
        falseEastingEdit->setValidator(positiveValidator);
        falseNorthingEdit->setValidator(positiveValidator);
        scaleEdit->setValidator(scaleValidator);

        layout->addRow("Название Базы данных:", nameDbEdit);
        layout->addRow("Название:", nameCrsEdit);
        layout->addRow("Эллипсоид:", ellipsoidBox);
        layout->addRow("Центральный меридиан:", centralMeridianEdit);
        layout->addRow("Начало координат (широта):", latOriginEdit);
        layout->addRow("Ложный восток:", falseEastingEdit);
        layout->addRow("Ложный север:", falseNorthingEdit);
        layout->addRow("Масштаб:", scaleEdit);

        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, this, &CoordSystemDialog::saveAndAccept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        loadSettings();
    }

    QString toDbName() const {
        return nameDbEdit->text();
    }

    QString toCrsName() const {
        return nameCrsEdit->text();
    }

    QString toProjString() const {
        return QString("+proj=tmerc +ellps=%1 +lon_0=%2 +lat_0=%3 +x_0=%4 +y_0=%5 +k=%6")
            .arg(ellipsoidBox->currentText())
            .arg(centralMeridianEdit->text())
            .arg(latOriginEdit->text())
            .arg(falseEastingEdit->text())
            .arg(falseNorthingEdit->text())
            .arg(scaleEdit->text().isEmpty() ? "1.0" : scaleEdit->text());
    }

    QString toWktString() const {
        return QString(
            "PROJCS[\"%1\",\n"
            "  GEOGCS[\"%2\",\n"
            "    DATUM[\"%2\", SPHEROID[\"%2\", 6378137, 298.257223563]],\n"
            "    PRIMEM[\"Greenwich\", 0],\n"
            "    UNIT[\"degree\", 0.0174532925199433]],\n"
            "  PROJECTION[\"Transverse_Mercator\"],\n"
            "  PARAMETER[\"central_meridian\", %3],\n"
            "  PARAMETER[\"latitude_of_origin\", %4],\n"
            "  PARAMETER[\"false_easting\", %5],\n"
            "  PARAMETER[\"false_northing\", %6],\n"
            "  PARAMETER[\"scale_factor\", %7],\n"
            "  UNIT[\"metre\", 1]]"
        )
        .arg(nameCrsEdit->text())
        .arg(ellipsoidBox->currentText())
        .arg(centralMeridianEdit->text())
        .arg(latOriginEdit->text())
        .arg(falseEastingEdit->text())
        .arg(falseNorthingEdit->text())
        .arg(scaleEdit->text().isEmpty() ? "1.0" : scaleEdit->text());
    }

    double centralMeridian() const { return centralMeridianEdit->text().toDouble(); }
    double latOrigin() const       { return latOriginEdit->text().toDouble(); }
    double falseEasting() const    { return falseEastingEdit->text().toDouble(); }
    double falseNorthing() const   { return falseNorthingEdit->text().toDouble(); }
    double scale() const {
        QString val = scaleEdit->text();
        return val.isEmpty() ? 1.0 : val.toDouble();
    }
    QString name() const           { return nameCrsEdit->text(); }
    QString ellipsoid() const      { return ellipsoidBox->currentText(); }



private slots:
    void saveAndAccept() {
        QSettings settings;
        settings.beginGroup("CoordSystem");
        settings.setValue("name", nameCrsEdit->text());
        settings.setValue("ellipsoid", ellipsoidBox->currentIndex());
        settings.setValue("centralMeridian", centralMeridianEdit->text());
        settings.setValue("latOrigin", latOriginEdit->text());
        settings.setValue("falseEasting", falseEastingEdit->text());
        settings.setValue("falseNorthing", falseNorthingEdit->text());
        settings.setValue("scale", scaleEdit->text());
        settings.endGroup();

        accept();
    }

    void loadSettings() {
        QSettings settings;
        settings.beginGroup("CoordSystem");
        nameCrsEdit->setText(settings.value("name", "My CRS").toString());
        ellipsoidBox->setCurrentIndex(settings.value("ellipsoid", 1).toInt());
        centralMeridianEdit->setText(settings.value("centralMeridian", "0").toString());
        latOriginEdit->setText(settings.value("latOrigin", "0").toString());
        falseEastingEdit->setText(settings.value("falseEasting", "500000").toString());
        falseNorthingEdit->setText(settings.value("falseNorthing", "0").toString());
        scaleEdit->setText(settings.value("scale", "1.0").toString());
        settings.endGroup();
    }

private:

    QLineEdit *nameDbEdit;
    
    QLineEdit *nameCrsEdit;
    QComboBox *ellipsoidBox;
    QLineEdit *centralMeridianEdit;
    QLineEdit *latOriginEdit;
    QLineEdit *falseEastingEdit;
    QLineEdit *falseNorthingEdit;
    QLineEdit *scaleEdit;
};


#include "coordsDial.moc"


QString makeInsertSQL(int srid,
                      const QString &name,
                      const QString &ellps,
                      double lon0,
                      double lat0,
                      double fe,
                      double fn,
                      double scale,
                      const QString &proj0,
                      const QString &wkt0)
{

    QString proj = proj0;
    QString wkt = wkt0;

    return QString(
        "INSERT INTO spatial_ref_sys (srid, auth_name, auth_srid, srtext, proj4text) "
        "VALUES ("
        "    %1,"
        "    'EPSG',"
        "    %1,"
        "    '%2',"
        "    '%3'"
        ") "
        "ON CONFLICT (srid) DO UPDATE "
        "SET auth_name = EXCLUDED.auth_name, "
        "    auth_srid = EXCLUDED.auth_srid, "
        "    proj4text = EXCLUDED.proj4text, "
        "    srtext    = EXCLUDED.srtext;")
        .arg(srid)
        .arg(wkt.replace("'", "''"))   // экранируем кавычки
        .arg(proj.replace("'", "''"));
}




bool getCoordSystem(QWidget *widget, QString &proj, QString & wkt, QString & crsName, QString & dbName, QString & sql)
{
    CoordSystemDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        int srid = 9998;
        proj = dlg.toProjString();
        wkt = dlg.toWktString();
        dbName = dlg.toDbName();
        crsName = dlg.toCrsName();

        sql = makeInsertSQL(srid,
                                dlg.toCrsName(),
                                "WGS84",  // Не нужно
                                dlg.centralMeridian(),
                                dlg.latOrigin(),
                                dlg.falseEasting(),
                                dlg.falseNorthing(),
                                dlg.scale(),
                                proj,
                                wkt);


        qDebug() << "Proj:" << proj;
        qDebug() << "WKT:" << wkt;
        return true;
    }

    return false;
}
