#include "RecursionToTheRescue.h"

#include <climits>
#include <iostream>

#include "Disasters.h"
#include "map.h"
using namespace std;

/* * * * Doctors Without Orders * * * */

bool canAllPatientsBeSeenHelper(Vector<Doctor> &doctors,
                                Vector<Patient> &patients,
                                Map<string, Set<string>> &tmp,
                                Map<string, Set<string>> &schedule) {
    // base case
    if (patients.isEmpty()) {
        schedule = tmp;
        return true;
    }

    // choose
    Patient patient = patients.pop_back();

    // explore
    for (int i = 0; i < doctors.size(); i++) {
        if (doctors[i].hoursFree < patient.hoursNeeded) {
            continue;
        }

        // choose
        doctors[i].hoursFree -= patient.hoursNeeded;

        string doctorName = doctors[i].name;
        if (tmp.containsKey(doctorName)) {
            tmp[doctorName].add(patient.name);
        } else {
            Set<string> patientsSet;
            patientsSet.add(patient.name);
            tmp.add(doctorName, patientsSet);
        }

        // explore
        if (canAllPatientsBeSeenHelper(doctors, patients, tmp, schedule)) {
            return true;
        }

        // unchoose
        doctors[i].hoursFree += patient.hoursNeeded;
        tmp[doctorName].remove(patient.name);
    }

    // unchoose
    patients.push_back(patient);

    return false;
}

/**
 * Given a list of doctors and a list of patients, determines whether all the
 * patients can be seen. If so, this function fills in the schedule outparameter
 * with a map from doctors to the set of patients that doctor would see.
 *
 * @param doctors  The list of the doctors available to work.
 * @param patients The list of the patients that need to be seen.
 * @param schedule An outparameter that will be filled in with the schedule,
 * should one exist.
 * @return Whether or not a schedule was found.
 */
bool canAllPatientsBeSeen(const Vector<Doctor> &doctors,
                          const Vector<Patient> &patients,
                          Map<string, Set<string>> &schedule) {
    // [TODO: Delete these lines and implement this function!]
    // (void)(doctors, patients, schedule);
    // Vector<Doctor> doctorsMutable = Vector<Doctor>(doctors);
    // Vector<Patient> patientsMutable = Vector<Patient>(patients);
    Vector<Doctor> doctorMut = doctors;
    Vector<Patient> patientsMut = patients;
    Map<string, Set<string>> tmp;
    return canAllPatientsBeSeenHelper(doctorMut, patientsMut, tmp, schedule);
}

/* * * * Disaster Planning * * * */
bool canBeMadeDisasterReadyHelper(Map<string, Set<string>> &roadNetwork,
                                  int numCities, Set<string> &chosen,
                                  Set<string> &notReady,
                                  Set<string> &locations) {
    // base case
    if (numCities <= 0) {
        if (notReady.isEmpty()) {
            locations.addAll(chosen);
            return true;
        } else {
            return false;
        }
    }

    if (roadNetwork.isEmpty()) {
        if (!notReady.isEmpty()) {
            return false;
        }
        locations.addAll(chosen);
        return true;
    }

    // choose
    string city;
    for (string candidate : notReady) {
        if (roadNetwork.containsKey(candidate)) {
            city = candidate;
            break;
        }
    }

    if (!notReady.contains(city)) {
        return false;
    }

    Set<string> neighbors = roadNetwork[city];
    chosen.add(city);
    roadNetwork.remove(city);
    Set<string> backtrace;

    notReady.remove(city);
    backtrace.add(city);

    for (string neighbor : neighbors) {
        if (notReady.contains(neighbor)) {
            notReady.remove(neighbor);
            backtrace.add(neighbor);
        }
    }

    // explore with city chosen
    if (canBeMadeDisasterReadyHelper(roadNetwork, numCities - 1, chosen,
                                     notReady, locations)) {
        return true;
    }

    // unchoose (without unchoosing roadNetwork.add(city, neighbors))
    chosen.remove(city);
    notReady.addAll(backtrace);

    // explore with city unchosen
    if (canBeMadeDisasterReadyHelper(roadNetwork, numCities, chosen, notReady,
                                     locations)) {
        return true;
    }

    // unchoose
    roadNetwork.add(city, neighbors);

    return false;
}

/**
 * Given a transportation grid for a country or region, along with the number of
 * cities where disaster supplies can be stockpiled, returns whether it's
 * possible to stockpile disaster supplies in at most the specified number of
 * cities such that each city either has supplies or is connected to a city that
 * does.
 * <p>
 * This function can assume that every city is a key in the transportation map
 * and that roads are bidirectional: if there's a road from City A to City B,
 * then there's a road from City B back to City A as well.
 *
 * @param roadNetwork The underlying transportation network.
 * @param numCities   How many cities you can afford to put supplies in.
 * @param locations   An outparameter filled in with which cities to choose if a
 * solution exists.
 * @return Whether a solution exists.
 */
bool canBeMadeDisasterReady(const Map<string, Set<string>> &roadNetwork,
                            int numCities, Set<string> &locations) {
    // [TODO: Delete these lines and implement this function!]
    // (void)(roadNetwork, numCities, locations);
    Map<string, Set<string>> network = roadNetwork;
    Set<string> citiesChosen, citiesNotReady;
    for (string city : roadNetwork.keys()) {
        citiesNotReady.add(city);
    }
    return canBeMadeDisasterReadyHelper(network, numCities, citiesChosen,
                                        citiesNotReady, locations);
}

/* * * * Winning the Election * * * */

/**
 * Given a list of the states in the election, including their popular and
 * electoral vote totals, and the number of electoral votes needed, as well as
 * the index of the lowest-indexed state to consider, returns information about
 * how few popular votes you'd need in order to win that at least that many
 * electoral votes.
 *
 * @param electoralVotesNeeded the minimum number of electoral votes needed
 * @param states All the states in the election (plus DC, if appropriate)
 * @param minStateIndex the lowest index in the states Vector that should be
 * considered
 */
MinInfo minPopularVoteToGetAtLeast(int electoralVotesNeeded,
                                   const Vector<State> &states,
                                   int minStateIndex) {
    // [TODO: Delete these lines and implement this function!]
    (void)(electoralVotesNeeded);
    (void)(states);
    (void)(minStateIndex);
    return {0, {}};
}

/**
 * Given a list of all the states in an election, including their popular and
 * electoral vote totals, returns information about how few popular votes you'd
 * need to win in order to win the presidency.
 *
 * @param states All the states in the election (plus DC, if appropriate)
 * @return Information about how few votes you'd need to win the election.
 */
MinInfo minPopularVoteToWin(const Vector<State> &states) {
    // [TODO: Delete these lines and implement this function!]
    (void)(states);
    return {0, {}};
}
