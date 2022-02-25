#include "view.h"

#include "helpers.h"
#include "nodeitem.h"
#include "nodeeditor.h"
#include "sharedinstances.h"
#include "relayer.h"

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
	QGraphicsItem *it = itemAt(e->pos());
	QMenu *menu = new QMenu;

	if (it != nullptr && isinstance<NodeItem>(it)) {
		QAction *edit = menu->addAction(tr("Edit"));
		QAction *suppr = menu->addAction(tr("Remove"));
		QAction *act = menu->exec(mapToGlobal(e->pos()));
		if (act == edit) {
			SharedInstances::instance()->relayer()->editNode(dynamic_cast<NodeItem *>(it));
		} else if (act == suppr) {
			SharedInstances::instance()->relayer()->rmElement();
		}
	} else {
		QAction *add = menu->addAction(tr("New"));
		QAction *act = menu->exec(mapToGlobal(e->pos()));
		if (act == add) {
			SharedInstances::instance()->relayer()->addNode();
		}
	}
	delete menu;
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
		if(e->angleDelta().y() > 0 || e->angleDelta().x() > 0) {
			if (currentScale < 20) {
				scale(1.1, 1.1);
				ensureVisibility();
				currentScale *= 1.1;
				SharedInstances::instance()->relayer()->dispScaled(currentScale);
            } else if (currentScale != 20.0) {
                double v = currentScale/20;
                if ((v*currentScale) <= 20) {
                    scale(v, v);
					ensureVisibility();
                    currentScale *= v;
					SharedInstances::instance()->relayer()->dispScaled(currentScale);
                }
            }
        } else {
			if ((currentScale * 0.9) >= 0.01) {
				scale(0.9, 0.9);
				ensureVisibility();
				currentScale *= 0.9;
				SharedInstances::instance()->relayer()->dispScaled(currentScale);
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
	if (e->key() == Qt::Key::Key_Control) {
        if (!ctrlHold) {
            ctrlHold = true;
        }
		e->accept();
    }
	return QGraphicsView::keyPressEvent(e);
}

void View::keyReleaseEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key::Key_Control) {
        if (ctrlHold) {
            ctrlHold = false;
        }
		e->accept();
    }
	return QGraphicsView::keyReleaseEvent(e);
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

void View::ensureVisibility()
{
	QPoint p = mapFromGlobal(QCursor::pos());
	if ((p.x() < width() && p.x() > -1) && (p.y() < height() && p.y() > -1)) {
		QRect r(p.x() - width()/2, p.y() - height()/2, width()/2, height()/2);
		QPointF t = mapToScene(r.topLeft()), b = mapToScene(r.bottomRight());
		ensureVisible(QRectF(t, b));
	}
}
