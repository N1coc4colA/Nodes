#ifndef SINGLENODEEDITS_H
#define SINGLENODEEDITS_H

#include <QWidget>
#include <QLineEdit>

class NodeItem;
class QListWidget;

class oldStorage {
public:
    QString title = QObject::tr("Title");
    QString type = QObject::tr("Type");

    inline static oldStorage *instance() {
        static oldStorage *inst = new oldStorage;
        return inst;
    }
};

class SingleNodeEdits : public QWidget
{
    Q_OBJECT
public:
    explicit SingleNodeEdits();

Q_SIGNALS:
    void returned(NodeItem *);

public Q_SLOTS:
    void applyChanges();

private:
    NodeItem *m_target;

    QLineEdit *m_title;
    QLineEdit *m_type;
    QListWidget *m_list;
    oldStorage *storage = oldStorage::instance();
};



#endif // SINGLENODEEDITS_H
