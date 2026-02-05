#ifndef QUEUE_H
#define QUEUE_H

#include <string>
using namespace std;
struct QueueNode {
    string data;
    QueueNode* next;
    
    QueueNode(string line) {
        data = line;
        next = NULL;
    }
};
class Queue {
private:
    QueueNode* front;
    QueueNode* rear;
    int size;

public:
    Queue();
    void enqueue(string line);
    string dequeue();
    bool isEmpty();
    int getSize();
    void clear();

};

#endif
