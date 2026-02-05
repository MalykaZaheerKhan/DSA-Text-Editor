#ifndef STACK_H
#define STACK_H
#include "LinkedList.h"
struct State {
    Node* textState;
    State* next;
    State(Node* s) {
        textState = s;
        next = NULL;
    }
};
class Stack {
private:
    State* top;

public:
    Stack();
    void push(Node* state);
    Node* pop();
    bool isEmpty();
};

#endif       
