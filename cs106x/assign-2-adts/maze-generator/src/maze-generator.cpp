/**
 * File: maze-generator.cpp
 * ------------------------
 * Presents an adaptation of Kruskal's algorithm to generate mazes.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "maze-graphics.h"
#include "set.h"
#include "shuffle.h"
#include "simpio.h"
#include "vector.h"

static int getMazeDimension(string prompt, int minDimension = 7, int maxDimension = 50) {
    while (true) {
        int response = getInteger(prompt);
        if (response == 0)
            return response;
        if (response >= minDimension && response <= maxDimension)
            return response;
        cout << "Please enter a number between " << minDimension << " and " << maxDimension << ", inclusive." << endl;
    }
}

static void generateWallsAndChambers(const int &dimension, Vector<wall> &walls, Vector<Set<cell>> &chambers) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            cell c2 = {i, j};
            // each chamber includes one unique cell
            Set<cell> chamber;
            chamber.add(c2);
            chambers.add(chamber);

            if (i - 1 >= 0) {
                cell c1 = {i - 1, j};
                wall w = {c1, c2};
                walls.add(w);
            }
            if (j - 1 >= 0) {
                cell c1 = {i, j - 1};
                wall w = {c1, c2};
                walls.add(w);
            }
        }
    }
}

static int getChamberIndex(const cell &c, const int &dimension) { return c.row * dimension + c.col; }

static bool unionChambers(Vector<Set<cell>> &chambers, const wall &w, int &dimension) {
    int index1 = getChamberIndex(w.one, dimension);
    int index2 = getChamberIndex(w.two, dimension);
    // skip cycle
    if (chambers[index1].contains(w.two) || (chambers[index2].contains(w.one))) {
        return false;
    }
    // remove wall
    Set<cell> unioned;
    unioned.addAll(chambers[index1].unionWith(chambers[index2]));
    for (cell c : unioned) {
        int index = getChamberIndex(c, dimension);
        chambers[index] = unioned;
    }
    chambers[index1] = unioned;
    chambers[index2] = unioned;
    return true;
}

int main() {
    MazeGeneratorView view;
    while (true) {
        int dimension = getMazeDimension("What should the dimension of your maze be [0 to exit]? ");
        if (dimension == 0)
            break;

        cout << "Building initial chambers..." << endl;
        view.setDimension(dimension);
        view.drawBorder();

        Vector<Set<cell>> chambers; // a chamber is represented by a set of wall/cells
        Vector<wall> walls;         // a wall is represented by two cells

        generateWallsAndChambers(dimension, walls, chambers);

        view.addAllWalls(walls);
        view.repaint();

        walls.shuffle();
        cout << "Safely merging chambers, being careful to never introduce loops..." << endl;
        for (wall w : walls) {
            if (!unionChambers(chambers, w, dimension)) {
                continue;
            }
            view.removeWall(w);
            view.repaint();
        }
    }

    return 0;
}
