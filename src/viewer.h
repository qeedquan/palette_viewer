#pragma once

#include <QtWidgets>
#include <QTransform>
#include "colorwell.h"

#define MAX_PALETTE_COLUMN 16

struct History
{
    int  index;
    QRgb oldColor, newColor;
};

class Palette
{
public:
    // colors associated with an index
    QVector<QRgb> colors;

    // given an index, return points related to it
    QVector<QVector<QPoint>> points;

    // history of the modification
    QVector<History> history;

public:
    void build(const QImage& image, const QRect& view);
};

class Viewer : public QGraphicsView
{
    Q_OBJECT

private:
    bool                       pressed;
    QPoint                     selectP1, selectP2;
    QRect                      selector, boxSelector;
    QWidget                   *paletteWindow;
    QColorWell                *colorWindow;

    QGraphicsPixmapItem       *pixmapItem;
    QGraphicsRectItem         *boxItem;
    QImage                     image;
    QGraphicsScene             scene;
    Palette                    palette;

private:
    void makeColorWindow(const Palette& palette);
    QRect calcSelectBox(QPoint p1, QPoint p2);

public:
    Viewer(QWidget *parent = 0) : QGraphicsView(parent)
    {
        setScene(&scene);
    }

    bool loadImage(const QString& file);
    bool saveImage(const QString& name, bool region);
    void embed(QGridLayout *l);
    void setPalette(int i, QRgb newColor, bool saveHistory = true);
    void undo(QKeyEvent *e);

public slots:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
};
