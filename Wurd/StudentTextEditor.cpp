#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
     // set cursor
     cur.m_row = 0;
     cur.m_col = 0;
     // initialize lines list
     contents.push_back("");
     curLine = contents.begin();
}

StudentTextEditor::~StudentTextEditor()
{
    contents.clear();
}

bool StudentTextEditor::load(string file)
{
    // clear screen
    reset();
    
    // create input file stream
    ifstream inFile(file);
    if (!inFile) {
        cerr << "Error: Cannot open " << file << ".txt!\n";
        return false;
    }

    string s;
    int count = 0;

    while (getline(inFile, s)) {
        // remove carriage return chars
        if (s.size() >= 2 && s.at(s.size() - 1) == '\r') {
            s.erase(s.size() - 1);
        }
        // load lines into list
        contents.push_back(s);
        count++;
    }
    // set iterator to the first line
    curLine = contents.begin();
    
    return true;
}

bool StudentTextEditor::save(string file)
{
    // create output file stream
    ofstream outFile(file);
    if (!outFile) {
        cerr << "Error: Cannot create " << file << ".txt!\n";
        return false;
    }
    
    list<string>::iterator it = contents.begin();
    
    // add file contents to output stream
    while (it != contents.end()) {
        outFile << *it << endl;
        it++;
    }
    
    return true;
}

void StudentTextEditor::reset()
{
    contents.clear();
    
    // create new initial line & reset iterator
    contents.push_back("");
    curLine = contents.begin();
    
    // reset cursor pos
    cur.m_row = 0;
    cur.m_col = 0;
}

void StudentTextEditor::move(Dir dir)
{
    switch (dir) {
        case Dir::UP:
            // not at top of screen
            if (cur.m_row > 0) {
                curLine--;
                size_t newLen = (*curLine).size();
                // decrement cursor row if new line is shorter
                if (cur.m_col > newLen)
                    cur.m_col = (*curLine).size();
                
                cur.m_row--;
            }
            break;
        case Dir::DOWN:
            // not at bottom of screen
            if (cur.m_row < (contents.size() - 1)) {
                curLine++;
                size_t newLen = (*curLine).size();
                
                // decrement cursor row if new line is shorter
                if (cur.m_col > newLen)
                    cur.m_col = (*curLine).size();
                
                cur.m_row++;
            }
            break;
        case Dir::LEFT:
            // not at leftmost side
            if (cur.m_col > 0) {
                cur.m_col--;
            } else {
                // at leftmost position of line & not top row
                if (cur.m_row > 0) {
                    // set cursor & iterator to end of above line
                    cur.m_row--;
                    curLine--;
                    cur.m_col = (*curLine).size();
                }
            }
            break;
        case Dir::RIGHT:
            // not at rightmost side
            if (cur.m_col < (*curLine).size()) {
                cur.m_col++;
            } else {
                // at rightmost position of line & not bottom row
                if (cur.m_row < contents.size() - 1) {
                    // set cursor & iterator to beginnig of below line
                    cur.m_row++;
                    curLine++;
                    cur.m_col = 0;
                }
            }
            break;
        case Dir::HOME:
            // move cursor to beginning of line
            cur.m_col = 0;
            break;
        case Dir::END:
            // move cursor to end of line.
            cur.m_col = (*curLine).size();
            break;
        default:
            break;
    }
}

void StudentTextEditor::del()
{
    // cursor at the end of the line & not on bottom row
    if (cur.m_col == (*curLine).size() && cur.m_row < (contents.size() - 1)) {
        // join line below with current line
        curLine++;
        string temp = *curLine;
        contents.erase(curLine);
        
        curLine--;
        (*curLine).insert(cur.m_col, temp);
        // track action
        getUndo()->submit(Undo::JOIN, cur.m_row, cur.m_col, '#');
    } else if (cur.m_col != (*curLine).size()) {
        // track action
        getUndo()->submit(Undo::DELETE, cur.m_row, cur.m_col, (*curLine).at(cur.m_col));
        // delete char at cursor
        (*curLine).erase(cur.m_col, 1);
    }
}

void StudentTextEditor::backspace()
{
    // not at leftmost position of line
    if (cur.m_col > 0) {
        // track action
        getUndo()->submit(Undo::DELETE, cur.m_row, cur.m_col - 1, (*curLine).at(cur.m_col - 1));
        // erase char behind cursor & move cursor left
        (*curLine).erase(cur.m_col - 1, 1);
        cur.m_col--;
    } else if (cur.m_row > 0) {
        // not at top line
        // join current line with line above
        string temp = *curLine;
        curLine = contents.erase(curLine);
        
        curLine--;
        cur.m_row--;
        cur.m_col = (*curLine).size();
        (*curLine).insert(cur.m_col, temp);
        // track action
        getUndo()->submit(Undo::JOIN, cur.m_row, cur.m_col, '#');
    }
}

void StudentTextEditor::insert(char ch)
{
    // if ch is a tab
    if (ch == 9) {
        for (int i = 0; i < 4; i++) {
            insert(' ');
        }
        return;
    }
    // insert char into line
    (*curLine).insert(cur.m_col, 1, ch);
    cur.m_col++;
    // track action
    getUndo()->submit(Undo::INSERT, cur.m_row, cur.m_col, ch);
}

void StudentTextEditor::enter()
{
    string temp = "";
    // not at the end of the line
    if (cur.m_col < (*curLine).size()) {
        // store line past cursor & erase from current line
        temp = (*curLine).substr(cur.m_col, (*curLine).size() - cur.m_col);
        (*curLine).erase(cur.m_col, (*curLine).size() - cur.m_col);
    }
    // track action
    getUndo()->submit(Undo::SPLIT, cur.m_row, cur.m_col, '#');
    // move stored string onto a new line
    curLine++;
    curLine = contents.insert(curLine, temp);
    // set cursor to beginning of that new line
    cur.m_col = 0;
    cur.m_row++;
}

void StudentTextEditor::getPos(int& row, int& col) const
{
    row = cur.m_row;
    col = cur.m_col;
}

int StudentTextEditor::getLines(int startRow, int numRows, vector<std::string>& lines) const
{
    if (startRow < 0 || startRow >= contents.size()) {
        return -1;
    }
    // clear lines vector
    lines.clear();
    
    list<string>::const_iterator it = contents.begin();
    // itertate to startRow
    for (int j = 0; j < startRow; j++) {
        it++;
    }

    int i = 0;
    // add lines up to numRows or until the end of content list
    for (i = startRow; i < startRow + numRows && i < contents.size() && it != contents.end(); i++, it++) {
        lines.push_back(*it);
    }
    
    return i - startRow;
}

void StudentTextEditor::undo()
{
    int r, c, numCH;
    string s;
    // get last action from Undo stack
    Undo::Action act = getUndo()->get(r, c, numCH, s);
    
    // no actions in stack
    if (act == Undo::ERROR) return;
    // set cursor to position given by Undo stack
    cur.m_col = c;
    if (cur.m_row < r) {
        while (cur.m_row != r) {
            curLine++;
            cur.m_row++;
        }
    } else if (cur.m_row > r) {
        while (cur.m_row != r) {
            curLine--;
            cur.m_row--;
        }
    }
    // complete the reversal action returned by getUndo
    switch (act) {
        case Undo::INSERT:
            reinsert(s);
            break;
            
        case Undo::DELETE:
            takeback(numCH);
            break;
            
        case Undo::SPLIT:
            reenter();
            break;
            
        case Undo::JOIN:
            reJoin();
            break;
        default:
            break;
    }
    
}
