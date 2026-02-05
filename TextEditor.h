#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "LinkedList.h"
#include "Stack.h"
#include "Queue.h"

class TextEditor {
private:
    LinkedList text;
    Stack undoStack;
    Stack redoStack;
    Queue clipboard;

public:
    // Basic operations  //linkedlist //stack
    void insertText(string line, int position);
    void deleteLine(int position);
    void undo();
    void redo();
    void display();
    
    // File operations
    void saveFile(const string& filename);
    void loadFile(const string& filename);
    
    // Copy/Paste operations using Queue
    void copy(int start, int end);  
    void cut(int start,int end);      
    void paste(int position);              
    LinkedList& getText() { return text; }
};

#endif
