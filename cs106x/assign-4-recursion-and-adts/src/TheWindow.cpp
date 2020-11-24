#include "TheWindow.h"
#include <memory>
using namespace std;

namespace {
    /* Clears the display by resetting it to the background color
     * and clearing all graphical elements.
     */
    void clearDisplay(GWindow* theWindow) {
        theWindow->clearCanvas();
        theWindow->setColor(kBackgroundColor);
        theWindow->fillRect(0, 0,
                           theWindow->getWidth(),
                           theWindow->getHeight());
        theWindow->repaint();
    }

    /* Creates a window that will serve as The Window. */
    GWindow* makeTheWindow() {
        GWindow* window = new GWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT);

        /* Turn on double-buffering to reduce flicker. */
        window->setRepaintImmediately(false);

        /* Make it pretty. */
        clearDisplay(window);
        window->repaint();
        return window;
    }
}

GWindow* theWindow() {
    static GWindow* window = makeTheWindow();
    return window;
}

void clearDisplay() {
    clearDisplay(theWindow());
}
