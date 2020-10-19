/**
 * File: life.cpp
 * --------------
 * Implements the Game of Life.
 */

#include <fstream>
#include <iostream> // for cout
using namespace std;

#include "console.h" // required of all files that contain the main function
#include "gevents.h" // for mouse event detection
#include "gtimer.h"
#include "simpio.h" // for getLine
#include "strlib.h"

#include "life-constants.h" // for kMaxAge
#include "life-graphics.h" // for class LifeDisplay

/**
 * Function: welcome
 * -----------------
 * Introduces the user to the Game of Life and its rules.
 */
static void welcome() {
    cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
    cout << "Cells live and die by the following rules:" << endl << endl;
    cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
    cout << "\tLocations with 2 neighbors remain stable" << endl;
    cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
    cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl << endl;
    cout << "In the animation, new cells are dark and fade to gray as they age." << endl << endl;
    getLine("Hit [enter] to continue....   ");
}

/**
 * Funtion: initGridRandom
 * -----------------------
 * Initializes grid object randomly.
 */
static void initGridRandom(Grid<int> &grid, LifeDisplay &display) {
    int width = randomInteger(minRandSize, maxRandSize);
    int height = randomInteger(minRandSize, maxRandSize);
    display.setDimensions(width, height);
    grid.resize(width, height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            // grid.set(i, j, randomBool() ? randomInteger(1, kMaxAge) : 0);
            if (randomBool()) {
                int age = randomInteger(1, kMaxAge);
                grid.set(i, j, age);
                display.drawCellAt(i, j, age);
            } else {
                grid.set(i, j, 0);
            }
        }
    }
    display.repaint();
}

/**
 * Funtion: initGridFromFile
 * -------------------------
 * Initializes grid object according to opened file.
 */
static void initGridFromFile(Grid<int> &grid, LifeDisplay &display, ifstream &file) {
    string line;
    // skip lines start with "#"
    while (getline(file, line)) {
        if (!startsWith(line, "#")) {
            break;
        }
    }
    // get number of rows and columns
    int width = stringToInteger(line);
    getline(file, line);
    int height = stringToInteger(line);

    display.setDimensions(width, height);
    grid.resize(width, height);
    // set grid elements
    for (int i = 0; i < width; i++) {
        getline(file, line);
        for (int j = 0; j < height; j++) {
            // grid.set(i, j, (token == "X") ? 1 : 0);
            if (line.at(j) == 'X') {
                grid.set(i, j, 1);
                display.drawCellAt(i, j, 1);
            } else {
                grid.set(i, j, 0);
            }
        }
    }
    file.close();
    display.repaint();
}

/**
 * Funtion: initGrid
 * -----------------
 * Initializes grid object depend on user's choice.
 */
static void initGrid(Grid<int> &grid, LifeDisplay &display) {
    cout << "You can start your colony with random cells or read from a prepared file." << endl;
    string name;
    ifstream file;
    while (true) {
        name = getLine("Enter name of colony file (or RETURN to seed randomly): ");
        if (name == "") {
            // random generation
            display.setTitle("Random Colony");
            initGridRandom(grid, display);
            break;
        }
        // read from file
        file.open("files/" + name);
        if (file.fail()) {
            cout << "Unable to open the file named \"" << name << "\".  Please select another file." << endl;
            continue;
        }
        display.setTitle(name);
        initGridFromFile(grid, display, file);
        break;
    }
}

/**
 * Function: calcuNeighbors
 * ------------------------
 * Calculate the number of neighbors given cell's position.
 */
static int calcuNeighbors(const Grid<int> &grid, int row, int col) {
    int iBegin = max(0, row - 1);
    int jBegin = max(0, col - 1);
    int iEnd = min(row + 1, grid.numRows() - 1);
    int jEnd = min(col + 1, grid.numCols() - 1);
    int neighbors = 0;
    for (int i = iBegin; i <= iEnd; i++) {
        for (int j = jBegin; j <= jEnd; j++) {
            if ((grid.get(i, j) == 0) || ((i == row) && (j == col))) {
                continue;
            }
            neighbors++;
        }
    }
    return neighbors;
}

