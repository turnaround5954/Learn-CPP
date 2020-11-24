#include "Disasters.h"
#include "TheWindow.h"
#include "gobjects.h"
#include <memory>
#include <climits>
#include <algorithm>
#include "hashmap.h"
#include "vector.h"
#include "gevents.h"
#include "gthread.h"
using namespace std;

/* Everything in here is private to this file. */
namespace {
/* Colors to use to draw the network. */
const string kDarkCityColor    = "#404040";
const string kDarkRoadColor    = "#808080";
const string kDarkFontColor    = "#a0a0a0";
const string kLightFontColor   = "#000000";

const string kStockpileCityColor = "#FFCF60";
const string kCoveredCityColor = "#FFFFFF";

const string kConsideringCityColor = "#6090FF";
const string kPreviouslyConsideringCityColor = "#263966";

/* Radius of a city */
const double kCityOuterRadius = 30;
const double kCityInnerRadius = 20;

/* Buffer space around the window. */
const double kBufferSpace = 60;

/* Lower bound on the width or height of the data range, used for
     * collinear points.
     */
const double kMinDataSeparation = 1e-5;

/* Max length of a string in a label. */
const string::size_type kMaxLength = 3;

/* Lag to allow stockpile/covering repaint calls to actually show up */
const int kRepaintPauseTime = 5;

/* Geometry information for drawing the network. */
struct Geometry {
    /* Range of X and Y values in the data set, used for
         * scaling everything.
         */
    double minDataX, minDataY, maxDataX, maxDataY;

    /* Range of X and Y values to use when drawing everything. */
    double minDrawX, minDrawY, maxDrawX, maxDrawY;
};

/* Information about a road being displayed onscreen */
struct GRoad {
    /* The line that displays this road on the map */
    GLine *line;

    /* The names of the two cities connected by this road */
    string city1;
    string city2;
};

/* Information about a city being displayed onscreen */
struct GCity {
    /* The circle that displays the border color on the map */
    GOval *outerCircle;

    /* The circle that displays the inner city color on the map */
    GOval *innerCircle;

    /* The label displaying this city's name */
    GText *label;

    /* All the indices of roads connecting to this city (in allRoads) */
    Vector<int> roads;

    /* Easy way to check whether the city is stockpiling resources */
    bool isStockpiled;

