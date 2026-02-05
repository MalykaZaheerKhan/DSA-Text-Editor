#include "mainwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QClipboard>
#include <QStyle>
#include <QPainter>
#include <QTextBlock>tfc
#include <QCloseEvent>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

//  CodeEditor Implementation

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CoeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // Set font
    QFont font("Courier New", 10);
    setFont(font);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QPlainTextEdit::keyPressEvent(event);
        emit enterPressed();
    } else {
        QPlainTextEdit::keyPressEvent(event);
    }
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(120, 120, 120));
            painter.drawText(0, top, lineNumberArea->width() - 5, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

// ===================== MainWindow Implementation =====================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , isModified(false)
    , isSyncing(false)
{
    setupUI();
    createMenuBar();
    createToolBar();

    setWindowTitle("DSA Text Editor - Untitled");
    resize(900, 650);

    updateStatusBar();

    // Initialize backend with empty state
    syncBackendWithGUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Text Editor with line numbers
    textEdit = new CodeEditor(this);
    textEdit->setPlaceholderText("Start typing here...");
    mainLayout->addWidget(textEdit);

    // Status bar
    statusLabel = new QLabel("Lines: 0 | Characters: 0");
    statusBar()->addWidget(statusLabel);

    statusBar()->addPermanentWidget(new QLabel("  "));

    cursorPosLabel = new QLabel("Ln 1, Col 1");
    statusBar()->addPermanentWidget(cursorPosLabel);

    // Connect signals
    connect(textEdit, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(textEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onCursorPositionChanged);
    connect(textEdit, &CodeEditor::enterPressed, this, &MainWindow::onEnterPressed);
}

void MainWindow::createMenuBar()
{
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* newAction = fileMenu->addAction("&New");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewFile);

    QAction* openAction = fileMenu->addAction("&Open...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);

    QAction* saveAction = fileMenu->addAction("&Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);

    QAction* saveAsAction = fileMenu->addAction("Save &As...");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAsFile);

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Edit Menu
    QMenu* editMenu = menuBar()->addMenu("&Edit");

    QAction* undoAction = editMenu->addAction("&Undo");
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, &MainWindow::onUndo);

    QAction* redoAction = editMenu->addAction("&Redo");
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, &MainWindow::onRedo);

    editMenu->addSeparator();

    QAction* cutAction = editMenu->addAction("Cu&t");
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &MainWindow::onCut);

    QAction* copyAction = editMenu->addAction("&Copy");
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MainWindow::onCopy);

    QAction* pasteAction = editMenu->addAction("&Paste");
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, &MainWindow::onPaste);
}

void MainWindow::createToolBar()
{
    QToolBar* toolbar = addToolBar("Main Toolbar");
    toolbar->setMovable(false);

    QAction* newAction = toolbar->addAction(
        QApplication::style()->standardIcon(QStyle::SP_FileIcon), "New");
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewFile);

    QAction* openAction = toolbar->addAction(
        QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon), "Open");
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);

    QAction* saveAction = toolbar->addAction(
        QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton), "Save");
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveFile);

    toolbar->addSeparator();

    QAction* undoAction = toolbar->addAction(
        QApplication::style()->standardIcon(QStyle::SP_ArrowBack), "Undo");
    connect(undoAction, &QAction::triggered, this, &MainWindow::onUndo);

    QAction* redoAction = toolbar->addAction(
        QApplication::style()->standardIcon(QStyle::SP_ArrowForward), "Redo");
    connect(redoAction, &QAction::triggered, this, &MainWindow::onRedo);

    toolbar->addSeparator();

    QAction* cutAction = toolbar->addAction("Cut");
    connect(cutAction, &QAction::triggered, this, &MainWindow::onCut);

    QAction* copyAction = toolbar->addAction("Copy");
    connect(copyAction, &QAction::triggered, this, &MainWindow::onCopy);

    QAction* pasteAction = toolbar->addAction("Paste");
    connect(pasteAction, &QAction::triggered, this, &MainWindow::onPaste);
}

void MainWindow::onNewFile()
{
    if (isModified && !saveChangesDialog()) {
        return;
    }

    isSyncing = true;
    textEdit->clear();
    editor = TextEditor();
    currentFilePath.clear();
    setWindowTitle("DSA Text Editor - Untitled");
    isModified = false;
    isSyncing = false;

    syncBackendWithGUI();
    updateStatusBar();
}

void MainWindow::onOpenFile()
{
    if (isModified && !saveChangesDialog()) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "",
                                                    "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        editor.loadFile(fileName.toStdString());
        currentFilePath = fileName;
        setWindowTitle("DSA Text Editor - " + QFileInfo(fileName).fileName());
        isModified = false;

        syncGUIWithBackend();
        updateStatusBar();
    }
}

