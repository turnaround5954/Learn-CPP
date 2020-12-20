// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own, along with
// comments on every function and on complex code sections.
// TODO: remove this comment header

#include "encoding.h"
using namespace std;
// TODO: include any other headers you need

Map<int, int> buildFrequencyTable(istream &input) {
    // TODO: implement this function
    Map<int, int> freqTable; // this is just a placeholder so it will compile
    char ch;
    while (input.get(ch)) {
        if (!freqTable.containsKey(ch)) {
            freqTable.put(ch, 1);
        } else {
            freqTable[ch] += 1;
        }
    }
    freqTable.put(PSEUDO_EOF, 1);
    return freqTable; // this is just a placeholder so it will compile
}

HuffmanNode *buildEncodingTree(const Map<int, int> &freqTable) {
    // TODO: implement this function
    return NULL; // this is just a placeholder so it will compile
}

Map<int, string> buildEncodingMap(HuffmanNode *encodingTree) {
    // TODO: implement this function
    Map<int, string>
        encodingMap;    // this is just a placeholder so it will compile
    return encodingMap; // this is just a placeholder so it will compile
}

void encodeData(istream &input, const Map<int, string> &encodingMap,
                obitstream &output) {
    // TODO: implement this function
}

void decodeData(ibitstream &input, HuffmanNode *encodingTree, ostream &output) {
    // TODO: implement this function
}

void compress(istream &input, obitstream &output) {
    // TODO: implement this function
}

void decompress(ibitstream &input, ostream &output) {
    // TODO: implement this function
}

void freeTree(HuffmanNode *node) {
    // TODO: implement this function
}
