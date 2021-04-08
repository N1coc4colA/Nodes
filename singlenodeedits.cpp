#include "singlenodeedits.h"

#include "nodeitem.h"
#include "portitem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QSpacerItem>
#include <QAction>

#include <iostream>

SingleNodeEdits::SingleNodeEdits() : QWidget()
{
        QVBoxLayout *mainLayout = new QVBoxLayout;

        QVBoxLayout *titleLayout = new QVBoxLayout;
        QVBoxLayout *typeLayout = new QVBoxLayout;
        QHBoxLayout *bottomLayout = new QHBoxLayout;
        QLabel *title = new QLabel(tr("Title: "));
        QLabel *type = new QLabel(tr("Type: "));
        QPushButton *btn = new QPushButton(tr("Ok"));
        QPushButton *add = new QPushButton(tr("Add"));
        QPushButton *rm = new QPushButton(tr("Remove"));

        m_list = new QListWidget;
        m_type = new QLineEdit(storage->type);
        m_title = new QLineEdit(storage->title);

        titleLayout->addWidget(title);
        titleLayout->addWidget(m_title);
        typeLayout->addWidget(type);
        typeLayout->addWidget(m_type);
        bottomLayout->addWidget(rm);
        bottomLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum));
        bottomLayout->addWidget(add);

        mainLayout->addLayout(titleLayout);
        mainLayout->addLayout(typeLayout);
        mainLayout->addWidget(m_list);
        mainLayout->addLayout(bottomLayout);
        mainLayout->addWidget(btn);

        setLayout(mainLayout);

        connect(btn, &QPushButton::clicked, this, &SingleNodeEdits::applyChanges);
        connect(add, &QPushButton::clicked, this, [this]() {
            QListWidgetItem *it = new QListWidgetItem;
            it->setText("Vide");
            it->setFlags(it->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
            it->setCheckState(Qt::CheckState::Unchecked);
            m_list->addItem(it);
        });
        connect(rm, &QPushButton::clicked, this, [this]() {
            QList<QListWidgetItem *> list = m_list->selectedItems();
            int i = 0;
            while (i<list.length()) {
                delete m_list->takeItem(m_list->row(list[i]));
                i++;
            }
        });

        show();
}

void SingleNodeEdits::applyChanges()
{
    int i = 0;
    int len = m_list->count();
    m_target = new NodeItem;
    m_target->setTitle(m_title->text());
    m_target->setTextType(m_type->text());

    while (i<len) {
        QListWidgetItem *it = m_list->item(i);
        m_target->addPorts(it->text(), (it->checkState() == Qt::CheckState::Unchecked));
        i++;
    }

    m_target->build();
    Q_EMIT returned(m_target);
    this->close();
    storage->type = m_type->text();
    storage->title = m_title->text();
    this->~SingleNodeEdits();
}
