#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H
#include <QtOpenGL/QtOpenGL>
#include "../BingApi.hpp"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    MyGLWidget(QWidget* parent);
    void setTileSystem(Bing::TileSystem* tileSystem);
private:
    void initializeGL();
    void resizeGL(int w, int h);
    void resizeOverlayGL(int w, int h);
    void paintGL();
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    Bing::TileSystem* m_TileSystem;
signals:
    void positionZoomChanged(const QString& info);
};

#endif // MYGLWIDGET_H
