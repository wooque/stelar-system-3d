#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QScrollArea>
#include <QMenu>
#include <QAction>

#include "glwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openConfiguration();
    void about();

private:
    QWidget *centralWidget;
    QScrollArea *glWidgetArea;
    GLWidget *glWidget;

    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *openSystem;
    QAction *exitAct;
    QAction *aboutAct;
};
