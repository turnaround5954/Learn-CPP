/**
 * File: boggle.cpp
 * ----------------
 * Implements the game of Boggle.
 */

#include "console.h"
#include "gboggle.h"
#include "grid.h"
#include "lexicon.h"
#include "map.h"
#include "random.h"
#include "set.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include <cctype>
#include <ctype.h>
#include <iostream>
using namespace std;

static const string kStandardCubes[16] = {"AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS", "AOOTTW", "CIMOTU",
                                          "DEILRX", "DELRVY", "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
                                          "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"};

static const string kBigBoggleCubes[25] = {"AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM", "AEEGMU", "AEGMNN",
                                           "AFIRSY", "BJKQXZ", "CCNSTW", "CEIILT", "CEILPT", "CEIPST", "DDLNOR",
                                           "DDHNOT", "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU", "FIPRSY",
                                           "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"};

static const int kMinLength = 4;
static const double kDelayBetweenHighlights = 100;
static const double kDelayAfterAllHighlights = 500;
static const string kEnglishLanguageDatafile = "dictionary.txt";

/**
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */
static void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl;
    cout << endl;
}

/**
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */
static void giveInstructions() {
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl;
    cout << endl;
    cout << "Hit return when you're ready...";
    getLine(); // ignore return value
}

/**
 * Function: getPreferredBoardSize
 * -------------------------------
 * Repeatedly prompts the user until he or she responds with one
 * of the two supported Boggle board dimensions.
 */

static int getPreferredBoardSize() {
    cout << "You can choose standard Boggle (4x4 grid) or Big Boggle (5x5 grid)." << endl;
    return getIntegerBetween("Which dimension would you prefer: 4 or 5?", 4, 5);
}

/**
 * Function: shuffleVec
 * --------------------
 * Randomly shuffle the elements in a Vector.
 */
static void shuffleVec(Vector<string> &vec) {
    const int last = vec.size() - 1;
    for (int i = 0; i < vec.size(); i++) {
        int r = randomInteger(i, last);
        // swap the element at position i and r
        string tmp = vec[i];
        vec[i] = vec[r];
        vec[r] = tmp;
    }
}

/**
 * Function: configureGrid
 * -----------------------
 * Get characters to be displayed on board in order
 * and initialize the grid object.
 */
static void configureGrid(Grid<char> &grid, int dimension) {
    cout << "I'll give you a chance to set up the board to your specification, "
            "which makes it easier to confirm your boggle program is working."
         << endl;

    string characters = "";
    if (getYesOrNo("Do you want to force the board configuration? ")) {
        // force configuration
        cout << "Enter a " << dimension << "-character string to identify which letters you want on the cubes." << endl;
        cout << "The first " << dimension << " characters form the top row, the next " << dimension
             << " characters form the second row, and so forth." << endl;
        while (true) {
            characters = getLine("Enter a string: ");
            if ((int)characters.size() != dimension * dimension) {
                cout << "Enter a string that's precisely " << dimension * dimension << " characters long." << endl;
                continue;
            }
            bool islegal = true;
            for (char ch : characters) {
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                    continue;
                }
                islegal = false;
            }
            if (!islegal) {
                cout << "Enter a string with only alphabetic letters." << endl;
                continue;
            }
            break;
        }
        toUpperCaseInPlace(characters);
    } else {
        // generate configuration from the template
        Vector<string> mutableVec;
        const string *p = dimension == 4 ? kStandardCubes : kBigBoggleCubes;
        for (int i = 0; i < dimension * dimension; i++) {
            mutableVec.add(p[i]);
        }
        shuffleVec(mutableVec);
        for (string cube : mutableVec) {
            characters += cube[randomInteger(0, dimension - 1)];
        }
    }

    // initialize grid
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            grid.set(i, j, characters[i * dimension + j]);
        }
    }
}

/**
 * Function: configureBoard
 * ------------------------
 * Label the cubes on the board.
 */
static void configureBoard(Grid<char> &grid) {
    for (int i = 0; i < grid.numRows(); i++) {
        for (int j = 0; j < grid.numCols(); j++) {
            labelCube(i, j, grid.get(i, j));
        }
    }
}

/**
 *Function: precomputeHelper
 * -------------------------
 * Helper function for recursion.
 */
