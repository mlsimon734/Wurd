#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <iostream>
#include <list>
#include <string>
using namespace std;

class Undo;

class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(string file);
	bool save(string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, vector<string>& lines) const;
	void undo();

private:
    // track cursor position
    struct cursor {
        unsigned long m_row, m_col;
    };
    cursor cur;
    
    // store document content
    list<string> contents;
    // track cursor within list
    list<string>::iterator curLine;
    
    // to reverse typing
    void takeback(int numCH) {
        (*curLine).erase(cur.m_col, numCH);
    }
    
    // to reverse a delete
    void reinsert(string s) {
        // reinsert string & position cursor accordingly
        (*curLine).insert(cur.m_col, s);
        cur.m_col += s.size();
    }
    
    void reenter() {
        string temp = "";
        
        // store substring to be moved to new line & erase it
        if (cur.m_col < (*curLine).size()) {
            temp = (*curLine).substr(cur.m_col, (*curLine).size() - cur.m_col);
            (*curLine).erase(cur.m_col, (*curLine).size() - cur.m_col);
        }
        // insert substring to new line
        curLine++;
        curLine = contents.insert(curLine, temp);
        // cursor back to line above
        curLine--;
    }
    
    void reJoin() {
        // retrieve string right of line break and erase
        curLine++;
        string temp = *curLine;
        curLine = contents.erase(curLine);
        
        // reconstruct full line above
        curLine--;
        (*curLine).insert(cur.m_col, temp);
    }
    
};

#endif // STUDENTTEXTEDITOR_H_
