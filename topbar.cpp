#include "topbar.h"

#include "themeeditor.h"
#include "relayer.h"
#include "sharedinstances.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>


TopBar::TopBar(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	m_author = new QLineEdit(this);
    m_author->setPlaceholderText(tr("Author's name"));

	scale = new QDoubleSpinBox(this);
    scale->setMaximum(20);
    scale->setMinimum(0.01);
    scale->setValue(1);
    scale->setSingleStep(0.1);

	ThemeEditor *editor = new ThemeEditor(parent);

	QPushButton *addObj = new QPushButton(this);
	QPushButton *open = new QPushButton(this);
	QPushButton *save = new QPushButton(this);
	QPushButton *listL = new QPushButton(this);
	QPushButton *colors = new QPushButton(this);

    open->setIcon(QIcon(":/folder-open"));
    save->setIcon(QIcon(":/save"));
    addObj->setIcon(QIcon(":/plus"));
	listL->setIcon(QIcon(":/l-list"));
	colors->setIcon(QIcon(":/palette"));
    open->setToolTip(tr("Open file"));
    save->setToolTip(tr("Save file"));
    addObj->setToolTip(tr("Add a node"));
	listL->setToolTip(tr("See links"));
	colors->setToolTip(tr("Change nodes' theme"));

	mainLayout->addWidget(save);
	mainLayout->addWidget(open);
    mainLayout->addWidget(addObj);
	mainLayout->addWidget(listL);
	mainLayout->addWidget(colors);
    mainLayout->addWidget(scale);
	mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum));
	mainLayout->addWidget(m_author, 0, Qt::AlignmentFlag::AlignLeft);

    mainLayout->setSpacing(2);
    mainLayout->setMargin(2);

	connect(listL, &QPushButton::clicked, SharedInstances::instance()->relayer(), &Relayer::showLinksList);
	connect(addObj, &QPushButton::clicked, SharedInstances::instance()->relayer(), &Relayer::addNode);
	connect(open, &QPushButton::clicked, SharedInstances::instance()->relayer(), &Relayer::runLoad);
	connect(save, &QPushButton::clicked, SharedInstances::instance()->relayer(), &Relayer::runSave);
	connect(colors, &QPushButton::clicked, editor, &ThemeEditor::show);
	connect(scale, qOverload<double>(&QDoubleSpinBox::valueChanged), SharedInstances::instance()->relayer(), &Relayer::setScaling);
	connect(SharedInstances::instance()->relayer(), &Relayer::scaled, this, &TopBar::updateScaling);

    setMaximumHeight(30);
    setLayout(mainLayout);
}

void TopBar::updateScaling(double scaling)
{
    scale->setValue(scaling);
}

void TopBar::setAuthor(QString name)
{
    m_author->setText(name);
}

QString TopBar::author()
{
    return m_author->text();
}
