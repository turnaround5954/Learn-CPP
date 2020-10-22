/**
 * File: maze-graphics.h
 * ---------------------
 * Exports the view class that displays the generation
 * of a maze.
 */

#ifndef _maze_graphics_
#define _maze_graphics_

#include <string>
#include "gwindow.h"
#include "maze-types.h"
#include "gthread.h"

class MazeGeneratorView : private GWindow {
public:

/**
 * Method: constructor
 * -------------------
 * Presents a clear graphics window so that calls to
 * drawBorder and drawWall can be made to help animate
 * the construction of a simple maze.
 */

    MazeGeneratorView();

/**
 * Method: destructor
 * ------------------
 * Closes and otherwise destroys the MazeGeneratorView.
 */
    ~MazeGeneratorView();

/**
 * Method: setDimension
 * --------------------
 * Sets the dimension of the maze, which is understood to be square, to
 * be that provided.
 */
    void setDimension(int dimension);

/**
 * Method: drawBorder
 * ------------------
 * Draws a border around the confines of the maze.
 */
    void drawBorder();

/**
 * Method: addOneWall
 * ----------------
 * Adds (but does not display) the wall presented via the only argument.
 * To display all newly-added walls on the screen, call view.repaint();
 */
    void addOneWall(const wall& w);

/**
 * Method: addAllWalls
 * ----------------
 * Adds (but does not display) all the walls contained within the parameter.
 * To display all newly-added walls on the screen, call view.repaint();
 * Optimized for speed as compared to several sequential calls to addOneWall.
 *
 * This function requires that the parameter passed in is, in fact, a collection
 * that can be iterated through and that contains wall objects.
 */
    template <typename C>
    void addAllWalls(const C& collection) { addAllWalls(collection.begin(), collection.end()); }
    // defined inline as required by template type

/**
 * Method: removeWall
 * ------------------
 * Removes a previously drawn wall, which is the wall provided.
 * To update the display to reflect new removals, call view.repaint();
 */
    void removeWall(const wall& w);

/**
 * Method: repaint
 * ---------------
 * Forces the accumulation of all add and remove operations to actually show
 * up in the graphics window.
 */
    void repaint() { GWindow::repaint(); } // inlined for convenience

private:
    int dimension;
    double wallLength;
    double ulx;
    double uly;

    Map<wall, GLine*> wallLines;

    void addOneWall(const wall& w, const std::string& color, double inset);
    void drawColoredLine(double startx, double starty, double endx, double endy, const std::string& color);

    // optimize addAllWalls for speed by using the QtGui thread
    template <typename ForwardIterator>
    void addAllWalls(ForwardIterator start, ForwardIterator end) {
        GThread::runOnQtGuiThread([&] {
            for(; start != end; ++start) {
                addOneWall(*start);
            }
        });
    }
};

#endif
