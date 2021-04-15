#include "StudentUndo.h"
#include <iostream>
using namespace std;

StudentUndo::~StudentUndo() {
    clear();
}

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch)
{
    switch (action) {
        case Undo::INSERT:
            // previous action was INSERT in consecutive position
            if (!actionStack.empty() &&
                actionStack.top()->a == INSERT &&
                row == actionStack.top()->row &&
                (col - actionStack.top()->col) == actionStack.top()->batch) {
                // increment batch size and add char added to batch string
                actionStack.top()->batch++;
                actionStack.top()->s += ch;
            } else {
                // otherwise create new Undoable and push to stack w/corresponding position
                Undoable* temp = new Undoable;
                temp->a = INSERT;
                temp->row = row;
                temp->col = col;
                temp->s += ch;
                actionStack.push(temp);
            }
            break;
        case Undo::DELETE:
            // previous action was backspace in consecutive position
            if (!actionStack.empty() &&
                actionStack.top()->a == DELETE &&
                row == actionStack.top()->row &&
                (actionStack.top()->col - col) == 1) {
                
                // decrement col to last deleted char
                actionStack.top()->col--;
                // increment batch size and add char deleted to batch string
                actionStack.top()->batch++;
                actionStack.top()->s.insert(0, 1, ch);
            }
            // previous action was delete in same position
            else if (!actionStack.empty() &&
                actionStack.top()->a == DELETE &&
                row == actionStack.top()->row &&
                actionStack.top()->col == col) {
                
                // increment batch size and add char deleted to batch string
                actionStack.top()->batch++;
                actionStack.top()->s += ch;
            } else {
                // otherwise create new Undoable and push to stack w/corresponding position
                Undoable* temp = new Undoable;
                temp->a = DELETE;
                temp->row = row;
                temp->col = col;
                temp->s += ch;
                actionStack.push(temp);
            }
            break;
        
        case Undo::SPLIT:
        case Undo::JOIN: {
            // create new Undoable and push to stack w/corresponding position
            Undoable* temp = new Undoable;
            temp->a = action;
            temp->row = row;
            temp->col = col;
            actionStack.push(temp);
            break;
        }
        default:
            break;
    }
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text)
{
    if (actionStack.empty()) {
        return Undo::Action::ERROR;
    }
    
    Undoable* temp = actionStack.top();
    
    switch (temp->a) {
        case Undo::INSERT:
            // set count to # chars to erase
            count = actionStack.top()->batch;
            text = "";
            // row & col set to beginning position of string to be deleted
            row = temp->row;
            col = temp->col - 1;
            // delete memory and pop stack stop
            delete actionStack.top();
            actionStack.pop();
            // return reversal action
            return Undo::Action::DELETE;
            break;
            
        case Undo::DELETE:
            count = 1;
            // text set to string to be reinserted
            text = actionStack.top()->s;
            // row & col set to location to insert string
            row = temp->row;
            col = temp->col;
            // delete memory and pop stack stop
            delete actionStack.top();
            actionStack.pop();
            // return reversal action
            return Undo::Action::INSERT;
            break;
            
        case Undo::SPLIT:
            count = 1;
            text = "";
            // row & col set to location to rejoin lines
            row = temp->row;
            col = temp->col;
            // delete memory and pop stack stop
            delete actionStack.top();
            actionStack.pop();
            // return reversal action
            return Undo::Action::JOIN;
            break;
            
        case Undo::JOIN:
            count = 1;
            text = "";
            // row & col set to location to split lines
            row = temp->row;
            col = temp->col;
            // delete memory and pop stack stop
            delete actionStack.top();
            actionStack.pop();
            // return reversal action
            return Undo::Action::SPLIT;
            break;
            
        default:
            return Undo::Action::ERROR;
            break;
    }
}

void StudentUndo::clear()
{
    while (!actionStack.empty()) {
        delete actionStack.top();
        actionStack.pop();
    }
}
