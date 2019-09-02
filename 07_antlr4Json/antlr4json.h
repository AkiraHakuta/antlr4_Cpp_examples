#pragma once

#include <string>

using namespace std;

class JSON_NODE{
    public:
        string name;
        unsigned int my_addr;
        unsigned int child_addr;
        unsigned int sibling_addr;
        string key;       
        string atom;
        JSON_NODE(){child_addr = 0; sibling_addr = 0; key = "", atom = "";};        
};

void dump_antlr4json(map<unsigned int, JSON_NODE>& ajson);

JSON_NODE* jasonNodeWalk(map<unsigned int, JSON_NODE>& ajson, JSON_NODE& currJn, string route);

string beautify_lisp_string(string in_string);
