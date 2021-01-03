/**
 * File: parser.h
 * --------------
 * This file acts as the interface to the parser module.
 */

#pragma once
#include "exp.h"
#include "tokenscanner.h"
#include <string>

/**
 * Function: parseExp
 * Usage: Expression *exp = parseExp(scanner);
 * -------------------------------------------
 * Parses a complete expression from the specified TokenScanner object,
 * making sure that there are no tokens left in the scanner at the end.
 */

Expression *parseExp(TokenScanner &scanner, SSModel &context);
