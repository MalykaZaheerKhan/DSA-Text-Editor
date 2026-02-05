#include "TextEditor.h"
#include "FileHandler.h"
#include <iostream>
using namespace std;

// Insert line at position with undo support
void TextEditor::insertText(string line, int position) {
    undoStack.push(text.copyList()); 
    text.insertLine(line, position);
}

// Delete line at position with undo support
void TextEditor::deleteLine(int position) {
    undoStack.push(text.copyList()); 
    text.deleteLine(position);
}

// Undo last operation
void TextEditor::undo() {
    if (undoStack.isEmpty()) {
        cout << "Nothing to undo.\n";
        return;
    }
    redoStack.push(text.copyList());
    Node* prev = undoStack.pop();
    text.replaceList(prev);
    cout << "Undo successful.\n";
}

// Redo last undone operation
void TextEditor::redo() {
    if (redoStack.isEmpty()) {
        cout << "Nothing to redo.\n";
        return;
    }
    undoStack.push(text.copyList());
    Node* next = redoStack.pop();
    text.replaceList(next);
    cout << "Redo successful.\n";
}

// Display all text
void TextEditor::display() {
    text.display();
}

// Save text to file
void TextEditor::saveFile(const string& filename) {
    FileHandler::saveToFile(filename, text.getHead());
}

// Load text from file
void TextEditor::loadFile(const string& filename) {
    Node* loadedText = FileHandler::loadFromFile(filename);
    if (loadedText != NULL) {
        undoStack.push(text.copyList());
        text.replaceList(loadedText);
    }
}
//copy-cut-paste

// Copy 
void TextEditor::copy(int start, int end) {
    int count = text.getCount();

    if (count == 0) {
        cout << "Text is empty! Nothing to copy.\n";
        return;
    }

    if (start < 1 || end > count || start > end) {
        cout << "Invalid range!\n";
        return;
    }
    clipboard.clear();

    Node* temp = text.getHead();
    for (int i = 1; i < start; i++) {
        temp = temp->next;
    }

    for (int i = start; i <= end; i++) {
        clipboard.enqueue(temp->data);
        temp = temp->next;
    }
}

// Cut 
void TextEditor::cut(int start, int end) {
    int count = text.getCount();

    if (count == 0) {
        cout << "Text is empty! Nothing to cut.\n";
        return;
    }

    if (start < 1 || end > count || start > end) {
        cout << "Invalid range!\n";
        return;
    }

    clipboard.clear();
    undoStack.push(text.copyList());

    Node* temp = text.getHead();
    for (int i = 1; i < start; i++) {
        temp = temp->next;
    }

    // Copy lines to clipboard and delete from linked list
    int linesToDelete = end - start + 1;
    for (int i = 0; i < linesToDelete; i++) {
        clipboard.enqueue(temp->data);
        text.deleteLine(start); 
        temp = text.getHead();
        for (int j = 1; j < start; j++) 
        temp = temp->next;
    }
}

// Paste clipboard content at given position
void TextEditor::paste(int position) {
    if (clipboard.isEmpty()) {
        cout << "Clipboard is empty! Nothing to paste.\n";
        return;
    }

    int count = text.getCount();
    if (position < 1 || position > count + 1) {
        cout << "Invalid position!\n";
        return;
    }
    undoStack.push(text.copyList());

    Queue tempQueue;  // temporary queue to preserve clipboard
    int pastePos = position;
    int linesPasted = 0;

    while (!clipboard.isEmpty()) {
        string line = clipboard.dequeue();
        tempQueue.enqueue(line);        
        text.insertLine(line, pastePos);
        pastePos++;
        linesPasted++;
    }

    // Restore clipboard
    while (!tempQueue.isEmpty()) {
        clipboard.enqueue(tempQueue.dequeue());
    }

}
