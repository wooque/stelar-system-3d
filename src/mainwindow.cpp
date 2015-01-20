#include "mainwindow.h"

#include <QtWidgets>
#include <QGridLayout>

MainWindow::MainWindow()
{
    openSystem = new QAction(tr("&Open system"), this);
    openSystem->setShortcut(QKeySequence::Open);
    connect(openSystem, SIGNAL(triggered()), this, SLOT(openConfiguration()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setShortcuts(QKeySequence::HelpContents);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openSystem);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    glWidget = new GLWidget;
    glWidgetArea = new QScrollArea;
    glWidgetArea->setFocusPolicy(Qt::NoFocus);
    glWidgetArea->setWidget(glWidget);
    glWidgetArea->setWidgetResizable(true);
    glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glWidgetArea->setMinimumSize(50, 50);
    setCentralWidget(glWidgetArea);

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
    QMessageBox::about(this, tr("About Planetarium"), tr("Visualises star systems"));
}
