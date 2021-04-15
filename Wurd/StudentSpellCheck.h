#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
using namespace std;

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() {dictRoot = newNode();}
	virtual ~StudentSpellCheck();
	bool load(string dict_file);
	bool spellCheck(string word, int maxSuggestions, vector<string>& suggestions);
	void spellCheckLine(const string& line, vector<Position>& problems);

private:
    // node in trie
    struct TrieLeaf {
        TrieLeaf* branches[27];
        bool isWordEnd;
    };
    // create blank trie node
    TrieLeaf* newNode() {
        TrieLeaf* leaf = new TrieLeaf;
        
        leaf->isWordEnd = false;
        
        for (int i = 0; i < 27; i++) {
            leaf->branches[i] = nullptr;
        }
        
        return leaf;
    }
    // insert a word into trie
    void insert(TrieLeaf* root, string word) {
        // chars in word as keys
        for (int i = 0; i < word.length(); i++) {
            // a-z corresponds to 0-25 in array
            // ' corresponds 26
            int letter;
            if (word.at(i) != '\'') {
                letter = word.at(i) - 'a';
            } else {
                letter = 26;
            }
            // if char branch doesn't already exist
            if (root->branches[letter] == nullptr) {
                root->branches[letter] = newNode();
            }
            // move to branch
            root = root->branches[letter];
        }
        // mark word end
        root->isWordEnd = true;
    }
    
    bool search(TrieLeaf* root, string word) {
        // for ever char in word
        for (int i = 0; i < word.size(); i++) {
            // convert char to int key
            int letter;
            if (word.at(i) == '\'') {
                letter = 26;
            } else {
                letter = word.at(i) - 'a';
            }
            // if key doesn't match a branch
            if (root->branches[letter] == nullptr)
                return false;
            // move down to next trie level
            root = root->branches[letter];
        }
        // true if word end reached
        return (root != nullptr && root->isWordEnd);
    }
    // recursively free memory
    void freeTree(TrieLeaf* node) {
        if (node == nullptr) {
            return;
        }
        // for every branch
        for (int i = 0; i < 27; i++) {
            // free memory
            freeTree(node->branches[i]);
        }
        // free current node
        delete node;
    }
    // root node
    TrieLeaf* dictRoot;
};

#endif  // STUDENTSPELLCHECK_H_
