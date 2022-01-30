#include "viewer.h"

using std::min;
using std::max;

// builds a palette table for the portion of image we are interested in
void Palette::build(const QImage &image, const QRect& view)
{
    QMap<QRgb, QVector<QPoint>> lut;

    // get the portion of the image to build palette table for
    int x1 = view.topLeft().x();
    int y1 = view.topLeft().y();
    int x2 = view.bottomRight().x();
    int y2 = view.bottomRight().y();
    int rx = x1;

    points.clear();
    colors.clear();

    // go through the image, finding all locations of a unique color
    for (; y1 <= y2; y1++)
    {
        for (x1 = rx; x1 <= x2; x1++)
        {
            int x = x1;
            int y = y1;

            QRgb p = image.pixel(x, y);
            if (!lut.contains(p))
            {
                QVector<QPoint> pt;

                pt.push_back(QPoint(x, y));
                lut[p] = pt;
            }
            else
            {
                lut[p].push_back(QPoint(x, y));
                continue;
            }

            colors.push_back(p);
        }
    }

    // now go through all the colors and copy the point locations
    // associated with a color
    for (auto i = 0; i < colors.size(); i++)
    {
        QRgb p = colors[i];
        QVector<QPoint> pts = lut[p];
        points.push_back(pts);
    }
}

// set palette to a color and save it to history
void Viewer::setPalette(int i, QRgb newColor, bool saveHistory)
{
    History hist;
    auto pts = palette.points[i];

    for (int j = 0; j < pts.size(); j++)
        image.setPixel(pts[j].x(), pts[j].y(), newColor);

    hist.index = i;
    hist.oldColor = palette.colors[i];
    hist.newColor = newColor;

    if (saveHistory)
        palette.history.push_back(hist);

    palette.colors[i] = newColor;
    pixmapItem->pixmap().convertFromImage(image);
    colorWindow->repaint();
    viewport()->update();
}

bool Viewer::loadImage(const QString& file)
{
    QImage pix(file);

    // need to convert pixel format or else setPixel will act based upon
    // the format that could be indexed (we want truecolor)
    pix = pix.convertToFormat(QImage::Format_ARGB32, Qt::AutoColor);

    // load image
    if (pix.isNull())
        return false;

    image = pix;
    selector = image.rect();

    // build palette
    palette.build(image, selector);
    makeColorWindow(palette);

    // now embed it into the scene
    scene.clear();
    scene.setBackgroundBrush(QBrush(Qt::lightGray, Qt::CrossPattern));

    // align it, this is important because of how we get selection box
    // for palette
    auto pixmap = QPixmap::fromImage(image);
    pixmapItem = scene.addPixmap(pixmap);
    boxItem = scene.addRect(0, 0, 0, 0);
    boxItem->setVisible(false);
    scene.setSceneRect(image.rect());
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QSize areaSize = viewport()->size();
    QSize widgetSize = image.size();

    verticalScrollBar()->setPageStep(widgetSize.height());
    horizontalScrollBar()->setPageStep(widgetSize.width());
    verticalScrollBar()->setRange(0, widgetSize.height() - areaSize.height());
    horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
    horizontalScrollBar()->setValue(0);
    verticalScrollBar()->setValue(0);
    update();

    return true;
}

bool Viewer::saveImage(const QString &file, bool region)
{
    if (region)
        return image.copy(selector).save(file, "PNG");

    return image.save(file, "PNG");
}

// add the palette color table to the window for display
void Viewer::makeColorWindow(const Palette &palette)
{
    int row, col;
    auto l = paletteWindow->layout();
    auto size = palette.colors.size();

    row = size / MAX_PALETTE_COLUMN;
    if ((size % MAX_PALETTE_COLUMN) != 0)
        row++;

    col = size + 1;
    if (size > MAX_PALETTE_COLUMN)
        col = MAX_PALETTE_COLUMN;

    // add colors to the color window for display
    if (colorWindow)
    {
        l->removeWidget(colorWindow);
        delete colorWindow;
    }

    auto rgb = (QRgb*)palette.colors.data();
    colorWindow = new QColorWell(NULL, row, col, size, rgb, this);
    l->addWidget(colorWindow);
}

void Viewer::embed(QGridLayout *l)
{
    // palette window
    paletteWindow = new QWidget();
    colorWindow = NULL;

    // embed layout into window
    auto layout = new QGridLayout();
    paletteWindow->setLayout(layout);
    layout->setSpacing(2);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    // make a scroll area to embed the palette window into
    auto scrollArea = new QScrollArea();
    scrollArea->setWidget(paletteWindow);
    scrollArea->setWidgetResizable(true);

    // now embed the viewer
    l->addWidget(scrollArea);
    l->addWidget(this);
}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    if (image.isNull())
        return;

    QPoint pt;
    QTransform idt;
    int ox, oy;
    switch (e->button())
    {
        case Qt::LeftButton:
            ox = horizontalScrollBar()->value();
            oy = verticalScrollBar()->value();
            pt = QPoint(ox, oy) + e->pos();
            if (scene.itemAt(pt.x(), pt.y(), idt) == NULL)
                return;

            selectP1 = pt;
            pressed = true;
            break;

        case Qt::MiddleButton:
            // build palette
            palette.build(image, selector);
            makeColorWindow(palette);
            break;

        case Qt::RightButton:
            boxItem->setVisible(false);
            pressed = false;
            selector = boxSelector = image.rect();
            break;

        default:
            break;
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *e)
{
    if (image.isNull())
        return;

    if (!pressed)
        return;

    int ox, oy;
    QPoint p1, p2;

    ox = horizontalScrollBar()->value();
    oy = verticalScrollBar()->value();

    p1 = selectP1;
    p2 = e->pos() + QPoint(ox, oy);
    boxSelector = calcSelectBox(p1, p2);

    boxItem->setRect(boxSelector);
    boxItem->setVisible(true);
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    if (image.isNull())
        return;

    switch (e->button())
    {
        case Qt::LeftButton:
            if (pressed)
            {
                selector = boxSelector;
                pressed = false;
            }
            break;

        default:
            break;
    }
}

QRect Viewer::calcSelectBox(QPoint p1, QPoint p2)
{
    QPoint pa, pb;

    pa = QPoint(min(p1.x(), p2.x()), min(p1.y(), p2.y()));
    pb = QPoint(max(p1.x(), p2.x()), max(p1.y(), p2.y()));
    return QRect(pa, pb);
}

void Viewer::undo(QKeyEvent*)
{
    if (palette.history.size() == 0)
        return;

    auto i = palette.history.size() - 1;
    setPalette(palette.history[i].index, palette.history[i].oldColor, false);
    palette.history.pop_back();
}
