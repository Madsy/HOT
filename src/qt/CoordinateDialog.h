#ifndef COORDINATEDIALOG_H
#define COORDINATEDIALOG_H

#include <QDialog>
#include "../BingApi.hpp"
#include "ui_BuildingFinderCoordinateDialog.h"

class CoordinateDialog : public QDialog
{
    Q_OBJECT
public:
    CoordinateDialog(QWidget* parent);
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
    Ui::CoordinateDialog ui;
    double m_Longitude, m_Latitude, m_Zoom;
};

#endif // COORDINATEDIALOG_H
