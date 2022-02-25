#include "themeeditor.h"

#include "nodepalette.h"
#include "relayer.h"
#include "sharedinstances.h"

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>


QHBoxLayout *build(QString text, QWidget *editor, QWidget *parent = nullptr)
{
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(editor);
	layout->addWidget(new QLabel(text, parent));
	return layout;
}

ThemeEditor::ThemeEditor(QWidget *parent)
	: QWidget(parent), container(new QWidget(this)), automatic(new QCheckBox(tr(""), this)),
	  pTitle(new ColorPicker(this)), pType(new ColorPicker(this)),
	  pOutter(new ColorPicker(this)), pInner(new ColorPicker(this)),
	  pRight(new ColorPicker(this)), pLeft(new ColorPicker(this))
{
	QVBoxLayout *mainL = new QVBoxLayout(this);
	QVBoxLayout *subL = new QVBoxLayout(container);

	mainL->addLayout(build(tr("Automatic theming"), automatic, this));
	subL->addLayout(build(tr("Titles"), pTitle, container));
	subL->addLayout(build(tr("Types"), pType, container));
	subL->addLayout(build(tr("Nodes"), pInner, container));
	subL->addLayout(build(tr("Nodes' border"), pOutter, container));
	subL->addLayout(build(tr("Inputs' text color"), pLeft, container));
	subL->addLayout(build(tr("Outputs' text color"), pRight, container));

	container->setLayout(subL);
	mainL->addWidget(container);
	setLayout(mainL);

	setWindowFlags(Qt::Dialog | windowFlags());
	setWindowModality(Qt::WindowModality::ApplicationModal);
	hide();

	loadColors();
	connect(SharedInstances::instance()->relayer(), &Relayer::nodePaletteChanged, this, &ThemeEditor::loadColors);
	connect(automatic, &QCheckBox::stateChanged, this, &ThemeEditor::changeAutomaticTheming);
	connect(pTitle, &ColorPicker::colorChanged, this, &ThemeEditor::setColors);
	connect(pType, &ColorPicker::colorChanged, this, &ThemeEditor::setColors);
	connect(pInner, &ColorPicker::colorChanged, this, &ThemeEditor::setColors);
	connect(pOutter, &ColorPicker::colorChanged, this, &ThemeEditor::setColors);
	connect(pRight, &ColorPicker::colorChanged, this, &ThemeEditor::setColors);
	connect(pLeft, &ColorPicker::colorChanged, this, &ThemeEditor::setColors);
}

void ThemeEditor::loadColors()
{
	if (updatedRightNow) {
		updatedRightNow = false;
		return;
	}
	pTitle->setColor(SharedInstances::instance()->nodePalette()->getTitle());
	pType->setColor(SharedInstances::instance()->nodePalette()->getType());
	pOutter->setColor(SharedInstances::instance()->nodePalette()->getOutter());
	pInner->setColor(SharedInstances::instance()->nodePalette()->getInner());
	pRight->setColor(SharedInstances::instance()->nodePalette()->getRight());
	pLeft->setColor(SharedInstances::instance()->nodePalette()->getLeft());
}

void ThemeEditor::setColors()
{
	NodePaletteData d = SharedInstances::instance()->nodePalette()->getData();

	d.title = pTitle->color();
	d.type = pType->color();
	d.inner = pInner->color();
	d.outter = pOutter->color();
	d.right = pRight->color();
	d.left = pLeft->color();

	updatedRightNow = true;
	SharedInstances::instance()->nodePalette()->setData(d);
}

void ThemeEditor::setColorsFromTheme()
{
	NodePaletteData d = SharedInstances::instance()->nodePalette()->getData();

	d.title = palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Text);
	d.type = palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark);
	d.inner = palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Light);
	d.outter = palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Base);
	d.right = palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText);
	d.left = palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText);

	updatedRightNow = true;
	SharedInstances::instance()->nodePalette()->setData(d);

	loadColors();
}

void ThemeEditor::changeAutomaticTheming(int)
{
	SharedInstances::instance()->nodePalette()->getData().autoatic = automatic->isChecked();
	container->setEnabled(!automatic->isChecked());

	if (SharedInstances::instance()->nodePalette()->getData().autoatic) {
		setColorsFromTheme();
	}
}

bool ThemeEditor::event(QEvent *e)
{
	if (e->type() == QEvent::ThemeChange && SharedInstances::instance()->nodePalette()->getData().autoatic) {
		setColorsFromTheme();
	}
	return QWidget::event(e);
}

void ThemeEditor::closeEvent(QCloseEvent *e)
{
	SharedInstances::instance()->relayer()->enable();
	return QWidget::closeEvent(e);
}

void ThemeEditor::showEvent(QShowEvent *e)
{
	SharedInstances::instance()->relayer()->disable();
	setEnabled(true);
	return QWidget::showEvent(e);
}
