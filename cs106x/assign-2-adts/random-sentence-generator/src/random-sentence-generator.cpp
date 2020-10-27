/**
 * File: random-sentence-generator.cpp
 * -----------------------------------
 * Presents a short program capable of reading in
 * context-free grammar files and generating arbitrary
 * sentences from them.
 */

#include <fstream>
#include <iostream>
using namespace std;

#include "console.h"
#include "map.h"
#include "simpio.h" // for getLine
#include "stack.h"
#include "strlib.h" // for toLowerCase, trim
#include "vector.h"

static const string kGrammarsDirectory = "grammars/";
static const string kGrammarFileExtension = ".g";
static string getNormalizedFilename(string filename) {
    string normalizedFileName = kGrammarsDirectory + filename;
    if (!endsWith(normalizedFileName, kGrammarFileExtension))
        normalizedFileName += kGrammarFileExtension;
    return normalizedFileName;
}

static bool isValidGrammarFilename(string filename) {
    string normalizedFileName = getNormalizedFilename(filename);
    ifstream infile(normalizedFileName.c_str());
    return !infile.fail();
}

static string getFileName() {
    while (true) {
        string filename = trim(getLine("Name of grammar file? [<return> to quit]: "));
        if (filename.empty() || isValidGrammarFilename(filename))
            return filename;
        cout << "Failed to open the grammar file named \"" << filename << "\". Please try again...." << endl;
    }
}

static void buildGrammar(const string &filename, Map<string, Vector<string>> &grammar) {
    ifstream infile;
    infile.open(getNormalizedFilename(filename));
    string key, line;
    while (true) {
        // detect EOF
        if (!getline(infile, line)) {
            break;
        }
        // skip empty line
        if (line == "") {
            continue;
        }
        // add key
        key = line;
        // get number of candidates

        getline(infile, line);
        int num = stringToInteger(line);

        // get all candidates
        int index = 0;
        Vector<string> candidates;
        while (index < num) {
            getline(infile, line);
            candidates.add(line);
            index++;
        }
        grammar.add(key, candidates);
    }
    infile.close();
}

static void printRandomSentence(const Map<string, Vector<string>> &grammar) {
    // use stack
    Stack<string> nonterminals;
    string sentence = "<start>";
    string nonterminal;
    bool hasLeft = false;
    uint pos = 0;

    while ((pos < sentence.length()) || (!nonterminals.isEmpty())) {
        // pop nonterminal
        if (!nonterminals.isEmpty()) {
            sentence = randomElement(grammar.get(nonterminals.pop())) + sentence.substr(pos);
            pos = 0;
            hasLeft = false;
        }

        // get nonterminal
        if (sentence[pos] == '<') {
            hasLeft = true;
            nonterminal = '<';
            pos++;
            continue;
        }

        // push nonterminal
        if (sentence[pos] == '>') {
            nonterminal += '>';
            hasLeft = false;
            nonterminals.add(nonterminal);
            pos++;
            continue;
        }

        // save nonterminal by character
        if (hasLeft) {
            nonterminal += sentence[pos++];
            continue;
        }

        // print out terminal
        cout << sentence[pos++];
    }

    cout << endl;
}

int main() {
    while (true) {
        string filename = getFileName();
        if (filename.empty())
            break;
        // cout << "Here's where you read in the \"" << filename << "\" grammar "
        //      << "and generate three random sentences." << endl;
        Map<string, Vector<string>> grammar;
        buildGrammar(filename, grammar);

        for (int i = 1; i <= 3; i++) {
            cout << i << ".) ";
            printRandomSentence(grammar);
            cout << endl;
        }
    }

    cout << "Thanks for playing!" << endl;
    return 0;
}
