#ifndef NODEPALETTE_H
#define NODEPALETTE_H

#include <QObject>
#include <QColor>

struct NodePaletteData
{
	QColor inner = QColor(150, 150, 150, 200);
	QColor outter = Qt::gray;
	QColor title = Qt::white;
	QColor type = Qt::white;
	QColor left = Qt::green;
	QColor right = Qt::green;

	bool autoatic = false;
};

class NodePalette : public QObject
{
	Q_OBJECT
public:
	explicit NodePalette(QObject *parent = nullptr);
	~NodePalette();

	QColor getInner();
	QColor getOutter();
	QColor getTitle();
	QColor getType();
	QColor getLeft();
	QColor getRight();

	NodePaletteData &getData();
	void setData(NodePaletteData d);

private:
	NodePaletteData m_data;
};

#endif // NODEPALETTE_H
