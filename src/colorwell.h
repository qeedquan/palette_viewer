#pragma once

#include "wellarray.h"

class QColorWell : public QWellArray
{
public:
    QColorWell(QWidget *parent, int r, int c, int cells, QRgb *vals, void *data)
        : QWellArray(r, c, cells, parent), values(vals), userdata(data), mousePressed(false), oldCurrent(-1, -1)
    {
        setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    }

    ~QColorWell()
    {}

protected:
    int rgbIndex(int row, int col) const;

    void paintCellContents(QPainter *, int row, int col, const QRect&);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
#endif

private:
    QRgb *values;
    void *userdata;
    bool mousePressed;
    QPoint pressPos;
    QPoint oldCurrent;
};