    /* Easy way to check whether the city is currently trying to be covered */
    bool isTryingToBeCovered;
};

/* The current state of the visualizer (what it is displaying) */
static DisasterTest currentNetwork;
static Geometry currentNetworkGeometry;

/* Stack of cities currently being marked as trying to be covered */
static Stack<std::string> coveredStack;

/* A map from city name to the graphical object displaying it */
static HashMap<string, GCity> gCityMap;

/* A set of all roads currently displayed */
Vector<GRoad> allRoads;

/* Given a data set, fills in the min and max X and Y values
     * encountered in that set.
     */
void computeDataBounds(const DisasterTest& network, Geometry& geo) {
    geo.minDataX = geo.minDataY = numeric_limits<double>::infinity();
    geo.maxDataX = geo.maxDataY = -numeric_limits<double>::infinity();

    for (const string& cityName: network.cityLocations) {
        geo.minDataX = min(geo.minDataX, network.cityLocations[cityName].getX());
        geo.minDataY = min(geo.minDataY, network.cityLocations[cityName].getY());

        geo.maxDataX = max(geo.maxDataX, network.cityLocations[cityName].getX());
        geo.maxDataY = max(geo.maxDataY, network.cityLocations[cityName].getY());
    }

    /* And now edge cases! Ensure that the difference between these amounts is
         * nonzero and not terribly small.
         */
    geo.maxDataX = max(geo.maxDataX, geo.minDataX + kMinDataSeparation);
    geo.maxDataY = max(geo.maxDataY, geo.minDataY + kMinDataSeparation);
}

/* Once we have the data bounds, we can compute the graphics bounds,
     * which will try to take maximum advantage of the width and height
     * that we have available to us.
     */
void computeGraphicsBounds(Geometry& geo) {
    /* Get the aspect ratio of the window. */
    double winWidth  = theWindow()->getCanvasWidth()  - 2 * kBufferSpace;
    double winHeight = theWindow()->getCanvasHeight() - 2 * kBufferSpace;
    double winAspect = winWidth / winHeight;

    /* Get the aspect ratio of the data set. */
    double dataAspect = (geo.maxDataX - geo.minDataX) / (geo.maxDataY - geo.minDataY);

    double dataWidth, dataHeight;

    /* If the data aspect ratio exceeds the window aspect ratio,
         * the limiting factor in the display is going to be the
         * width. Therefore, we'll use that to determine our effective
         * width and height.
         */
    if (dataAspect >= winAspect) {
        dataWidth = winWidth;
        dataHeight = dataWidth / dataAspect;
    } else {
        dataHeight = winHeight;
        dataWidth = dataAspect * dataHeight;
    }

    /* Now, go center that in the window. */
    geo.minDrawX = (winWidth  -  dataWidth) / 2.0 + kBufferSpace;
    geo.minDrawY = (winHeight - dataHeight) / 2.0 + kBufferSpace;

    geo.maxDrawX = geo.minDrawX + dataWidth;
    geo.maxDrawY = geo.minDrawY + dataHeight;
}

/* Given the road network, determines its geometry. */
Geometry geometryFor(const DisasterTest& network) {
    Geometry result;
    computeDataBounds(network, result);
    computeGraphicsBounds(result);
    return result;
}

/* Converts a coordinate in logical space into a coordinate in
     * physical space.
     */
GPoint logicalToPhysical(const GPoint& pt, const Geometry& geo) {
    double x = ((pt.getX() - geo.minDataX) / (geo.maxDataX - geo.minDataX)) * (geo.maxDrawX - geo.minDrawX) + geo.minDrawX;
    double y = ((pt.getY() - geo.minDataY) / (geo.maxDataY - geo.minDataY)) * (geo.maxDrawY - geo.minDrawY) + geo.minDrawY;

    return { x, y };
}

/* Draws all the roads in the network, initially unhighlighted. */
void drawRoads(const Geometry& geo,
               const DisasterTest& network) {
    for (const string& source: network.network) {
        for (const string& dest: network.network[source]) {
            /* Draw the line, remembering that the coordinates are in
                 * logical rather than physical space.
                 */
            GLine *line =  new GLine(logicalToPhysical(network.cityLocations[source], geo),
                                     logicalToPhysical(network.cityLocations[dest], geo));
            /* Selected roads draw in the bright color; deselected
                 * roads draw in a the dark color.
                 */
            line->setColor(kDarkRoadColor);
            theWindow()->add(line);

            GRoad road =  { line, source, dest };
            allRoads.add(road);
            gCityMap[source].roads.add(allRoads.size() - 1);
            gCityMap[dest].roads.add(allRoads.size() - 1);
        }
    }
}

/* Draws all the cities, initially unhighlighted. */
void drawCities(const Geometry& geo,
                const DisasterTest& network) {
    for (const string& city: network.network) {
        /* Figure out the center of the city on the screen. */
        auto center = logicalToPhysical(network.cityLocations[city], geo);

        /* Outer "ring" */
        GOval *oval = new GOval(center.getX() - kCityOuterRadius,
                                center.getY() - kCityOuterRadius,
                                2 * kCityOuterRadius, 2 * kCityOuterRadius);
        oval->setFilled(true);
        oval->setColor(kDarkCityColor);
        oval->setFillColor(kDarkCityColor);
        theWindow()->add(oval);
        gCityMap[city].outerCircle = oval;

        /* Inner "circle" */
        oval = new GOval(center.getX() - kCityInnerRadius,
                         center.getY() - kCityInnerRadius,
                         2 * kCityInnerRadius, 2 * kCityInnerRadius);
        oval->setFilled(true);
        oval->setColor(kDarkCityColor);
        oval->setFillColor(kDarkCityColor);
        theWindow()->add(oval);
        gCityMap[city].innerCircle = oval;

        gCityMap[city].isStockpiled = false;
        gCityMap[city].isTryingToBeCovered = false;
    }
}

/* Returns a shortened name for the given city name. We use the first
     * three letters of the name if it's a single word and otherwise use
     * its initials.
     */
string shorthandFor(const string& name) {
    auto components = stringSplit(name, " ");
    if (components.size() == 0) {
        error("It shouldn't be possible for there to be no components of the city name.");
        return "";
    } else if (components.size() == 1) {
        if (components[0].length() < kMaxLength) return components[0];
        else return components[0].substr(0, 3);
    } else {
        /* Use initials. */
        string result;
        for (size_t i = 0; result.length() < kMaxLength && i < components.size(); i++) {
            /* Skip empty components, which might exist if there are consecutive spaces in
                 * the name
                 */
            if (!components[i].empty()) {
                result += components[i][0];
            }
        }
        return result;
    }
}

/* Draws the names of all the cities into the display window,
     * using abbreviations as appropriate.
     */
void drawCityLabels(const Geometry& geo,
                    const DisasterTest& network) {

    for (const string& city: network.network) {
        /* Figure out the center of the city on the screen. */
        auto center = logicalToPhysical(network.cityLocations[city], geo);

        /* Set the label text and color. */
        GText *label = new GText(shorthandFor(city));
        label->setColor(kDarkFontColor);
        label->setFont("Monospace-12");

        theWindow()->add(label,
                         center.getX() - label->getWidth() / 2.0,
                         center.getY() + label->getFontAscent() / 2.0);
        gCityMap[city].label = label;
    }
}
}