void MainWindow::onSaveFile()
{
    if (currentFilePath.isEmpty()) {
        onSaveAsFile();
    } else {
        syncBackendWithGUI();
        editor.saveFile(currentFilePath.toStdString());
        isModified = false;
        setWindowTitle("DSA Text Editor - " + QFileInfo(currentFilePath).fileName());
        QMessageBox::information(this, "Success", "File saved successfully!");
    }
}

void MainWindow::onSaveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File As", "",
                                                    "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        syncBackendWithGUI();
        editor.saveFile(fileName.toStdString());
        currentFilePath = fileName;
        isModified = false;
        setWindowTitle("DSA Text Editor - " + QFileInfo(fileName).fileName());
        QMessageBox::information(this, "Success", "File saved successfully!");
    }
}

void MainWindow::onUndo()
{
    // Call backend undo (it handles saving current state to redo stack)
    editor.undo();

    // Load the undone state back to GUI
    syncGUIWithBackend();

    setModified(true);
    updateStatusBar();
}

void MainWindow::onRedo()
{
    // Call backend redo (it handles saving current state to undo stack)
    editor.redo();

    // Load the redone state back to GUI
    syncGUIWithBackend();

    setModified(true);
    updateStatusBar();
}

void MainWindow::onCut()
{
    // Save state before cut
    syncBackendWithGUI();

    onCopy();
    textEdit->textCursor().removeSelectedText();

    // Update backend after cut
    syncBackendWithGUI();
}

void MainWindow::onCopy()
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection()) {
        return;
    }

    QString selectedText = cursor.selectedText();
    QApplication::clipboard()->setText(selectedText.replace(QChar::ParagraphSeparator, "\n"));
}

void MainWindow::onPaste()
{
    QString clipboardText = QApplication::clipboard()->text();
    if (!clipboardText.isEmpty()) {
        // Save state before paste
        syncBackendWithGUI();

        textEdit->insertPlainText(clipboardText);

        // Update backend after paste
        syncBackendWithGUI();
    }
}

void MainWindow::onTextChanged()
{
    if (!isSyncing) {
        setModified(true);
    }
    updateStatusBar();
}

void MainWindow::onCursorPositionChanged()
{
    QTextCursor cursor = textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    cursorPosLabel->setText(QString("Ln %1, Col %2").arg(line).arg(col));
}

void MainWindow::onEnterPressed()
{
    // Save current state to undo stack when Enter is pressed
    Node* currentState = editor.getText().copyList();
    if (currentState != nullptr) {
        // Push current state to undo stack using TextEditor's stack
        editor.insertText("", 1);  // This will push current state to undo stack
        editor.getText().replaceList(currentState);  // Restore current state
    }
}

void MainWindow::syncGUIWithBackend()
{
    isSyncing = true;

    Node* temp = editor.getText().getHead();
    QString text;

    while (temp != nullptr) {
        text += QString::fromStdString(temp->data);
        if (temp->next != nullptr) {
            text += "\n";
        }
        temp = temp->next;
    }

    textEdit->setPlainText(text);
    isSyncing = false;
}

void MainWindow::syncBackendWithGUI()
{
    if (isSyncing) return;

    QString text = textEdit->toPlainText();
    QStringList lines = text.split('\n');

    // Build new linked list
    LinkedList newList;

    int position = 1;
    for (const QString& line : lines) {
        newList.insertLine(line.toStdString(), position);
        position++;
    }

    // Replace editor's text with new list
    editor.getText().replaceList(newList.copyList());
}

void MainWindow::setModified(bool modified)
{
    isModified = modified;
    QString title = "DSA Text Editor - ";
    if (!currentFilePath.isEmpty()) {
        title += QFileInfo(currentFilePath).fileName();
    } else {
        title += "Untitled";
    }
    if (isModified) {
        title += " *";
    }
    setWindowTitle(title);
}

bool MainWindow::saveChangesDialog()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Unsaved Changes",
        "Do you want to save your changes?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
        onSaveFile();
        return true;
    } else if (reply == QMessageBox::No) {
        return true;
    }

    return false;
}

void MainWindow::updateStatusBar()
{
    QString text = textEdit->toPlainText();
    int lineCount = textEdit->document()->blockCount();
    int charCount = text.length();

    statusLabel->setText(QString("Lines: %1 | Characters: %2").arg(lineCount).arg(charCount));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (isModified && !saveChangesDialog()) {
        event->ignore();
    } else {
        event->accept();
    }
}
