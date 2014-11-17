#pragma once

#include <QGLWidget>
#include <memory>

#include "NebeskoTelo.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private slots:
    void advanceTime();

private:
    std::unique_ptr<NebeskoTelo> Sunce;
    std::unique_ptr<NebeskoTelo> pozadina;

    long  prethodno_vreme = 0;

    unsigned pogled = 0;
    int visina = 40;
    int planeta = -1;
    int prethodno_y = -1;
};