static void precomputeHelper(Grid<char> &grid, const Lexicon &english, Lexicon &found, string &word,
                             Map<string, Vector<GridLocation>> &pathDict, Vector<GridLocation> &path,
                             const GridLocation &loc) {

    // collect legitmate words
    if (word.length() >= kMinLength && english.contains(word)) {
        if (!found.contains(word)) {
            found.add(word);
            pathDict.put(word, path);
        }
    }

    // prune dead end searches
    if (!english.containsPrefix(word)) {
        return;
    }

    // search over neighbors
    for (GridLocation inloc : loc.neighbors()) {
        // out of bound or visited before
        if (!grid.inBounds(inloc) || grid.get(inloc) == '!') {
            continue;
        }

        // make choice
        char ch = grid.get(loc);
        word.push_back(ch);
        path.push_back(loc);
        grid.set(loc, '!');

        // recursion
        precomputeHelper(grid, english, found, word, pathDict, path, inloc);

        // undo choice (backtracking)
        grid.set(loc, ch);
        path.pop_back();
        word.pop_back();
    }
}

/**
 * Function: precompute
 * --------------------
 * Precompute solution.
 */
static void precompute(Grid<char> &grid, Lexicon &english, Lexicon &found,
                       Map<string, Vector<GridLocation>> &pathDict) {
    string base = "";
    Vector<GridLocation> path;
    for (GridLocation loc : grid.locations()) {
        precomputeHelper(grid, english, found, base, pathDict, path, loc);
    }
}

/**
 * Function: humanAct
 * ------------------
 * The human's turn, return human's score.
 */
static int humanAct(const Lexicon &english, const Lexicon &computed, const Map<string, Vector<GridLocation>> &pathDict,
                    Lexicon &humanFound) {
    int score = 0;
    while (true) {
        string word = getLine("Enter a word: ");

        if (word == "") {
            return score;
        }
        if (!english.contains(word)) {
            cout << "Sorry, that isn't even a word." << endl;
            continue;
        }
        if (word.length() < kMinLength) {
            cout << "Sorry, that isn't long enough to even be considered." << endl;
            continue;
        }
        if (!computed.contains(word)) {
            cout << "That word can't be constructed with this board." << endl;
            continue;
        }
        if (humanFound.contains(word)) {
            cout << "You've already guessed that word." << endl;
            continue;
        }

        humanFound.add(word);
        score += word.length() - kMinLength + 1;

        recordWordForPlayer(word, HUMAN);
        Stack<GridLocation> locStack;
        for (GridLocation loc : pathDict.get(toUpperCase(word))) {
            pause(kDelayBetweenHighlights);
            highlightCube(loc.row, loc.col, true);
            locStack.push(loc);
        }
        pause(kDelayAfterAllHighlights);
        while (!locStack.isEmpty()) {
            GridLocation loc = locStack.pop();
            highlightCube(loc.row, loc.col, false);
            pause(kDelayBetweenHighlights);
        }
    }
}

/**
 * Function: computerAct
 * ---------------------
 * The computer's turn.
 */
static int computerAct(const Lexicon &computed, const Lexicon &humanFound) {
    int score = 0;
    for (string word : computed) {
        if (humanFound.contains(word)) {
            continue;
        }
        score += word.length() - kMinLength + 1;
        recordWordForPlayer(word, COMPUTER);
    }
    return score;
}

/**
 * Function: runGame
 * -----------------
 * Include the human's turn and the computer's turn.
 */
static void runGame(const Lexicon &english, const Lexicon &computed, const Map<string, Vector<GridLocation>> pathDict) {
    Lexicon humanFound;
    int humanScore = humanAct(english, computed, pathDict, humanFound);
    int computerScore = computerAct(computed, humanFound);
    if (humanScore < computerScore) {
        cout << "Shocker! The computer player prevailed!" << endl;
    } else if (humanScore > computerScore) {
        cout << "Whoa!!!  You actually beat the computer at its own game.  Excellent Boggle skills, human!" << endl;
    } else {
        cout << "Wow! You matched the computer!  That's pretty much a Boggle victory for the human race!" << endl;
    }
}

/**
 * Function: playBoggle
 * --------------------
 * Manages all details needed for the user to play one
 * or more games of Boggle.
 */
static void playBoggle() {
    int dimension = getPreferredBoardSize();
    drawBoard(dimension, dimension);
    // cout << "This is where you'd play the game of Boggle" << endl;

    Grid<char> grid(dimension, dimension);
    Lexicon english(kEnglishLanguageDatafile);
    Lexicon computed;
    Map<string, Vector<GridLocation>> pathDict;
    configureGrid(grid, dimension);
    configureBoard(grid);
    precompute(grid, english, computed, pathDict);

    // start compete
    runGame(english, computed, pathDict);
}

/**
 * Function: main
 * --------------
 * Serves as the entry point to the entire program.
 */
int main() {
    GWindow gw(kBoggleWindowWidth, kBoggleWindowHeight);
    initGBoggle(gw);
    welcome();
    if (getYesOrNo("Do you need instructions?"))
        giveInstructions();
    do {
        playBoggle();
    } while (getYesOrNo("Would you like to play again?"));
    cout << "Thank you for playing!" << endl;
    shutdownGBoggle();
    return 0;
}
