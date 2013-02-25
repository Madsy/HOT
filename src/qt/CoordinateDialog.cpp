#include "CoordinateDialog.h"
#if 0
class CoordinateDialog : public QWidget
{
public:
    void setLongitude(double longitude);
    void setLatitude(double latitude);
    void setZoom(double zoom);
    double getLongitude() const;
    double getLatitude() const;
    double getZoom() const;
private slots:
    void on_lineEditLongitude_textChanged();
    void on_lineEditLatitude_textChanged();
    void on_verticalSliderZoom_valueChanged();
private:
    Ui::CoordinateDialog dialog;
    double m_Longitude, m_Latitude, m_Zoom;
};

#endif

CoordinateDialog::CoordinateDialog(QWidget* parent) : QDialog(parent)
{
    ui.setupUi(this);
    m_Longitude = 0.0;
    m_Latitude = 0.0;
    m_Zoom = 3.0;
}

void CoordinateDialog::setLongitude(double longitude)
{
    m_Longitude = longitude;
}

void CoordinateDialog::setLatitude(double latitude)
{
    m_Latitude = latitude;
}

void CoordinateDialog::setZoom(double zoom)
{
    m_Zoom = zoom;
}

double CoordinateDialog::getLongitude() const
{
    return m_Longitude;
}

double CoordinateDialog::getLatitude() const
{
    return m_Latitude;
}

double CoordinateDialog::getZoom() const
{
    return m_Zoom;
}

void CoordinateDialog::on_lineEditLongitude_textChanged()
{
    bool ok = false;
    double longitude = ui.lineEditLongitude->text().toDouble(&ok);
    if(ok) setLongitude(longitude);
}

void CoordinateDialog::on_lineEditLatitude_textChanged()
{
    bool ok = false;
    double latitude = ui.lineEditLatitude->text().toDouble(&ok);
    if(ok) setLatitude(latitude);
}

void CoordinateDialog::on_verticalSliderZoom_valueChanged()
{
    double zoom = ui.verticalSliderZoom->value();
    if(zoom >= 3 && zoom <= 18) setZoom(zoom);
}
