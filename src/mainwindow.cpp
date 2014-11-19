#include "mainwindow.h"

#include <QtWidgets>
#include <QGridLayout>

MainWindow::MainWindow()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    glWidget = new GLWidget;

    glWidgetArea = new QScrollArea;
    glWidgetArea->setFocusPolicy(Qt::NoFocus);
    glWidgetArea->setWidget(glWidget);
    glWidgetArea->setWidgetResizable(true);
    glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glWidgetArea->setMinimumSize(50, 50);

    openSystem = new QAction(tr("&Open system"), this);
    openSystem->setShortcut(QKeySequence::Open);
    connect(openSystem, SIGNAL(triggered()), this, SLOT(openConfiguration()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openSystem);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    QGridLayout *centralLayout = new QGridLayout;
    centralLayout->addWidget(glWidgetArea, 0, 0);
    centralWidget->setLayout(centralLayout);

    setWindowTitle(tr("Planetarium"));
    resize(1024, 600);
}

void MainWindow::openConfiguration()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open System"),
                                                    QString(),
                                                    tr("Configurations (*.json)"));
    if (!fileName.isEmpty()) {
        glWidget->loadConfiguration(fileName);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Planetarium"),
            tr("<b>Planetarium</b> visualises star systems"));
}

