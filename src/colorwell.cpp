#include "app.h"
#include "colorwell.h"

int QColorWell::rgbIndex(int row, int col) const
{
    return col + row*numCols();
}

void QColorWell::paintCellContents(QPainter *p, int row, int col, const QRect &r)
{
    int i = rgbIndex(row, col);
    if (i >= numCells())
        return;

    p->fillRect(r, QColor(values[i]));
}

void QColorWell::mousePressEvent(QMouseEvent *e)
{
    int row, col;

    row = selectedRow();
    col = selectedColumn();

    oldCurrent = QPoint(row, col);
    QWellArray::mousePressEvent(e);
    mousePressed = true;
    pressPos = e->pos();
}

void QColorWell::mouseMoveEvent(QMouseEvent *e)
{
    QWellArray::mouseMoveEvent(e);
#ifndef QT_NO_DRAGANDDROP
    if (!mousePressed)
        return;
    if ((pressPos - e->pos()).manhattanLength() > QApplication::startDragDistance()) {
        setCurrent(oldCurrent.x(), oldCurrent.y());
        int i = rowAt(pressPos.y()) + columnAt(pressPos.x()) * numRows();
        QColor col(values[i]);
        QMimeData *mime = new QMimeData;
        mime->setColorData(col);
        QPixmap pix(cellWidth(), cellHeight());
        pix.fill(col);
        QPainter p(&pix);
        p.drawRect(0, 0, pix.width() - 1, pix.height() - 1);
        p.end();
        QDrag *drg = new QDrag(this);
        drg->setMimeData(mime);
        drg->setPixmap(pix);
        mousePressed = false;
        drg->start();
    }
#endif
}

#ifndef QT_NO_DRAGANDDROP
void QColorWell::dragEnterEvent(QDragEnterEvent *e)
{
    if (qvariant_cast<QColor>(e->mimeData()->colorData()).isValid())
        e->accept();
    else
        e->ignore();
}

void QColorWell::dragLeaveEvent(QDragLeaveEvent *)
{
    if (hasFocus())
        parentWidget()->setFocus();
}

void QColorWell::dragMoveEvent(QDragMoveEvent *e)
{
    if (qvariant_cast<QColor>(e->mimeData()->colorData()).isValid()) {
        setCurrent(rowAt(e->pos().y()), columnAt(e->pos().x()));
        e->accept();
    } else {
        e->ignore();
    }
}

void QColorWell::dropEvent(QDropEvent *e)
{
    QColor col = qvariant_cast<QColor>(e->mimeData()->colorData());
    if (col.isValid()) {
        int i = rowAt(e->pos().y()) + columnAt(e->pos().x()) * numRows();
        values[i] = col.rgb();
        update();
        e->accept();
    } else {
        e->ignore();
    }
}

#endif // QT_NO_DRAGANDDROP

void QColorWell::mouseReleaseEvent(QMouseEvent *e)
{
    if (!mousePressed)
        return;

    QWellArray::mouseReleaseEvent(e);
    mousePressed = false;

    int i = rgbIndex(selectedRow(), selectedColumn());
    if (i >= numCells())
        throw "out of bounds";

    QRgb rgb = values[i];
    QColor newColor = QColorDialog::getColor(rgb, this, "Select Color", QColorDialog::DontUseNativeDialog);

    auto p = (Viewer*)userdata;
    p->setPalette(i, newColor.rgba());
}
