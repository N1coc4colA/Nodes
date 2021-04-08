QT += widgets opengl gui
requires(qtConfig(fontcombobox))

HEADERS	    =   \
    connectionitem.h \
    portitem.h \
    nodeeditor.h \
    view.h \
    helpers.h \
    nodeitem.h \
    nodewidget.h \
    nodepalette.h \
    nodewindow.h \
    singlenodeedits.h \
    nodesdiscover.h \
    dom.h \
    nodesopener.h \
    loadingdialog.h \
    topbar.h
SOURCES	    =   \
		main.cpp \
    connectionitem.cpp \
    portitem.cpp \
    nodeeditor.cpp \
    view.cpp \
    nodeitem.cpp \
    nodewidget.cpp \
    nodepalette.cpp \
    nodewindow.cpp \
    singlenodeedits.cpp \
    nodesdiscover.cpp \
    nodesopener.cpp \
    helpers.cpp \
    loadingdialog.cpp \
    topbar.cpp
RESOURCES   = \
    main.qrc


# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/graphicsview/diagramscene
INSTALLS += target
