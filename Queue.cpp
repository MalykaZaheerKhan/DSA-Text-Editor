
#include "Queue.h"
#include <iostream>
using namespace std;

Queue::Queue() {
    front = NULL;
    rear = NULL;
    size = 0;
}


// Add line to rear of queue
void Queue::enqueue(string line) {
   
    if (line.empty()) {
        cout << "Cannot enqueue empty line!\n";
        return;
    }
    
    QueueNode* newNode = new QueueNode(line);
    
    // Check if memory allocation successful
    if (newNode == NULL) {
        cout << "Memory allocation failed!\n";
        return;
    }
    
    if (rear == NULL) {  // Queue is empty
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    size++;
}

// Remove and return front line
string Queue::dequeue() {
    // Check if queue is empty
    if (isEmpty()) {
        cout << "Queue is empty! Cannot dequeue.\n";
        return "";
    }
    
    QueueNode* temp = front;
    string data = temp->data;
    front = front->next;
    
    // If queue becomes empty after dequeue
    if (front == NULL) {
        rear = NULL;
    }
    
    delete temp;
    size--;
    return data;
}

void Queue::clear() {
    while (!isEmpty()) {
        dequeue();  // remove elements one by one
    }
}

bool Queue::isEmpty() {
    return (front == NULL && rear == NULL && size == 0);
}
int Queue::getSize() {
    return size;
}

