#pragma once

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>
#include <memory>
#include <chrono>

class NebeskoTelo;

enum class view_modes {AXIS, SPHERE, CENTER};

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
    static std::string view_to_string(view_modes mode);

private slots:
    void advanceTime();

private:
    std::unique_ptr<NebeskoTelo> pozadina;
    std::vector<std::unique_ptr<NebeskoTelo>> stars;
    std::vector<std::unique_ptr<NebeskoTelo>> planets;

    std::chrono::system_clock::time_point prethodno_vreme;

    int win_width = 0;
    int win_height = 0;
    const float UNIV_R = 500.0f;

    int pogled = 0;
    int planeta = -1;
    float view_radius = 0.5;
    view_modes view_mode = view_modes::AXIS;

    float scale_x = 0.0f;
    float scale_y = 0.0f;
};
