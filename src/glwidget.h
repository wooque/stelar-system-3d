#pragma once

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QJsonObject>
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

    void loadLightingData(float* lighting, const QJsonObject &data,
                          const QString &type);
    std::unique_ptr<NebeskoTelo> loadStelarBody(const QString &conf_dir,
                                                const QJsonObject &data,
                                                bool is_star,
                                                int trail_length);


    static std::string view_to_string(view_modes mode);

private slots:
    void advanceTime();

private:
    std::unique_ptr<NebeskoTelo> pozadina;
    std::vector<std::unique_ptr<NebeskoTelo>> bodies;

    std::chrono::system_clock::time_point prethodno_vreme;

    float ambient[3] = {0.1, 0.1, 0.1};
    float difuse[3] = {1.0, 1.0, 1.0};
    float position[3] = {0.0, 0.0, 0.0};

    int win_width = 0;
    int win_height = 0;
    const float UNIV_R = 500.0f;

    int view_body = 0;
    int ref_body = -1;
    float view_radius = 0.5;
    view_modes view_mode = view_modes::AXIS;

    float scale_x = 0.0f;
    float scale_y = 0.0f;

    static const QString BACKGROUND;
    static const QString LIGHTING;
    static const QString AMBIENT;
    static const QString DIFUSE;
    static const QString POSITION;
    static const QString STARS;
    static const QString PLANETS;
    static const QString NAME;
    static const QString RADIUS;
    static const QString REVOLUTION_VELOCITY;
    static const QString REVOLUTION_RADIUS;
    static const QString ROTATION_VELOCITY;
    static const QString SLOPE;
    static const QString ROTATION_SLOPE;
    static const QString TEXTURE;
    static const QString RING;
    static const QString RING_INNER_RADIUS;
    static const QString RING_OUTTER_RADIUS;
    static const QString SATELLITES;
};
