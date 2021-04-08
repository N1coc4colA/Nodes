#include "view.h"

#include "helpers.h"
#include "nodeitem.h"
#include "singlenodeedits.h"

#include <QMenu>
#include <QBrush>
#include <QCursor>
#include <QPalette>
#include <QPainter>
#include <QMimeData>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtOpenGL/QGLFormat>
#include <QtOpenGL/QGLWidget>

View::View(QWidget *parent) :
    QGraphicsView(parent)
{
    QGLFormat format(QGL::FormatOption::SampleBuffers);
    format.setSamples(10);
    setViewport(new QGLWidget(format));

    setRenderHint(QPainter::RenderHint::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
}

void View::contextMenuEvent(QContextMenuEvent *e)
{
    QPoint pos = mapFromGlobal(e->pos());
    QGraphicsItem *it = itemAt(pos);
    if (it != nullptr && isinstance<NodeItem>(it)) {
        QMenu *menu = new QMenu;
        QAction *hi = menu->addAction(tr("Editer"));
        QAction *act = menu->exec(mapToGlobal(pos));
        if (act == hi) {
        }
    }
}

void View::requestScaling(double v)
{
    if (v <= 20) {
        double calc = v/currentScale;
        scale(calc, calc);
        currentScale *= calc;
    }
}

void View::wheelEvent(QWheelEvent *e)
{
    if (pan) {
        return;
    }
    if (ctrlHold == false) {
        QGraphicsView::wheelEvent(e);
    } else {
        if(e->delta() > 0) {
            if (currentScale < 20) {
                scale(1.25, 1.25);
                currentScale *= 1.25;
                Q_EMIT scaled(currentScale);
            } else if (currentScale != 20.0) {
                double v = currentScale/20;
                if ((v*currentScale) <= 20) {
                    scale(v, v);
                    currentScale *= v;
                    Q_EMIT scaled(currentScale);
                }
            }
        } else {
            if ((currentScale * 0.8) >= 0.01) {
                scale(0.8, 0.8);
                currentScale *= 0.8;
                Q_EMIT scaled(currentScale);
            }
        }
    }
}

void View::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat("text/plain")) {
        e->accept();
    } else {
        e->ignore();
    }
}

void View::dropEvent(QDropEvent *e)
{
    Q_EMIT requestNode(e->mimeData()->text());
}

void View::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == Qt::CTRL) {
        if (!ctrlHold) {
            ctrlHold = true;
        }
    }
    QGraphicsView::keyPressEvent(e);
}

void View::keyReleaseEvent(QKeyEvent *e)
{
    if (e->modifiers() != Qt::CTRL) {
        if (ctrlHold) {
            ctrlHold = false;
        }
    }
    QGraphicsView::keyReleaseEvent(e);
}

void View::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton) {
        pan = true;
        pan_start_x = e->x();
        pan_start_y = e->y();
        setCursor(Qt::ClosedHandCursor);
    }
    return QGraphicsView::mousePressEvent(e);
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton) {
        pan = false;
        setCursor(Qt::ArrowCursor);
    }
    return QGraphicsView::mouseReleaseEvent(e);
}

void View::mouseMoveEvent(QMouseEvent *e)
{
    if (pan) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - e->x() + pan_start_x);
        verticalScrollBar()->setValue(verticalScrollBar()->value() - e->y() + pan_start_y);

        pan_start_x = e->x();
        pan_start_y = e->y();
    }

    return QGraphicsView::mouseMoveEvent(e);
}

void View::drawBackground(QPainter *p, const QRectF &rect)
{
    p->fillRect(rect, QPalette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Base));
    int left = int(rect.left()) - (int(rect.left()) % grid_size_fine);
    int top = int(rect.top()) - (int(rect.top()) % grid_size_fine);

    pen_l = QPen(QPalette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Foreground), 1);
    pen_s = QPen(QPalette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Foreground), 0.5);

    p->setPen(pen_s);
    double y = top, x = left;

    //Horizontal lines
    QVector<QLineF> lines;
    while (y < rect.bottom()) {
        lines << QLineF(rect.left(), y, rect.right(), y);
        y += grid_size_fine;
    }
    p->drawLines(lines);

    //Vertical lines
    lines.clear();
    while (x < rect.right()) {
        lines << QLineF(x, rect.top(), x, rect.bottom());
        x += grid_size_fine;
    }
    p->drawLines(lines);

    left = int(rect.left()) - (int(rect.left()) % grid_size_course);
    top = int(rect.top()) - (int(rect.top()) % grid_size_course);
    p->setPen(pen_l);

    //Horizontal lines
    y = top;
    lines.clear();
    while (y < rect.bottom()) {
        lines << QLineF(rect.left(), y, rect.right(), y);
        y += grid_size_course;
    }
    p->drawLines(lines);

    //Vertical lines
    x = left;
    lines.clear();
    while (x < rect.right()) {
        lines << QLineF(x, rect.top(), x, rect.bottom());
        x += grid_size_course;
    }
    p->drawLines(lines);

    return QGraphicsView::drawBackground(p, rect);
}
