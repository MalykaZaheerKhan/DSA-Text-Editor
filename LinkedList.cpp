#include "LinkedList.h"
#include <iostream>
using namespace std;

// constructor
LinkedList::LinkedList() {
    head = NULL;
}

// count total nodes
int LinkedList::getCount() {
    int count = 0;
    Node* temp = head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
} 

// insert line at any position (1-based)
void LinkedList::insertLine(string line, int position) {
    int count = getCount();

    if (position < 1 || position > count + 1) {
        cout << "Invalid position!\n";    
        return;
    }

    Node* newNode = new Node(line);

    if (position == 1) { // insert at start
        newNode->next = head;
        if (head != NULL)
            head->prev = newNode;
        head = newNode;
    } else {
        Node* temp = head;
        int i = 1;
        while (i < position - 1) { // move to node before position
            temp = temp->next;
            i++;
        }
        newNode->next = temp->next;
        newNode->prev = temp;
        if (temp->next != NULL)
            temp->next->prev = newNode;
        temp->next = newNode;
    }

    cout << "Line inserted.\n";
}

// delete line at any position (1-based)
void LinkedList::deleteLine(int position) {
    int count = getCount();

    if (position < 1 || position > count) {
        cout << "Invalid position!\n";
        return;
    }

    Node* temp = head;

    if (position == 1) { // delete first
        head = head->next;
        if (head != NULL)
            head->prev = NULL;
        delete temp;
    } else {
        int i = 1;
        while (i < position) { // move to node to delete
            temp = temp->next;
            i++;
        }
        if (temp->prev != NULL)
            temp->prev->next = temp->next;
        if (temp->next != NULL)
            temp->next->prev = temp->prev;
        delete temp;
    }

    cout << "Line deleted.\n";
}

// display all lines
void LinkedList::display() {
    if (head == NULL) {
        cout << "Text is empty.\n";
        return;
    }
    Node* temp = head;
    cout << "\n--- Current Text ---\n";
    while (temp != NULL) {
        cout << temp->data << endl;
        temp = temp->next;
    }
}

// copy list for undo/redo
Node* LinkedList::copyList() {
    if (head == NULL) return NULL;
    Node* newHead = new Node(head->data);
    Node* currOld = head->next;
    Node* currNew = newHead;

    while (currOld != NULL) {
        currNew->next = new Node(currOld->data);
        currNew->next->prev = currNew; // set prev
        currNew = currNew->next;
        currOld = currOld->next;
    }
    return newHead;
}

// replace current list with another version
void LinkedList::replaceList(Node* newHead) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    head = newHead;
}

Node* LinkedList::getHead() {
    return head;
}
