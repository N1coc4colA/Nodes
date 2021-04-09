#include "topbar.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>

TopBar::TopBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    m_author = new QLineEdit;
    m_author->setPlaceholderText(tr("Author's name"));

    scale = new QDoubleSpinBox;
    scale->setMaximum(20);
    scale->setMinimum(0.01);
    scale->setValue(1);
    scale->setSingleStep(0.1);

    QPushButton *addObj = new QPushButton;
    QPushButton *open = new QPushButton;
    QPushButton *save = new QPushButton;

    open->setIcon(QIcon(":/folder-open"));
    save->setIcon(QIcon(":/save"));
    addObj->setIcon(QIcon(":/plus"));
    open->setToolTip(tr("Open file"));
    save->setToolTip(tr("Save file"));
    addObj->setToolTip(tr("Add a node"));

    mainLayout->addWidget(addObj);
    mainLayout->addWidget(scale);
    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum));
    mainLayout->addWidget(m_author, 0, Qt::AlignmentFlag::AlignLeft);
    mainLayout->addWidget(save);
    mainLayout->addWidget(open);

    mainLayout->setSpacing(2);
    mainLayout->setMargin(2);

    connect(addObj, &QPushButton::clicked, this, &TopBar::generateNode);
    connect(open, &QPushButton::clicked, this, &TopBar::runLoad);
    connect(save, &QPushButton::clicked, this, &TopBar::runSave);
    connect(scale, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &TopBar::setScaling);

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
