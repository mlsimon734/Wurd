#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
#include <string>
using namespace std;

class StudentUndo : public Undo {
public:
    ~StudentUndo();
	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, string& text);
	void clear();

private:
    // each is a user change
    struct Undoable {
        Action a;
        int row, col;
        string s;
        int batch {1};
    };
    // stack holds user changes
    stack<Undoable*> actionStack;
    
};

#endif // STUDENTUNDO_H_
