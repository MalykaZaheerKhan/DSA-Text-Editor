#include "FileHandler.h"
#include <iostream>

using namespace std;

// SAVE: Write all lines to file
bool FileHandler::saveToFile(const string& filename, Node* head) {
    if (filename.empty()) {
        cout << "Invalid filename!\n";
        return false;
    }

    ofstream file(filename);
    
    if (!file.is_open()) {
        cout << "Error: Could not open file for writing: " << filename << "\n";
        return false;
    }

    if (head == NULL) {
        cout << "Text is empty. Empty file saved.\n";
        file.close();
        return true;
    }

    Node* temp = head;
    while (temp != NULL) {
        file << temp->data << "\n";
        temp = temp->next;
    }

    file.close();
    cout << "File saved successfully: " << filename << "\n";
    return true;
}

// LOAD: Read all lines from file into new linked list
Node* FileHandler::loadFromFile(const string& filename) {
    if (filename.empty()) {
        cout << "Invalid filename!\n";
        return NULL;
    }

    ifstream file(filename);
    
    if (!file.is_open()) {
        cout << "Error: File not found: " << filename << "\n";
        return NULL;
    }

    Node* head = NULL;
    Node* tail = NULL;
    string line;

    while (getline(file, line)) {
        Node* newNode = new Node(line);
        
        if (head == NULL) { // first node
            head = newNode;
            tail = newNode;
        } else { // add to end
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    file.close();
    cout << "File loaded successfully: " << filename << "\n";
    return head;
}