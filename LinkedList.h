
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
using namespace std;

// Node = single line of text
struct Node {
    string data;
    Node* next;
    Node* prev;  // added for doubly linked

    Node(string line) {
        data = line;
        next = NULL;
        prev = NULL;
    }
};

// Doubly Linked List = chain of nodes
class LinkedList {
private:
    Node* head;

public:
    LinkedList();
    void insertLine(string line, int position);
    void deleteLine(int position);
    void display();                            
    Node* copyList();                          
    void replaceList(Node* newHead);
    Node* getHead();                          
    int getCount();
};

#endif
