#ifndef NODEEDITOR_H
#define NODEEDITOR_H

#include <QWidget>
#include <QLineEdit>
#include <QStyledItemDelegate>

class ConnectionItem;
class PortItem;
class NodeItem;

class NodeEditorDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	NodeEditorDelegate(QObject *parent = nullptr);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						  const QModelIndex &index) const override;

	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const override;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
							  const QModelIndex &index) const override;
};

class NodeItem;
class QTableWidget;

class oldStorage {
public:
    QString title = QObject::tr("Title");
    QString type = QObject::tr("Type");

    inline static oldStorage *instance() {
        static oldStorage *inst = new oldStorage;
        return inst;
    }
};

class NodeEditor : public QWidget
{
    Q_OBJECT
public:
	explicit NodeEditor(NodeItem *src = nullptr);

Q_SIGNALS:
    void returned(NodeItem *);

public Q_SLOTS:
    void applyChanges();

protected:
	void closeEvent(QCloseEvent *e);

private:
    NodeItem *m_target;

    QLineEdit *m_title;
    QLineEdit *m_type;
	QTableWidget *m_list;
	NodeEditorDelegate *dlg;

    oldStorage *storage = oldStorage::instance();
};



#endif // NODEEDITOR_H
