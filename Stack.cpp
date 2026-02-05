
#include "Stack.h"

Stack::Stack() {
    top = NULL;
}

// push = add on top
void Stack::push(Node* state) {
    State* newState = new State(state);
    newState->next = top;
    top = newState;
}

// pop = remove from top
Node* Stack::pop() {
    if (top == NULL) return NULL;
    State* temp = top;
    Node* data = temp->textState;
    top = top->next;
    delete temp;
    return data;
}

bool Stack::isEmpty() {
    return top == NULL;
}


