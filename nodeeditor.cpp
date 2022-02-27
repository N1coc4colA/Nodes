#include "nodeeditor.h"

#include "nodeitem.h"
#include "portitem.h"
#include "linkstype.h"
#include "relayer.h"
#include "sharedinstances.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QSpacerItem>
#include <QAction>
#include <QHeaderView>
#include <QComboBox>
#include <QCheckBox>


NodeEditorDelegate::NodeEditorDelegate(QObject *p) : QStyledItemDelegate(p)
{
}

QWidget *NodeEditorDelegate::createEditor(QWidget *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch (index.column()) {
	case 1: {
		QCheckBox *bx = new QCheckBox(p);
		return bx;
	}
	case 2: {
		QComboBox *bx = new QComboBox(p);
		bx->addItems(SharedInstances::instance()->typesHolder()->typeNames());
		return bx;
	}
	default: {
		return QStyledItemDelegate::createEditor(p, option, index);
	}
	}
}

void NodeEditorDelegate::setEditorData(QWidget *edit, const QModelIndex &index) const
{
	switch(index.column()) {
	case 1: {
		QCheckBox *bx = static_cast<QCheckBox*>(edit);
		bx->setCheckState(index.data(Qt::CheckStateRole).value<Qt::CheckState>());
		break;
	}
	case 2: {
		QComboBox *bx = static_cast<QComboBox *>(edit);
		QString v = index.data(Qt::DisplayRole+2).toString();
		if (!SharedInstances::instance()->typesHolder()->typeNames().contains(v)) {
			v = "?";
		}
		bx->setCurrentText(v);
		break;
	}
	default: {
		return QStyledItemDelegate::setEditorData(edit, index);
	}
	}
}

void NodeEditorDelegate::setModelData(QWidget *edit, QAbstractItemModel *model, const QModelIndex &index) const
{
	switch(index.column()) {
	case 1: {
		QCheckBox *bx = static_cast<QCheckBox*>(edit);
		model->setData(index, bx->checkState());
		break;
	}
	case 2: {
		QComboBox *bx = static_cast<QComboBox *>(edit);
		QString v = bx->currentText();
		if (!SharedInstances::instance()->typesHolder()->typeNames().contains(v)) {
			v = "?";
		}
		model->setData(index, v);
		break;
	}
	default: {
		return QStyledItemDelegate::setModelData(edit, model, index);
	}
	}
}

void NodeEditorDelegate::updateEditorGeometry(QWidget *editor,
										   const QStyleOptionViewItem &option,
										   const QModelIndex &) const
{
	editor->setGeometry(option.rect);
}

NodeEditor::NodeEditor(NodeItem *src)
	: QWidget(),
	  m_list(new QTableWidget(this))
{
	m_title = new QLineEdit(storage->title, this);
	m_type = new QLineEdit(storage->type, this);
	m_list->setItemDelegate(new NodeEditorDelegate(m_list));

	m_list->setAlternatingRowColors(true);
	m_list->setColumnCount(3);
	m_list->model()->setHeaderData(0, Qt::Horizontal, "", Qt::ItemDataRole::DisplayRole);
	m_list->model()->setHeaderData(1, Qt::Horizontal, "", Qt::ItemDataRole::EditRole);
	m_list->model()->setHeaderData(2, Qt::Horizontal, "", Qt::ItemDataRole::EditRole);

	m_list->setHorizontalHeaderLabels({tr("Name"), tr("Input"), tr("Type")});

	SharedInstances::instance()->relayer()->disable();
	m_target = src;
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QVBoxLayout *titleLayout = new QVBoxLayout;
	QVBoxLayout *typeLayout = new QVBoxLayout;
	QHBoxLayout *bottomLayout = new QHBoxLayout;
	QLabel *title = new QLabel(tr("Title: "), this);
	QLabel *type = new QLabel(tr("Type: "), this);
	QPushButton *btn = new QPushButton(tr("Ok"), this);
	QPushButton *add = new QPushButton(tr("Add"), this);
	QPushButton *rm = new QPushButton(tr("Remove"), this);

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
	setWindowModality(Qt::WindowModality::ApplicationModal);
	setWindowFlags(Qt::Tool | Qt::Dialog);

	connect(btn, &QPushButton::clicked, this, &NodeEditor::applyChanges);
	connect(add, &QPushButton::clicked, this, [this]() {
		m_list->model()->insertRow(m_list->model()->rowCount());
		int r = m_list->model()->rowCount()-1;

		QTableWidgetItem *a = new QTableWidgetItem;
		QTableWidgetItem *b = new QTableWidgetItem;
		QTableWidgetItem *c = new QTableWidgetItem;

		a->setText(tr("Empty"));
		b->setFlags(b->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		b->setCheckState(Qt::CheckState::Unchecked);
		c->setText("?");

		m_list->setItem(r, 0, a);
		m_list->setItem(r, 1, b);
		m_list->setItem(r, 2, c);
	});
	connect(rm, &QPushButton::clicked, this, [this]() {
		m_list->clear();
	});

	if (m_target) {
		m_title->setText(src->title());
		m_type->setText(src->textType());

		QList<PortItem *> l = src->ports();
		for (auto e : l) {
			m_list->model()->insertRow(m_list->model()->rowCount());
			int r = m_list->model()->rowCount()-1;
			QTableWidgetItem *a = new QTableWidgetItem;
			QTableWidgetItem *b = new QTableWidgetItem;
			QTableWidgetItem *c = new QTableWidgetItem;

			a->setText(e->name());
			b->setFlags(b->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
			b->setCheckState(e->isOutput() ? Qt::CheckState::Unchecked : Qt::CheckState::Checked);
			c->setText(e->lnType().type->name);

			m_list->setItem(r, 0, a);
			m_list->setItem(r, 1, b);
			m_list->setItem(r, 2, c);
		}
	}

	m_title->setFocus();
	show();
}

void NodeEditor::applyChanges()
{
	if (!m_target) {
		m_target = new NodeItem;
	}
    m_target->setTitle(m_title->text());
    m_target->setTextType(m_type->text());

	QList<PortItem *> l = m_target->ports();

	int i = 0;
	int len = m_list->rowCount();

	//Remove all ports first or it might cause a UI bug.
	while(!m_target->ports().isEmpty()) {
		m_target->removePort(m_target->ports()[0]);
	}

	while (i<len) {
		m_target->addPort(m_list->model()->data(m_list->model()->index(i, 0)).toString(),
						   (m_list->model()->data(m_list->model()->index(i, 1), Qt::CheckStateRole).value<Qt::CheckState>() == Qt::CheckState::Unchecked),
						   SharedInstances::instance()->typesHolder()->getByName(m_list->model()->data(m_list->model()->index(i, 2)).toString()));
        i++;
    }

    m_target->build();
	Q_EMIT returned(m_target);
    storage->type = m_type->text();
	storage->title = m_title->text();
	this->close();
}

void NodeEditor::closeEvent(QCloseEvent *e)
{
	SharedInstances::instance()->relayer()->enable();
	QWidget::closeEvent(e);
	deleteLater();
}
