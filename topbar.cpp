#include "topbar.h"

#include "relayer.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>

TopBar::TopBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
	m_author = new QLineEdit(this);
    m_author->setPlaceholderText(tr("Author's name"));

	scale = new QDoubleSpinBox(this);
    scale->setMaximum(20);
    scale->setMinimum(0.01);
    scale->setValue(1);
    scale->setSingleStep(0.1);

	QPushButton *addObj = new QPushButton(this);
	QPushButton *open = new QPushButton(this);
	QPushButton *save = new QPushButton(this);
	QPushButton *listL = new QPushButton(this);

    open->setIcon(QIcon(":/folder-open"));
    save->setIcon(QIcon(":/save"));
    addObj->setIcon(QIcon(":/plus"));
	listL->setIcon(QIcon(":/l-list"));
    open->setToolTip(tr("Open file"));
    save->setToolTip(tr("Save file"));
    addObj->setToolTip(tr("Add a node"));
	listL->setToolTip(tr("See links"));

    mainLayout->addWidget(addObj);
	mainLayout->addWidget(listL);
    mainLayout->addWidget(scale);
	mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum));
    mainLayout->addWidget(m_author, 0, Qt::AlignmentFlag::AlignLeft);
    mainLayout->addWidget(save);
    mainLayout->addWidget(open);

    mainLayout->setSpacing(2);
    mainLayout->setMargin(2);

	connect(listL, &QPushButton::clicked, Relayer::instance(), &Relayer::showLinksList);
	connect(addObj, &QPushButton::clicked, Relayer::instance(), &Relayer::addNode);
	connect(open, &QPushButton::clicked, Relayer::instance(), &Relayer::runLoad);
	connect(save, &QPushButton::clicked, Relayer::instance(), &Relayer::runSave);
	connect(scale, qOverload<double>(&QDoubleSpinBox::valueChanged), Relayer::instance(), &Relayer::setScaling);
	connect(Relayer::instance(), &Relayer::scaled, this, &TopBar::updateScaling);

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
