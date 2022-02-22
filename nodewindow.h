#ifndef NODEWINDOW_H
#define NODEWINDOW_H

#include <QMainWindow>

class LinksViewer;

class NodeWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit NodeWindow(QWidget *parent = nullptr);
	~NodeWindow();

private:
	LinksViewer *vwr;
};

#endif // NODEWINDOW_H
