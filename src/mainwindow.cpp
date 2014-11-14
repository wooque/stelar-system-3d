#include "mainwindow.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    glWidget = new GLWidget;

    QGridLayout *centralLayout = new QGridLayout;
    centralLayout->addWidget(glWidget, 0, 0);
    centralWidget->setLayout(centralLayout);

    setWindowTitle(tr("Test"));
    resize(1024, 600);
}
