#pragma once

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>
#include <memory>
#include <chrono>

#include "NebeskoTelo.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

public slots:
    void loadConfiguration(QString filename);

private:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    bool isInitialise() const;

private slots:
    void advanceTime();

private:
    std::unique_ptr<NebeskoTelo> pozadina;
    std::vector<std::unique_ptr<NebeskoTelo>> stars;
    std::vector<std::unique_ptr<NebeskoTelo>> planets;

    std::chrono::system_clock::time_point prethodno_vreme;

    int pogled = 0;
    int visina = 40;
    int planeta = -1;
    int prethodno_y = -1;
};
