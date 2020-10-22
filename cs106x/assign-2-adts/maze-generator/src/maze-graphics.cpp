/**
 * File: maze-graphics.cpp
 * -----------------------
 * Provides the implementation of the (very primitive)
 * graphics routines needed to help animate the construction
 * of a maze.
 */

#include "maze-graphics.h"

#include <string>
using namespace std;

/**
 * Global constants that help provide more
 * meaningful names for what will otherwise be
 * magic values.
 */
static const string kWindowTitle("Maze Generator");
static const double kWindowWidth = 14 * 72; // 14 inches at 72 pixel per inch
static const double kWindowHeight = 8 * 72; // 8 inches at 72 pixels per inch
static const string kMazeVisibleColor("Blue");
static const string kMazeInvisibleColor("White");
static const double kMazeSize = 7 * 72; // 7 inches at 72 pixels per inch
static const double kLineWidth = 2.0;
static const double kEndSegmentFraction = 0.25;
static const double kEndOverlapFraction = 0.05;

MazeGeneratorView::MazeGeneratorView() : GWindow(kWindowWidth, kWindowHeight) {
    ulx = (getWidth() - kMazeSize)/2;
    uly = (getHeight() - kMazeSize)/2;
    setWindowTitle(kWindowTitle);
    setVisible(true);
    setRepaintImmediately(false);
}

MazeGeneratorView::~MazeGeneratorView() {
    close();
}

void MazeGeneratorView::setDimension(int dimension) {
    if (dimension <= 0)
        error("Value passed to MazeGeneratorView::setDimension should be positive.");
    
    setColor("White");
    fillRect(0, 0, getWidth(), getHeight());
    this->dimension = dimension;
    wallLength = kMazeSize / dimension;
    wallLines.clear();
    clear();
    drawBorder();
}

void MazeGeneratorView::addOneWall(const wall& w, const string& color, double inset) {
    if (wallLines.containsKey(w)) {
        // then we just need to recolor the wall's center segment
        wallLines[w]->setColor(color);
    } else {
        
        double startx = ulx + w.one.col * this->wallLength;
        double starty = uly + w.one.row * this->wallLength;
        
        double wallLength = this->wallLength;
        if (color == kMazeInvisibleColor) {
            wallLength -= 2 * inset;
        }
        
        GLine* wallLine;
        GLine* firstBit;
        GLine* lastBit;
        
        if (w.one.row == w.two.row) { // horizontal wall
            startx += this->wallLength;
            starty += inset;
            // have a little overlap between center segment and end segments
            firstBit = new GLine(startx, starty, startx,
                                 starty + wallLength * kEndSegmentFraction);
            wallLine = new GLine(startx, starty + wallLength * (kEndSegmentFraction - kEndOverlapFraction),
                                 startx, starty + wallLength * (1 - (kEndSegmentFraction - kEndOverlapFraction)));
            lastBit = new GLine(startx, starty + wallLength * (1 - kEndSegmentFraction),
                                startx, starty + wallLength);
        } else { // vertical wall
            starty += this->wallLength;
            startx += inset;
            // have a little overlap between center segment and end segments
            firstBit = new GLine(startx, starty,
                                 startx + wallLength * kEndSegmentFraction, starty);
            wallLine = new GLine(startx + wallLength * (kEndSegmentFraction - kEndOverlapFraction),
                                 starty, startx + wallLength * (1 - (kEndSegmentFraction - kEndOverlapFraction)), starty);
            lastBit = new GLine(startx + wallLength * kEndSegmentFraction,
                                starty, startx + wallLength, starty);
        }
        
        // insert end segments
        firstBit->setColor(color);
        firstBit->setLineWidth(kLineWidth);
        add(firstBit);
        lastBit->setColor(color);
        lastBit->setLineWidth(kLineWidth);
        add(lastBit);
        
        // insert mutable wall
        wallLine->setColor(color);
        wallLine->setLineWidth(kLineWidth);
        wallLines.put(w, wallLine);
        add(wallLine);
    }
}

void MazeGeneratorView::drawColoredLine(double startx, double starty,
                                        double endx, double endy,
                                        const string& color) {
    setColor(color);
    drawLine(startx, starty, endx, endy);
}

void MazeGeneratorView::drawBorder() {
    drawColoredLine(ulx, uly, ulx, uly + kMazeSize, kMazeVisibleColor);
    drawColoredLine(ulx, uly + kMazeSize,
                    ulx + kMazeSize - wallLength, uly + kMazeSize, kMazeVisibleColor);
    drawColoredLine(ulx + kMazeSize - wallLength, uly + kMazeSize,
                    ulx + kMazeSize, uly + kMazeSize, kMazeInvisibleColor);
    drawColoredLine(ulx + kMazeSize, uly + kMazeSize,
                    ulx + kMazeSize, uly, kMazeVisibleColor);
    drawColoredLine(ulx + kMazeSize, uly,
                    ulx + wallLength, uly, kMazeVisibleColor);
    drawColoredLine(ulx + wallLength, uly,
                    ulx, uly, kMazeInvisibleColor);
}

void MazeGeneratorView::addOneWall(const wall& w) {
    // not wrapped within Qt Gui thread here because it may be
    // called from addAllWalls, which already does that
    addOneWall(w, kMazeVisibleColor, 0);
}

void MazeGeneratorView::removeWall(const wall& w) {
    GThread::runOnQtGuiThread([&] {
        addOneWall(w, kMazeInvisibleColor, wallLength / 5);
    });
}
