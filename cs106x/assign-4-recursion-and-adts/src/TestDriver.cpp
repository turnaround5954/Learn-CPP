#include "TestDriver.h"
#include <string>
#include "simpio.h"
using namespace std;

void doFail(const char* message, size_t line, const char* filename) {
    throw TestFailedException(message, line, filename);
}
void doExpect(bool condition, const char* expression, size_t line, const char* filename) {
    if (!condition) {
        doFail(expression, line, filename);
    }
}

/* TestFailedException implementation. */
TestFailedException::TestFailedException(const char* message, std::size_t line, const char*)
    : logic_error("Line " + to_string(line) + ": " + string(message)) {

}

/* TestCaseAdder implementation. */
TestCaseAdder::TestCaseAdder(TestGroup& group, const string& name, TestCase test) {
    if (group.containsKey(name)) error("Two or more tests have the same name \"" + name + ".\"");
    group.put(name, test);
}

/* Runs all the tests in a given test group. */
void runTestsIn(const string& name, const TestGroup& tests) {
    cout << "==== Testing " << name << " ====" << endl;

    size_t numTests = 0;
    size_t numSuccesses = 0;

    /* Run each test. */
    for (auto testName: tests) {
        auto testCase = tests[testName];
        try {
            testCase();
            cout << "    PASS: " << testName << endl;
            numSuccesses++;
        } catch (const TestFailedException& e) {
            cerr << " !! FAIL:  " << testName << endl;
            cerr << "      " << e.what() << endl;
        } catch (const ErrorException& e) {
            cerr << " !! ERROR: " << testName << endl;
            cerr << "      Test failed due to the program triggering an ErrorException." << endl;
            cerr << endl;
            cerr << "      This means that the test did not fail because of a call" << endl;
            cerr << "      to fail() or an expect() failing, but rather because" << endl;
            cerr << "      some code explicitly called the error() function." << endl;
            cerr << endl;
            cerr << "      Error: " << e.getMessage() << endl;
        } catch (const exception& e) {
            cerr << " !! ERROR: " << testName << endl;
            cerr << "      Test failed due to the program triggering an exception." << endl;
            cerr << endl;
            cerr << "      This means that the test did not fail because of a call" << endl;
            cerr << "      to fail() or an expect() failing, but rather because" << endl;
            cerr << "      some code - probably an internal C++ library - triggered" << endl;
            cerr << "      an error." << endl;
            cerr << endl;
            cerr << "      Error: " << e.what() << endl;
        } catch (...) {
            cerr << " !! FAIL: " << testName << endl;
            cerr << "      Test failed due to the program triggering an unknown type" << endl;
            cerr << "      of exception. " << endl;
            cerr << endl;
            cerr << "      This means that the test did not fail because of a call" << endl;
            cerr << "      to fail() or an expect() failing, but rather because" << endl;
            cerr << "      some code triggered an error whose format we couldn't" << endl;
            cerr << "      recognize." << endl;
            cerr << endl;
        }
        numTests++;
    }

    cout << endl;
    cout << "Summary: " << numSuccesses << " / " << numTests
         << " test" << (numSuccesses == 1? "" : "s") << " passed." << endl;
    getLine("Press ENTER to continue... ");
    cout << endl;
}
