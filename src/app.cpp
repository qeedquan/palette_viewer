#include "app.h"

// sends to app to handle the event
void Window::keyPressEvent(QKeyEvent *e)
{
    emit keypress(e);
}

App::App(int &argc, char *argv[]) : QApplication(argc, argv)
{
    currentDir = QDir::currentPath();

    window = new Window();

    // menu buttons
    auto file = window->menuBar()->addMenu("File");
    auto open = new QAction("Open", window);
    auto save = new QAction("Save", window);
    auto saveas = new QAction("Save As", window);
    auto saver = new QAction("Save Region As", window);
    auto quit = new QAction("Quit", window);

    auto help = window->menuBar()->addMenu("Help");
    auto about = new QAction("About", window);

    file->addAction(open);
    file->addAction(save);
    file->addAction(saveas);
    file->addAction(saver);
    file->addAction(quit);

    help->addAction(about);

    // gray out save on startup
    saveButton = save;
    saveRegion = saver;
    saveRegion->setEnabled(false);
    saveButton->setEnabled(false);

    // menu signals
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(open, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(about, SIGNAL(triggered()), this, SLOT(about()));
    connect(save, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(saveas, SIGNAL(triggered()), this, SLOT(saveAsFile()));
    connect(saver, SIGNAL(triggered()), this, SLOT(saveAsRegionFile()));
    connect(window, SIGNAL(keypress(QKeyEvent*)), this, SLOT(keypress(QKeyEvent*)));

    // put viewer into grid and then embed the window into main window
    auto pw = new QWidget();
    auto grid = new QGridLayout();
    pw->setLayout(grid);
    viewer.embed(grid);
    window->setCentralWidget(pw);

    window->resize(640, 480);
    window->setWindowTitle("Palette Editor");
    window->show();
}

// shows about dialog
void App::about()
{
    char msg[80];

    snprintf(msg, sizeof(msg), "A simple palette editor %-9c\n\nAuthor: Quan Tran", ' ');
    QMessageBox::about(NULL, "Palette Editor", QString(msg));
}

// opens an image file
void App::openFile()
{
    const char *ext = "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ttf *.xbm *.xpm)";
    const char *dir = currentDir.toUtf8();

    auto filename = QFileDialog::getOpenFileName(window, "Open Image", dir, ext);
    auto i = filename.lastIndexOf("/");
    if (i <= 0)
        return;

    if (!viewer.loadImage(filename))
    {
        QMessageBox::critical(NULL, "Error", "Failed to open image: " + filename);
        return;
    }

    currentDir = filename.left(i);
    currentFile = filename;
    window->setWindowTitle("Palette Editor - " + filename);
    saveButton->setEnabled(true);
    saveRegion->setEnabled(true);
}

void App::saveFile()
{
    if (currentFile == "")
        return;

    if (!viewer.saveImage(currentFile, false))
        QMessageBox::critical(window, "Error", "Failed to save: " + currentFile);
}

void App::saveImageRegionFile(bool region)
{
    const char *ext = "Image Files (*.png)";
    const char *dir = currentDir.toUtf8();

    auto filename = QFileDialog::getSaveFileName(window, "Save Image", dir, ext);
    auto i = filename.lastIndexOf("/");
    if (i <= 0)
        return;

    if (!viewer.saveImage(filename, region))
        QMessageBox::critical(window, "Error", "Failed to save: " + filename);

    currentDir = filename.left(i);

    // if we are only saving a region of image, don't update window title or current file
    if (region)
        return;

    window->setWindowTitle("Palette Editor - " + filename);
    currentFile = filename;
}

void App::saveAsFile()
{
    saveImageRegionFile(false);
}

void App::saveAsRegionFile()
{
    saveImageRegionFile(true);
}

void App::keypress(QKeyEvent *e)
{
    // undo
    if (e->key() == Qt::Key_U)
        viewer.undo(e);
}
