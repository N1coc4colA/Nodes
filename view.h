#ifndef VIEW_H
#define VIEW_H

#include <QPen>
#include <QColor>
#include <QTimeLine>
#include <QGraphicsView>

class View : public QGraphicsView
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = nullptr);

Q_SIGNALS:
	void requestNode(QString str);

public Q_SLOTS:
    void requestScaling(double scaling);
	void ensureVisibility();

protected:
    void wheelEvent(QWheelEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void drawBackground(QPainter *p, const QRectF &r) override;
    void keyPressEvent(QKeyEvent *e) override;
	void keyReleaseEvent(QKeyEvent *e) override;

private:
    QColor bg = QColor(220, 220, 220);
    QPen pen_l = QPen(QColor(140, 140, 140), 1);
    QPen pen_s = QPen(QColor(150, 150, 150), 0.5);

    int grid_size_fine = 15;
    int grid_size_course = 150;
    int pan_start_x = 0;
    int pan_start_y = 0;
    bool pan = false;
    QPoint lastMousePos;

    double currentScale = 1;
    bool ctrlHold = false;
    int manipulationMode = 0;
};

#endif // VIEW_H
