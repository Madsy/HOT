#ifndef QTMAIN_H_GUARD
#define QTMAIN_H_GUARD

#include "../BingApi.hpp"
#include "ui_BuildingFinder.h"
#include "myglwidget.h"
#include "CoordinateDialog.h"

class BuildingFinderApp : public QMainWindow
{

  Q_OBJECT

public:
  BuildingFinderApp(Bing::TileSystem* tileSystem);
  virtual ~BuildingFinderApp();
protected:
  void keyPressEvent (QKeyEvent* event);
private slots:
  void on_toolButtonConfirm_clicked();
  void on_toolButtonConfirmNot_clicked();
  void on_toolButtonUncertain_clicked();
  void on_verticalSliderZoom_valueChanged(int value);
  void on_statusUpdate(const QString& info);
  void on_coordinateDialog_accept();
private:
  void resetView();

  Bing::TileSystem* m_TileSystem;
  Ui::BuildingFinder ui;
  QLabel* m_StatusLabel;
  CoordinateDialog* m_CoordinateDialog;
};

#endif
