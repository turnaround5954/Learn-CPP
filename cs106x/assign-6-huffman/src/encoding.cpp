// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own, along with
// comments on every function and on complex code sections.
// TODO: remove this comment header

#include "encoding.h"
#include "pqueue.h"
using namespace std;
// TODO: include any other headers you need

Map<int, int> buildFrequencyTable(istream &input) {
    // TODO: implement this function
    Map<int, int> freqTable;
    char ch;
    while (input.get(ch)) {
        if (!freqTable.containsKey(ch)) {
            freqTable.put(ch, 1);
        } else {
            freqTable[ch] += 1;
        }
    }
    freqTable.put(PSEUDO_EOF, 1);
    return freqTable;
}

HuffmanNode *buildEncodingTree(const Map<int, int> &freqTable) {
    // TODO: implement this function
    PriorityQueue<HuffmanNode *> *pq = new PriorityQueue<HuffmanNode *>();
    for (int key : freqTable.keys()) {
        int count = freqTable[key];
        pq->add(new HuffmanNode(key, count, nullptr, nullptr), count);
    }
    while (pq->size() >= 2) {
        HuffmanNode *zero = pq->dequeue();
        HuffmanNode *one = pq->dequeue();
        int priority = zero->count + one->count;
        HuffmanNode *parent = new HuffmanNode(NOT_A_CHAR, priority, zero, one);
        pq->enqueue(parent, priority);
    }
    return pq->dequeue();
}

void traversal(HuffmanNode *node, string &code, Map<int, string> &encMap) {
    // base case
    if (node == nullptr) {
        return;
    }

    if (node->character != NOT_A_CHAR) {
        string encoding = code;
        encMap.put(node->character, encoding);
    }

    // choose zero
    code.push_back('0');
    traversal(node->zero, code, encMap);
    code.pop_back();

    // choose one
    code.push_back('1');
    traversal(node->one, code, encMap);
    code.pop_back();
}

Map<int, string> buildEncodingMap(HuffmanNode *encodingTree) {
    // TODO: implement this function
    Map<int, string> encodingMap;
    string code = "";
    traversal(encodingTree, code, encodingMap);
    return encodingMap;
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
