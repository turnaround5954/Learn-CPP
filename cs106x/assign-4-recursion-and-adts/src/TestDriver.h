#ifndef TestDriver_Included
#define TestDriver_Included

#include <string>

/*** Utility functions to use in the context of your test cases. ***/

/* Checks whether the given condition is true. If so, nothing happens. If the condition
 * is false, then the test fails. For example:
 *
 *     expect(vec.isEmpty());
 *     expect(history.next() == "Dik-dik");
 */
#define expect(condition) /* Something internal you shouldn't worry about. */

/* Checks whether the given expression called the error() handler. If so, nothing happens.
 * If not, then the test fails. You can use this to confirm that an expression that is
 * supposed to trigger an exception indeed does so. For example:
 *
 *     Vector<int> myVec;
 *     expectError(myVec.get(0));    // Nothing happens, since myVec.get(0) does trigger error().
 *     expectError(myVec.isEmpty()); // The test fails because no error would be generated here.
 */
#define expectError(expression) /* Something internal you shouldn't worry about. */

/* Defines a new test case. Each test case you define should be written as
 *
 *    ADD_TEST("Description of the test") {
 *       ... your testing code goes here ...
 *    }
 *
 * These tests will automatically be added into the main test driver.
 */
#define ADD_TEST(description) /* Something internal you shouldn't worry about. */




/*** Internal types used by the testing system. You should not need to use anything ***
 *** below this point.                                                              ***/

#include "linkedhashmap.h"
#include <functional>
#include <stdexcept>

/* Type representing a function that can be used for testing. This is any function that
 * can be called with no arguments.
 */
using TestCase = std::function<void()>;

/* Exception type signifying that a test case failed. */
class TestFailedException: public std::logic_error {
public:
    TestFailedException(const char* message, std::size_t line, const char* filename);
};

/* Type representing a collection of tests. */
using TestGroup = LinkedHashMap<std::string, TestCase>;

/* Object whose sole purpose is to put a test case into a group of tests. */
class TestCaseAdder {
public:
    TestCaseAdder(TestGroup& group, const std::string& name, TestCase test);
};

/**** Defines the macro that adds a new test case. ****/

/* First, undefine ADD_TEST, since we defined it above as a way of "prototyping" it. */
#undef ADD_TEST

/* We need several levels of indirection here because of how the preprocessor works.
 * This first layer expands out to the skeleton of what we want.
 */
#define ADD_TEST(name) DO_ADD_TEST(_testCase, _adder, name, __LINE__)

/* This level of indirection exists so that line will be expanded to __LINE__ and
 * from there to the true line number. We still can't token-paste it here, since
 * the even further level of indirection.
 */
#define DO_ADD_TEST(fn, adder, name, line)\
    static void JOIN(fn, line)();\
    static TestCaseAdder JOIN(adder, line)(GROUP, name, JOIN(fn, line));\
    static void JOIN(fn, line)()

#define JOIN(X, Y) X##Y

/* Function-like macro that (actually) sets up the instantiation of the test group for
 * a file
 */
#define ENABLE_TESTS() TestGroup GROUP

/***** Macros used to implement testing primitives. *****/
void doFail(const char* message, std::size_t line, const char* filename);

#undef expect
#define expect(condition) doExpect(condition, "expect(" #condition "): condition was false.", __LINE__, __FILE__)
void doExpect(bool condition, const char* expression, std::size_t line, const char* filename);

#undef expectError
#define expectError(condition) do {\
    try {\
        (void)(condition); \
        doFail("expectError(" #condition "): no error generated.", __LINE__, __FILE__); \
    } catch (const ErrorException& ) { \
        /* Do nothing. */ \
    }\
} while(0)

/* Run a group of tests */
void runTestsIn(const std::string& name, const TestGroup& tests);


#endif
