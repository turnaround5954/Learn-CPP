/**
 * File: word-ladder.cpp
 * ---------------------
 * Implements a program to find word ladders connecting pairs of words.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "lexicon.h"
#include "queue.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"

static string getWord(const Lexicon &english, const string &prompt) {
    while (true) {
        string response = trim(toLowerCase(getLine(prompt)));
        if (response.empty() || english.contains(response))
            return response;
        cout << "Your response needs to be an English word, so please try again." << endl;
    }
}

static void printLadder(const Vector<string> &ladder) {
    for (int i = 0; i < ladder.size(); i++) {
        if (i > 0) {
            cout << " ";
        }
        cout << ladder[i];
    }
    cout << endl;
}

static void generateNeighbor(const Lexicon &english, const Lexicon &tracked, const string &current,
                             Vector<string> &neighbors) {
    for (uint i = 0; i < current.size(); i++) {
        for (char character = 'a'; character <= 'z'; character++) {
            if (character == current[i]) {
                continue;
            }
            string neighbor = current;
            neighbor[i] = character;
            if (english.contains(neighbor) && !tracked.contains(neighbor)) {
                neighbors.add(neighbor);
            }
        }
    }
}

static void generateLadder(const Lexicon &english, const string &start, const string &end) {
    // special case:
    if (start.length() != end.length()) {
        cout << "The two endpoints must contain the same number of characters, or else no word ladder can exist."
             << endl
             << endl;
        return;
    }

    // enqueue start word
    Queue<Vector<string>> queue;
    queue.enqueue(Vector<string>{start});

    // track used ladder
    Lexicon tracked;

    while (!queue.isEmpty()) {
        Vector<string> ladder = queue.dequeue();
        if (ladder.back() == end) {
            // ladder found
            cout << "Found ladder: ";
            printLadder(ladder);
            return;
        }

        Vector<string> neighbors;
        generateNeighbor(english, tracked, ladder.back(), neighbors);
        for (string neighbor : neighbors) {
            Vector<string> partial = ladder;
            queue.enqueue(partial + neighbor);
            tracked.add(neighbor);
        }
    }

    // exhausted
    cout << "Ladder not found" << endl << endl;
}

static const string kEnglishLanguageDatafile = "dictionary.txt";
static void playWordLadder() {
    Lexicon english(kEnglishLanguageDatafile);
    while (true) {
        string start = getWord(english, "Please enter the source word [return to quit]: ");
        if (start.empty())
            break;
        string end = getWord(english, "Please enter the destination word [return to quit]: ");
        if (end.empty())
            break;
        generateLadder(english, start, end);
    }
}

int main() {
    cout << "Welcome to the CS106 word ladder application!" << endl << endl;
    playWordLadder();
    cout << "Thanks for playing!" << endl;
    return 0;
}
