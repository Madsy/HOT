#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QScrollBar>
#include <QGraphicsPixmapItem>
#include <QStatusBar>
#include <boost/lexical_cast.hpp>
#include <utility>
#include "QtMain.h"

BuildingFinderApp::BuildingFinderApp(Bing::TileSystem* tileSystem) : m_TileSystem(tileSystem)
{
  ui.setupUi(this);
  ui.widget->setTileSystem(m_TileSystem);

  QStatusBar* status = statusBar();
  m_CoordinateDialog = new CoordinateDialog(this);
  m_StatusLabel = new QLabel("X: 0, Y: 0");
  status->addPermanentWidget(m_StatusLabel);
  QObject::connect(ui.widget, SIGNAL(positionZoomChanged(const QString&)), this, SLOT(on_statusUpdate(const QString&)));
  QObject::connect(m_CoordinateDialog, SIGNAL(accepted()), this, SLOT(on_coordinateDialog_accept()));
  resetView();
}

BuildingFinderApp::~BuildingFinderApp()
{
    delete m_CoordinateDialog;
}

void BuildingFinderApp::on_statusUpdate(const QString& info)
{
    m_StatusLabel->setText(info);
}

void BuildingFinderApp::on_toolButtonConfirm_clicked()
{

}

void BuildingFinderApp::on_toolButtonConfirmNot_clicked()
{

}

void BuildingFinderApp::on_toolButtonUncertain_clicked()
{

}

void BuildingFinderApp::on_verticalSliderZoom_valueChanged(int value)
{
 m_TileSystem->setZoom(value);
 ui.widget->repaint();
}

void BuildingFinderApp::resetView()
{
  const double zoom = 18;
  double zoomRatio = std::pow(2.0, zoom);
  ui.verticalSliderZoom->setValue(zoom);
  m_TileSystem->setZoom(zoom);
  int ix,iy;
  //15.0467, -3.20836
  //15.0494, -3.19147
  //15.0392, -3.19613
  Bing::TileSystem::latLongToPixelXY(15.0392, -3.19613, (int)zoom, ix, iy);
  double x = (double)ix / zoomRatio;
  double y = (double)iy / zoomRatio;
  m_TileSystem->setX(x);
  m_TileSystem->setY(y);
}

void BuildingFinderApp::on_coordinateDialog_accept()
{
    double longitude = m_CoordinateDialog->getLongitude();
    double latitude  = m_CoordinateDialog->getLatitude();
    double zoom = m_CoordinateDialog->getZoom();
    double zoomRatio = std::pow(2.0, zoom);
    int ix,iy;

    Bing::TileSystem::latLongToPixelXY(latitude, longitude, zoom, ix, iy);

    double x = (double)ix / zoomRatio;
    double y = (double)iy / zoomRatio;
    m_TileSystem->setX(x);
    m_TileSystem->setY(y);
    m_TileSystem->setZoom(zoom);
    ui.verticalSliderZoom->setValue(zoom);
}

void BuildingFinderApp::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Space){
        m_CoordinateDialog->show();
        ui.widget->repaint();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

//Trondheim: Long 10.4174 Lat: 63.4384 Zoom 17
//Mali village -3.20836 15.0467
int main(int argc, char *argv[])
{
  try {
    Bing::TileSystem tileSystem;
    QApplication app(argc, argv);
    BuildingFinderApp bfa(&tileSystem);
    bfa.show();
    return app.exec();
  } catch(std::exception& ex){
    QString error("Caught exception: ");
    error.append(ex.what());
    QMessageBox msgBox;
    msgBox.setText(error);
    return 1;
  }
  return 0;
}

