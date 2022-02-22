#ifndef LINKSVIEWER_H
#define LINKSVIEWER_H

#include <QListWidget>
#include <QSplitter>

class LinkEditor;

class LinksViewer : public QWidget
{
	Q_OBJECT
public:
	explicit LinksViewer(QWidget *parent = nullptr);
	~LinksViewer();

public Q_SLOTS:
	void handleShow();
	void handleLinksUpdate();
	void viewInheritanceOf(QString);

protected:
	void closeEvent(QCloseEvent *e) override;

private:
	QListWidget availableList;
	QListWidget inheritsList;
	QSplitter splitter;
	LinkEditor *lnEditor;
	bool updating = false;
};

#endif // LINKSVIEWER_H