/* Displays this network, fully unhighlighted, as the current network onscreen.
 * If a network was previously showing, it is replaced by this one.
 */
void displayNetwork(const DisasterTest& network) {

    /* Remove the existing network graphics, if any */
    for (const string& city : gCityMap) {
        theWindow()->remove(gCityMap[city].outerCircle);
        delete gCityMap[city].outerCircle;
        theWindow()->remove(gCityMap[city].innerCircle);
        delete gCityMap[city].innerCircle;
        theWindow()->remove(gCityMap[city].label);
        delete gCityMap[city].label;
    }

    for (GRoad& road : allRoads) {
        theWindow()->remove(road.line);
        delete road.line;
    }
    gCityMap.clear();
    allRoads.clear();
    currentNetworkGeometry = {};

    clearDisplay();

    /* Now, update to the new network */
    currentNetwork = network;

    /* There's a weird edge case where if there are no cities,
     * the window geometry can't be calculated properly. Therefore,
     * we're going skip all this logic if there's nothing to draw.
     */
    if (!network.network.isEmpty()) {
        currentNetworkGeometry = geometryFor(network);

        /* Draw the roads under the cities to avoid weird graphics
         * artifacts.
         */
        drawRoads(currentNetworkGeometry, network);
        drawCities(currentNetworkGeometry, network);
        drawCityLabels(currentNetworkGeometry, network);
    }
    theWindow()->repaint();
}

void setCitiesToStockpile(const Set<string>& cityNames, bool stockpile) {
    for (const string& city : cityNames) {
        setStockpileInCity(city, stockpile, false);
    }
    while (!coveredStack.isEmpty()) {
        string city = coveredStack.peek();
        setTryingToCoverCity(city, false, false);
    }
    theWindow()->repaint();
}

/**
 * Updates the graphics for this city to to reflect its current state.  Updates include:
 * connecting roads: thick white if connected to a stockpile, thin dark gray otherwise.
 * city center circle: yellow if stockpile, white if covered by another stockpile, dark gray otherwise
 * city outer ring: light blue if currently being considered for covering, dark blue if still
 * being considered, but earlier in the chain, for covering, and dark gray if not being considered.
 *
 * @param cityName the name of the city to update
 * @param stockpile true if this city should be marked a stockpile, false if not
 * @param repaint (optional) whether to re-render the visualizer immediately after this update
 */
