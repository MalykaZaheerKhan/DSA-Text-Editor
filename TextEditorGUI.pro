QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextEditorGUI
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    LinkedList.cpp \
    Stack.cpp \
    Queue.cpp \
    TextEditor.cpp \
    FileHandler.cpp

HEADERS += \
    mainwindow.h \
    LinkedList.h \
    Stack.h \
    Queue.h \
    TextEditor.h \
    FileHandler.h

# Default rules for deployment
qnx: target.path = /tmp/${TARGET}/bin
else: unix:!android: target.path = /opt/${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
```
