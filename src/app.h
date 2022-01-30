#pragma once

#include <QtWidgets>
#include "viewer.h"

class Window : public QMainWindow
{
    Q_OBJECT

signals:
    void keypress(QKeyEvent *e);

public slots:
    void keyPressEvent(QKeyEvent *e);
};

class App : public QApplication
{
    Q_OBJECT

private:
    QString currentDir;
    QString currentFile;

    Window   *window;
    QAction  *saveButton, *saveRegion;
    Viewer    viewer;

private:
    void saveImageRegionFile(bool region);

public:
    App(int &argc, char *argv[]);

public slots:
    void keypress(QKeyEvent *e);
    void about();
    void openFile();
    void saveFile();
    void saveAsFile();
    void saveAsRegionFile();
};