void updateColorsFor(const std::string& cityName) {

    /* Update the color of all the road lines */
    for (int roadIndex: gCityMap[cityName].roads) {
        if (gCityMap[allRoads[roadIndex].city1].isStockpiled ||
                gCityMap[allRoads[roadIndex].city2].isStockpiled) {
            allRoads[roadIndex].line->setColor(kStockpileCityColor);
            allRoads[roadIndex].line->setLineWidth(3.0);
        } else {
            allRoads[roadIndex].line->setColor(kDarkRoadColor);
            allRoads[roadIndex].line->setLineWidth(1.0);
        }
    }

    /* Update center circle */
    if (gCityMap[cityName].isStockpiled) {
        gCityMap[cityName].innerCircle->setColor(kStockpileCityColor);
        gCityMap[cityName].innerCircle->setFillColor(kStockpileCityColor);
        gCityMap[cityName].label->setColor(kLightFontColor);
    } else {
        /* Find out whether this city is still covered */
        bool isCovered = false;
        for (int roadIndex : gCityMap[cityName].roads) {
            GRoad r = allRoads[roadIndex];
            string otherCity = r.city1 == cityName ? r.city2 : r.city1;
            if (gCityMap[otherCity].isStockpiled) {
                isCovered = true;
                break;
            }
        }

        if (isCovered) {
            gCityMap[cityName].innerCircle->setColor(kCoveredCityColor);
            gCityMap[cityName].innerCircle->setFillColor(kCoveredCityColor);
            gCityMap[cityName].label->setColor(kLightFontColor);
        } else {
            gCityMap[cityName].innerCircle->setColor(kDarkCityColor);
            gCityMap[cityName].innerCircle->setFillColor(kDarkCityColor);
            gCityMap[cityName].label->setColor(kDarkFontColor);
        }
    }

    /* Update outer ring */
    if (gCityMap[cityName].isTryingToBeCovered) {
        if (!coveredStack.isEmpty() && coveredStack.peek() == cityName) {
            gCityMap[cityName].outerCircle->setColor(kConsideringCityColor);
            gCityMap[cityName].outerCircle->setFillColor(kConsideringCityColor);
        } else if (!coveredStack.isEmpty()) {
            gCityMap[cityName].outerCircle->setColor(kPreviouslyConsideringCityColor);
            gCityMap[cityName].outerCircle->setFillColor(kPreviouslyConsideringCityColor);

        } else {
            throw "Trying to color covered city (" + string(cityName) + ") that is not in covered stack";
        }
    } else {
        gCityMap[cityName].outerCircle->setColor(kDarkCityColor);
        gCityMap[cityName].outerCircle->setFillColor(kDarkCityColor);
    }
}

/**
 * Updates the graphics for this city to either highlight it, and the roads connecting
 * to it, in yellow (stockpile), or unhighlight it, and roads connected to it and no
 * other stockpile, to dark gray.
 *
 * @param cityName the name of the city to update
 * @param stockpile true if this city should be marked a stockpile, false if not
 * @param repaint (optional) whether to re-render the visualizer immediately after this update
 */
void setStockpileInCity(const std::string& cityName, bool stockpile, bool repaint) {
    GThread::runOnQtGuiThread([&]{
        gCityMap[cityName].isStockpiled = stockpile;
        updateColorsFor(cityName);
        for (int roadIndex : gCityMap[cityName].roads) {
            GRoad r = allRoads[roadIndex];
            string otherCity = r.city1 == cityName ? r.city2 : r.city1;
            updateColorsFor(otherCity);
        }
        if (repaint) {
            theWindow()->repaint();
        }
    });
}

/**
 * Updates the graphics for this city to either circle it in blue (trying to cover),
 * or in dark gray (not trying to cover).
 *
 * @param cityName the name of the city to update
 * @param stockpile true if this city should be marked that we are trying to cover it, false if not
 * @param repaint (optional) whether to re-render the visualizer immediately after this update
 */
void setTryingToCoverCity(const string &cityName, bool tryingToCover, bool repaint) {
    GThread::runOnQtGuiThread([&]{
        gCityMap[cityName].isTryingToBeCovered = tryingToCover;
        if (tryingToCover) {
            /* Make sure to update the city being considered before this to darken its color */
            if (!coveredStack.isEmpty()) {
                string oldConsideringCity = coveredStack.peek();
                coveredStack.push(cityName);
                updateColorsFor(oldConsideringCity);
                updateColorsFor(cityName);
            } else {
                coveredStack.push(cityName);
                updateColorsFor(cityName);
            }
        } else {
            /* Make sure to update the city being considered before this to lighten its color */
            if (!coveredStack.isEmpty()) {
                string poppedCity = coveredStack.pop();
                if (poppedCity != cityName) {
                    throw "Error: unexpected city being uncovered: " + string(cityName) + ", expected (" + string(poppedCity) + ")";
                }
                updateColorsFor(cityName);

                if (!coveredStack.isEmpty()) {
                    string oldConsideringCity = coveredStack.peek();
                    updateColorsFor(oldConsideringCity);
                }
            }
        }
        if (repaint) {
            theWindow()->repaint();
        }
    });
}
