#include "myglwidget.h"
#include <vector>
#include "../rectDetect.hpp"

MyGLWidget::MyGLWidget(QWidget* parent) : QGLWidget(parent)
{

}

void MyGLWidget::setTileSystem(Bing::TileSystem* tileSystem)
{
    m_TileSystem = tileSystem;
}

void MyGLWidget::initializeGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)width(), (double)height(), 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLineWidth(2.0f);
}

void MyGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)w, (double)h, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyGLWidget::resizeOverlayGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)w, (double)h, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyGLWidget::paintGL()
{
    QPixmap* tile;
    GLuint tileIndex;
    const int TileSize = Bing::TileSystem::TileSize;

    double zoom = m_TileSystem->Zoom();
    double halfWidth = (double)width() / 2.0;
    double halfHeight = (double)height() / 2.0;
    double zoomWorld = std::pow(2.0, zoom);
    double centerX = m_TileSystem->X() * zoomWorld;
    double centerY = m_TileSystem->Y() * zoomWorld;
    int tx = -((int)centerX - halfWidth);
    int ty = -((int)centerY - halfHeight);
    int xStart = centerX - halfWidth;
    int xEnd = centerX + halfWidth;
    int yStart = centerY - halfHeight;
    int yEnd = centerY + halfHeight;

    xStart = ((int)xStart / TileSize) * TileSize;
    yStart = ((int)yStart / TileSize) * TileSize;
    xEnd   = (((int)xEnd + TileSize - 1) / TileSize) * TileSize;
    yEnd   = (((int)yEnd + TileSize - 1) / TileSize) * TileSize;

    glLoadIdentity();
    glTranslatef(tx, ty, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    for(int y = yStart; y < yEnd; y += TileSize){
      for(int x = xStart; x < xEnd; x += TileSize){
        int tileX, tileY;
        Bing::TileSystem::pixelXYToTileXY(x, y, tileX, tileY);

        if(tileX >= (int)zoomWorld || tileX < 0)
            continue;
        if(tileY >= (int)zoomWorld || tileY < 0)
            continue;
        tile = m_TileSystem->getTile("tiles", tileX, tileY, zoom);
        tileIndex = bindTexture(*tile, GL_TEXTURE_2D, GL_RGB);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        drawTexture(QPointF(x,y), tileIndex, GL_TEXTURE_2D);
        deleteTexture(tileIndex);
        delete tile;

        if(zoom >= 18){
            std::vector<std::vector<cv::Point> > squares;
            findSquares("tiles", tileX, tileY, zoom, squares);
            if(!squares.empty()){
                glDisable(GL_TEXTURE_2D);
                glColor3ub(255, 0, 0);
                for(unsigned int i = 0; i < squares.size(); ++i){
                    glBegin(GL_LINE_LOOP);
                    for(unsigned int j = 0; j < squares[i].size(); ++j){
                        glVertex2i(x + squares[i][j].x, y + squares[i][j].y);
                    }
                    glEnd();
                }
                glColor3ub(255, 255, 255);
                glEnable(GL_TEXTURE_2D);
            }
        }
      }
    }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent* event)
{

}

void MyGLWidget::mousePressEvent(QMouseEvent* event)
{
    double zoom = m_TileSystem->Zoom();
    double zoomRatio = std::pow(2.0, zoom);
    double invZoomRatio = 1.0 / zoomRatio;
    double centerX = (double)width() / 2.0;
    double centerY = (double)height() / 2.0;
    double xDelta = ((double)event->x() - centerX) * invZoomRatio;
    double yDelta = ((double)event->y() - centerY) * invZoomRatio;
    double x = m_TileSystem->X() + xDelta;
    double y = m_TileSystem->Y() + yDelta;
    double latitude, longitude;
    m_TileSystem->pixelXYToLatLong(x * zoomRatio, y * zoomRatio, zoom, latitude, longitude);

    m_TileSystem->setX(x);
    m_TileSystem->setY(y);
    repaint();
    QString msg = "Long: " + QString::number(longitude) + " Lat: " + QString::number(latitude) + " Zoom: " + QString::number(m_TileSystem->Zoom());
    emit positionZoomChanged(msg);
}

void MyGLWidget::wheelEvent(QWheelEvent* event)
{

}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{

}
