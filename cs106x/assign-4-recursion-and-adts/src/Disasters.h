#ifndef Disasters_Included
#define Disasters_Included

#include "map.h"
#include "vector.h"
#include "set.h"
#include "gtypes.h"
#include <string>
#include <istream>

/**
 * Type representing a test case for the Disaster Preparation problem.
 */
struct DisasterTest {
    Map<std::string, Set<std::string>> network; // The road network
    Map<std::string, GPoint> cityLocations;     // Where each city should be drawn
    int minCitiesNeeded;                        // True minimum number of cities needed
};

/**
 * Given a stream pointing at a test case for Disaster Preparation,
 * pulls the data from that test case.
 *
 * @param source The stream containing the test case.
 * @return A test case from the file.
 * @throws ErrorException If an error occurs or the file is invalid.
 */
DisasterTest loadDisaster(std::istream& source);

/**
 * Draws the given road network, with all graphics unhighlighted.
 *
 * @param network The test case containing the network
 */
void displayNetwork(const DisasterTest& network);

/**
 * Updates the graphics for this city to either highlight it, and the roads connecting
 * to it, in yellow (stockpile), or unhighlight it, and roads connected to it and no
 * other stockpile, to dark gray (not stockpile).
 *
 * @param cityName the name of the city to update
 * @param stockpile true if this city should be marked a stockpile, false if not
 * @param repaint (optional) whether to re-render the visualizer immediately after this update
 */
void setStockpileInCity(const std::string& cityName, bool stockpile, bool repaint=true);

/**
 * Updates the graphics for this city to either circle it in blue (trying to cover),
 * or in dark gray (not trying to cover).
 *
 * @param cityName the name of the city to update
 * @param stockpile true if this city should be marked that we are trying to cover it, false if not
 * @param repaint (optional) whether to re-render the visualizer immediately after this update
 */
void setTryingToCoverCity(const std::string& cityName, bool tryingToCover, bool repaint=true);

/**
 * Updates the graphics for these cities to either highlight them, and the roads connecting
 * to them, in yellow (stockpile), or unhighlight them, and roads connected to them and no
 * other stockpile, to dark gray.  Re-renders the visualizer after all updates are finished.
 *
 * @param cityNames the names of the cities to update
 * @param stockpile true if these cities should be marked a stockpile, false if not
 */
void setCitiesToStockpile(const Set<std::string>& cityNames, bool stockpile);

#endif
