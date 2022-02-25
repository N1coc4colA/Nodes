
QT       += widgets opengl gui
requires(qtConfig(fontcombobox))

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}

CONFIG += debug c++20
LIBS += -ldl

QMAKE_CXXFLAGS += -rdynamic

TARGET = Nodes
TEMPLATE = app
VERSION = 1.0.5.8

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS	    =   \
	colorpicker.h \
    connectionitem.h \
    linkeditor.h \
    linkstype.h \
    linksviewer.h \
    nodeconnector.h \
	nodeeditor.h \
	operationalmutex.h \
    portitem.h \
    relayer.h \
	sharedinstances.h \
	themeeditor.h \
    view.h \
    helpers.h \
    nodeitem.h \
    nodewidget.h \
    nodepalette.h \
    nodewindow.h \
    nodesdiscover.h \
    dom.h \
    nodesopener.h \
    loadingdialog.h \
    topbar.h

SOURCES	    =   \
	colorpicker.cpp \
	dom_in.cpp \
	dom_ops.cpp \
	dom_out.cpp \
	linkeditor.cpp \
	linkstype.cpp \
	linksviewer.cpp \
		main.cpp \
    connectionitem.cpp \
	nodeconnector.cpp \
	nodeeditor.cpp \
	operationalmutex.cpp \
    portitem.cpp \
	relayer.cpp \
	sharedinstances.cpp \
	themeeditor.cpp \
    view.cpp \
    nodeitem.cpp \
    nodewidget.cpp \
    nodepalette.cpp \
    nodewindow.cpp \
    nodesdiscover.cpp \
    nodesopener.cpp \
    helpers.cpp \
    loadingdialog.cpp \
    topbar.cpp

RESOURCES   = \
    main.qrc


# install
target.path = /usr/bin
INSTALLS += target