/**
 * Function: step
 * --------------
 * Simulate one step.
 */
static bool step(Grid<int> &grid) {
    bool hitMax = true;
    bool unChanged = true;
    int width = grid.numRows();
    int height = grid.numCols();
    Grid<int> nextGen(width, height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int neighbors = calcuNeighbors(grid, i, j);
            if (neighbors <= 1) {
                nextGen.set(i, j, 0);
            } else if (neighbors == 2) {
                nextGen.set(i, j, (grid.get(i, j) == 0) ? 0 : grid.get(i, j) + 1);
            } else if (neighbors == 3) {
                nextGen.set(i, j, grid.get(i, j) + 1);
            } else {
                nextGen.set(i, j, 0);
            }
            // check convergence
            if (!hitMax || !unChanged) {
                continue;
            }
            int newAge = nextGen.get(i, j);
            if ((newAge > 0) && (newAge < kMaxAge)) {
                hitMax = false;
            }
            if ((grid.get(i, j) > 0) != (newAge > 0)) {
                unChanged = false;
            }
        }
    }
    grid = nextGen;
    return hitMax && unChanged;
}

/**
 * Function: updateDisplay
 * -----------------------
 * Update life display window.
 */
static void updateDisplay(const Grid<int> &grid, LifeDisplay &display) {
    for (int i = 0; i < grid.numRows(); i++) {
        for (int j = 0; j < grid.numCols(); j++) {
            display.drawCellAt(i, j, grid.get(i, j));
        }
    }
    display.repaint(); // refresh the window
}

/**
 * Function: runAnimation
 * ----------------------
 * Full simulation process.
 */
static void runAnimation(LifeDisplay &display, Grid<int> &grid, int ms) {
    GTimer timer(ms);
    timer.start();
    while (true) {
        GEvent event = waitForEvent(TIMER_EVENT + MOUSE_EVENT);
        if (event.getEventClass() == TIMER_EVENT) {
            if (step(grid)) {
                break;
            }
            updateDisplay(grid, display);
        } else if (event.getEventType() == MOUSE_PRESSED) {
            break;
        }
    }
    timer.stop();
}

static void runSimulation(LifeDisplay &display, Grid<int> &grid) {
    cout << "You choose how fast to run the simulation." << endl;
    cout << "\t1 = As fast as this chip can go!" << endl;
    cout << "\t2 = Not too fast, this is a school zone." << endl;
    cout << "\t3 = Nice and slow so I can watch everything that happens." << endl;
    cout << "\t4 = Require enter key be pressed before advancing to next generation." << endl;
    int choice;
    while (true) {
        choice = getInteger("Your choice: ");
        if ((choice >= 1) && (choice <= 4)) {
            break;
        }
    }

    switch (choice) {
    case 1:
        runAnimation(display, grid, 1);
        break;
    case 2:
        runAnimation(display, grid, 500);
        break;
    case 3:
        runAnimation(display, grid, 5000);
        break;
    case 4:
        string advance;
        while (true) {
            advance = getLine("Please return to advance [or type out \"quit\" to end]:");
            if (advance == "quit") {
                break;
            }
            if (step(grid)) {
                break;
            }
            updateDisplay(grid, display);
        }
    }
}

/**
 * Function: main
 * --------------
 * Provides the entry point of the entire program.
 */
int main() {
    LifeDisplay display;
    display.setTitle("Game of Life");
    welcome();
    Grid<int> grid; // define grid

    // loop contents
    while (true) {
        initGrid(grid, display);
        runSimulation(display, grid);
        // Would you like to run another?
        string runAnother;
        while (true) {
            runAnother = getLine("Would you like to run another?");
            if (runAnother == "yes") {
                break;
            } else if (runAnother == "no") {
                return 0;
            }
            cout << "Please enter \"yes\" or \"no\"." << endl;
        }
    }

    return 0;
}
