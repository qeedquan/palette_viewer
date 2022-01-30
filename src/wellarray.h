#pragma once

#include <QtWidgets>

struct QWellArrayData {
    QBrush *brush;
};

class QWellArray : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int selectedColumn READ selectedColumn)
    Q_PROPERTY(int selectedRow READ selectedRow)

public:
    QWellArray(int rows, int cols, int cells, QWidget* parent=0);
    ~QWellArray() {}
    QString cellContent(int row, int col) const;

    int selectedColumn() const {
        return selCol;
    }
    int selectedRow() const {
        return selRow;
    }

    virtual void setCurrent(int row, int col);
    virtual void setSelected(int row, int col);

    QSize sizeHint() const;

    virtual void setCellBrush(int row, int col, const QBrush &);
    QBrush cellBrush(int row, int col);

    inline int cellWidth() const
    {
        return cellw;
    }

    inline int cellHeight() const
    {
        return cellh;
    }

    inline int rowAt(int y) const
    {
        return y / cellh;
    }

    inline int columnAt(int x) const
    {
        if (isRightToLeft()) return ncols - (x / cellw) - 1;
        return x / cellw;
    }

    inline int rowY(int row) const
    {
        return cellh * row;
    }

    inline int columnX(int column) const
    {
        if (isRightToLeft()) return cellw * (ncols - column - 1);
        return cellw * column;
    }

    inline int numRows() const
    {
        return nrows;
    }

    inline int numCols() const
    {
        return ncols;
    }

    inline int numCells() const
    {
        return ncells;
    }

    inline QRect cellRect() const
    {
        return QRect(0, 0, cellw, cellh);
    }

    inline QSize gridSize() const
    {
        return QSize(ncols * cellw, nrows * cellh);
    }

    QRect cellGeometry(int row, int column)
    {
        QRect r;
        if (row >= 0 && row < nrows && column >= 0 && column < ncols)
            r.setRect(columnX(column), rowY(row), cellw, cellh);
        return r;
    }

    inline void updateCell(int row, int column)
    {
        update(cellGeometry(row, column));
    }

signals:
    void selected(int row, int col);

protected:
    virtual void paintCell(QPainter *, int row, int col, const QRect&);
    virtual void paintCellContents(QPainter *, int row, int col, const QRect&);

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void paintEvent(QPaintEvent *);

private:
    Q_DISABLE_COPY(QWellArray)
    static const int margin = 3;

    int nrows;
    int ncols;
    int ncells;
    int cellw;
    int cellh;
    int curRow;
    int curCol;
    int selRow;
    int selCol;
    QWellArrayData *d;
};
