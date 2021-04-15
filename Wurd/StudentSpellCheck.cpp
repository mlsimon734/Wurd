#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
    // free and delete trie pointers
    freeTree(dictRoot);
}

bool StudentSpellCheck::load(string dictionaryFile) {
    // create input file stream
    ifstream inFile(dictionaryFile);
    if (!inFile) {
        cerr << "Error: Cannot open dictionary!\n";
        return false;
    }
    
    freeTree(dictRoot);
    // allocate new root node
    dictRoot = newNode();
    
    string s;
    
    while (getline(inFile, s)) {
        // load lines into trie
        string temp = "";
        for (int i = 0; i < s.size(); i++) {
            if (isalpha(s.at(i)) || s.at(i) == '\'') {
                temp += tolower(s.at(i));
            }
        }
        
        insert(dictRoot, temp);
    }
    
    return true;
}

bool StudentSpellCheck::spellCheck(string word, int max_suggestions, vector<string>& suggestions) {
    // convert word to lowercase
    for (size_t i = 0; i < word.size(); i++) {
        word[i] = tolower(word[i]);
    }
    // word found in dictionary trie
    if (search(dictRoot, word)) {
        return true;
    }
    // clear suggestions vector
    suggestions.clear();
    
    // for every char in the word or until max_suggestions reached
    for (int i = word.size() - 1; i >= 0 && suggestions.size() < max_suggestions; i--) {
        for (int j = 0; j < 27 && suggestions.size() < max_suggestions; j++) {
            string temp = word;
            char ascii;
            
            if (j == 26) {
                ascii = '\'';
            } else {
                ascii = 'a' + j;
            }
            // insert every viable char in the given position
            temp[i] = ascii;
            // resulting string is a valid word
            if (search(dictRoot, temp)) {
                // add to suggestions vector
                suggestions.push_back(temp);
            }
        }
    }
    return false;
}

void StudentSpellCheck::spellCheckLine(const string& line, vector<SpellCheck::Position>& problems) {
    string s = line;
    
    for (size_t i = 0; i < s.size(); i++) {
        s[i] = tolower(s[i]);
    }
    
    int index = 0;
    string word = "";
    bool wordFound = false;
    problems.clear();
    
    // track position of words found
    SpellCheck::Position temp;
    temp.start = 0;
    temp.end = 0;
    
    while (index < s.size()) {
        // start of word found
        if ((isalpha(s.at(index)) || s.at(index) == '\'') && !wordFound) {
            // mark word start position, remember word found
            wordFound = true;
            temp.start = index;
        } else if (wordFound && !isalpha(s.at(index)) && s.at(index) != '\'') {
            // word end found due to non letter/apostrophe
            // store word end index
            temp.end = index - 1;
            // store word
            word = s.substr(temp.start, (temp.end - temp.start + 1));
            // if word isn't in dictionary
            if (!search(dictRoot, word)) {
                // add to problems vector
                problems.push_back(temp);
            }
            // reset position tracker, reset wordFound
            temp.start = index;
            temp.end = index;
            wordFound = false;
        } else if (wordFound && index == (s.size() - 1) && isalpha(s.at(index))) {
            // word ends at the end of the line
            temp.end = index;
            word = s.substr(temp.start, (temp.end - temp.start + 1));
            // if word isn't in dictionary
            if (!search(dictRoot, word)) {
                // add to problems vector
                problems.push_back(temp);
            }
        }
        index++;
    }
}
