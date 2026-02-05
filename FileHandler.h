#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "LinkedList.h"
#include <fstream>
#include <string>

using namespace std;

class FileHandler {
public:
    // Save all lines from linked list to file
    static bool saveToFile(const string& filename, Node* head);
    
    // Load all lines from file into linked list
    static Node* loadFromFile(const string& filename);
};

#endif