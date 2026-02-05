#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLabel>
#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include "TextEditor.h"

// Forward declarations
class LineNumberArea;

// ===================== CodeEditor Class =====================
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

signals:
    void enterPressed();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
};

// ===================== LineNumberArea Class =====================
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

// ===================== MainWindow Class =====================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File operations
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveAsFile();

    // Edit operations
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();

    // Text events
    void onTextChanged();
    void onCursorPositionChanged();
    void onEnterPressed();

private:
    void setupUI();
    void createMenuBar();
    void createToolBar();
    void syncGUIWithBackend();
    void syncBackendWithGUI();
    void setModified(bool modified);
    bool saveChangesDialog();
    void updateStatusBar();

    // Widgets
    CodeEditor* textEdit;
    QLabel* statusLabel;
    QLabel* cursorPosLabel;

    // Backend
    TextEditor editor;
    QString currentFilePath;
    bool isModified;
    bool isSyncing;
};

#endif // MAINWINDOW_H
