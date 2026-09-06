QT += core gui widgets sql printsupport charts
QT       += core gui sql network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    connection.cpp \
    main.cpp \
    centredeformation.cpp \
    qrcodegen.cpp \
    salle.cpp \
    smtp.cpp \
    stagiaire.cpp

HEADERS += \
    connection.h \
    centredeformation.h \
    qrcodegen.hpp \
    salle.h \
    smtp.h \
    stagiaire.h

FORMS += \
    centredeformation.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
