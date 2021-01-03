/**
 * File: ssmodel.cpp
 * ------------------
 * This file will contain the implementation of the SSModel class (once you
 * have finished it!
 */

#include "ssmodel.h"
#include "exp.h"
#include "parser.h"
using namespace std;

SSModel::SSModel(int nRows, int nCols, SSView *view) {
    rows = nRows;
    cols = nCols;
    this->view = view;
}

SSModel::~SSModel() { clear(); }

bool SSModel::nameIsValid(const string &cellname) const {
    location loc;
    if (!stringToLocation(cellname, loc)) {
        return false;
    }
    if (loc.row <= rows && loc.col - 'A' <= cols) {
        return true;
    }
    return false;
}

void SSModel::setCellFromScanner(const string &cellname,
                                 TokenScanner &scanner) {
    // cellname has been garanteed to be valid
    Expression *exp = parseExp(scanner, *this);
    Vector<string> dependencies;
    Expression::parseDependencies(exp, dependencies);
    string cellnameUpper = toUpperCase(cellname);

    // detect circle
    if (!detectCircle(cellnameUpper, dependencies)) {
        // do nothing
    } else {
        delete exp;
        error("Circle detected with " + cellname);
    }

    // fill cell info
    cellsSet.add(cellnameUpper);
    info *cellinfo = new info();
    cellinfo->value = exp->eval(*this);
    cellinfo->exp = exp;
    cellinfo->in = dependencies;

    for (string name : cellinfo->in) {
        if (!lookup.containsKey(name))
            lookup.put(name, new info());
        lookup[name]->out.add(cellnameUpper);
    }
    lookup.put(cellnameUpper, cellinfo);
    view->displayCell(cellnameUpper, exp->getType() == TEXTSTRING
                                         ? exp->toString()
                                         : realToString(cellinfo->value));

    // traverse subgraph, update cell information and display
    updateSubGraph(cellnameUpper);
}

void SSModel::printCellInformation(const string &cellname) const {
    string cellnameUpper = toUpperCase(cellname);
    if (!lookup.containsKey(cellnameUpper)) {
        cout << "No info, empty" << endl;
        return;
    }

    info *cellinfo = lookup[cellnameUpper];
    cout << cellinfo->exp->toString() << endl;
    cout << "Cells that " + cellname +
                " directly depends on: " + cellinfo->in.toString()
         << endl;
    cout << "Cells that directly depend on " + cellname + ":" +
                cellinfo->out.toString()
         << endl;
}

void SSModel::writeToStream(ostream &outfile) const {
    for (string name : cellsSet) {
        outfile << name + " = " + lookup[name]->exp->toString() << endl;
    }
}

void SSModel::readFromStream(istream &infile) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.scanStrings();

    string line;
    while (getline(infile, line)) {
        scanner.setInput(line);
        string cellname = scanner.nextToken();
        scanner.nextToken(); // skip "="
        setCellFromScanner(cellname, scanner);
    }
}

double SSModel::getCellValue(const string &name) {
    if (lookup.containsKey(name))
        return lookup[name]->value;
    return 0.0;
}

bool SSModel::rangeIsValid(const string &st, const string &ed) {
    location loc1, loc2;
    stringToLocation(st, loc1);
    stringToLocation(ed, loc2);
    if (loc1.row <= loc2.row && loc1.col <= loc2.col) {
        if (loc1.row == loc2.row && loc1.col == loc2.col)
            return false;
        return true;
    }
    return false;
}

Vector<string> SSModel::getCellInRange(const string &st, const string &ed) {
    // assert rangeIsValid(st, ed);
    location loc1, loc2;
    stringToLocation(st, loc1);
    stringToLocation(ed, loc2);
    Vector<string> res;
    for (int i = loc1.row; i <= loc2.row; i++) {
        for (int j = loc1.col; j <= loc2.col; j++) {
            location loc;
            loc.row = i;
            loc.col = j;
            res.add(locationToString(loc));
        }
    }
    return res;
}

void SSModel::clear() {
    for (string key : lookup.keys()) {
        delete lookup[key]->exp;
        delete lookup[key];
    }
    lookup.clear();
    cellsSet.clear();
    view->displayEmptySpreadsheet();
}

bool SSModel::detectCircle(const string &cellname,
                           Vector<string> dependencies) {

    for (string dep : dependencies) {
        Set<string> visited;
        if (detectCircleHelper(cellname, dep, visited))
            return true;
    }
    return false;
}

bool SSModel::detectCircleHelper(const string &cellname, const string &other,
                                 Set<string> &visited) {
    if (other == cellname)
        return true;

    if (!lookup.containsKey(other))
        return false;

    bool res = false;
    for (string name : lookup[other]->in) {
        if (visited.contains(name))
            continue;
        visited.add(name);
        res |= detectCircleHelper(cellname, name, visited);
    }
    return res;
}

void SSModel::updateSubGraph(const string &cellname) {
    if (!lookup.containsKey(cellname))
        return;

    // implement topological sort (using reverse dfs order)
    Stack<string> stack;
    Set<string> visited;
    for (string other : lookup[cellname]->out) {
        if (visited.contains(other))
            continue;
        updateSubGraphHelper(other, visited, stack);
    }

    while (!stack.isEmpty()) {
        string upcell = stack.pop();
        info *cellinfo = lookup[upcell];
        Expression *exp = cellinfo->exp;
        lookup[upcell]->value = exp->eval(*this);
        view->displayCell(upcell, exp->getType() == TEXTSTRING
                                      ? exp->toString()
                                      : realToString(cellinfo->value));
    }
}

void SSModel::updateSubGraphHelper(const string &cellname, Set<string> &visited,
                                   Stack<string> &stack) {
    visited.add(cellname);
    if (lookup.containsKey(cellname)) {
        for (string other : lookup[cellname]->out) {
            if (visited.contains(other))
                continue;
            updateSubGraphHelper(other, visited, stack);
        }
    }

    stack.push(cellname);
}
