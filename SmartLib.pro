QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AddBookDialog.cpp \
    Book.cpp \
    DatabaseManager.cpp \
    DeleteBookDialog.cpp \
    EditBookDialog.cpp \
    LoginWindow.cpp \
    RegisterWindow.cpp \
    Transaction.cpp \
    User.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AddBookDialog.h \
    Book.h \
    DatabaseManager.h \
    DeleteBookDialog.h \
    EditBookDialog.h \
    LoginWindow.h \
    RegisterWindow.h \
    Transaction.h \
    User.h \
    mainwindow.h

FORMS += \
    AddBookDialog.ui \
    DeleteBookDialog.ui \
    EditBookDialog.ui \
    LoginWindow.ui \
    RegisterWindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
