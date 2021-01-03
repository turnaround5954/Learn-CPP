/**
 * File: exp.cpp
 * -------------
 * This file implements the exp.h interface.
 */

#include "exp.h"
#include "error.h"
#include "ssutil.h"
#include "strlib.h"
#include <string>
using namespace std;

/**
 * Implementation notes: Expression
 * --------------------------------
 * The Expression class itself implements only those methods that
 * are not designated as pure virtual.
 */

void Expression::parseDependencies(const Expression *exp,
                                   Vector<std::string> &dependencies) {
    ExpressionType type = exp->getType();

    switch (type) {
    case DOUBLE:
        break;
    case TEXTSTRING:
        break;
    case IDENTIFIER:
        dependencies.add(exp->toString());
        break;
    case COMPOUND: {
        CompoundExp *cexp = (CompoundExp *)exp;
        parseDependencies(cexp->getLHS(), dependencies);
        parseDependencies(cexp->getRHS(), dependencies);
        break;
    }
    case RANGE:
        dependencies.addAll(((RangeExp *)exp)->getDependencies());
    }
}

Expression::Expression() { /* Empty */
}

Expression::~Expression() { /* Empty */
}

/**
 * Implementation notes: DoubleExp
 * -------------------------------
 * The ConstantExp subclass represents a numeric constant.  The eval
 * method simply returns that value.
 */

DoubleExp::DoubleExp(double value) { this->value = value; }

double DoubleExp::eval(SSModel & /* context */) const { return value; }

string DoubleExp::toString() const { return realToString(value); }

ExpressionType DoubleExp::getType() const { return DOUBLE; }

double DoubleExp::getDoubleValue() const { return value; }

/**
 * Implementation notes: TextStringExp
 * -----------------------------------
 * The IdentifierExp subclass represents a text string constant.  The
 * implementation of eval simply returns 0.0.
 */

TextStringExp::TextStringExp(const string &str) { this->str = str; }

double TextStringExp::eval(SSModel & /* context */) const { return 0.0; }

string TextStringExp::toString() const { return str; }

ExpressionType TextStringExp::getType() const { return TEXTSTRING; }

string TextStringExp::getTextStringValue() const { return '"' + str + '"'; }

/**
 * Implementation notes: IdentifierExp
 * -----------------------------------
 * The IdentifierExp subclass represents a variable name.  The
 * implementation of eval looks up that name in the evaluation context.
 */

IdentifierExp::IdentifierExp(const string &name) { this->name = name; }

double IdentifierExp::eval(SSModel &context) const {
    return context.getCellValue(name);
}

string IdentifierExp::toString() const { return name; }

ExpressionType IdentifierExp::getType() const { return IDENTIFIER; }

string IdentifierExp::getIdentifierName() const { return name; }

/**
 * Implementation notes: CompoundExp
 * ---------------------------------
 * The implementation of eval for CompoundExp evaluates the left and right
 * subexpressions recursively and then applies the operator.  Assignment is
 * treated as a special case because it does not evaluate the left operand.
 */

CompoundExp::CompoundExp(const string &op, const Expression *lhs,
                         const Expression *rhs) {
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

CompoundExp::~CompoundExp() {
    delete lhs;
    delete rhs;
}

double CompoundExp::eval(SSModel &context) const {
    double right = rhs->eval(context);
    double left = lhs->eval(context);
    if (op == "+")
        return left + right;
    if (op == "-")
        return left - right;
    if (op == "*")
        return left * right;
    if (op == "/")
        return left / right; // divide by 0.0 gives ±INF

    error("Illegal operator in expression.");
    return 0.0;
}

string CompoundExp::toString() const {
    return '(' + lhs->toString() + ' ' + op + ' ' + rhs->toString() + ')';
}

ExpressionType CompoundExp::getType() const { return COMPOUND; }

string CompoundExp::getOperator() const { return op; }

const Expression *CompoundExp::getLHS() const { return lhs; }

const Expression *CompoundExp::getRHS() const { return rhs; }

/**
 * Implementation notes: RangeExp
 * ---------------------------------
 * Add support for a new expression type of a function applied to a range.
 * A range function expression applies a named function to a cell range.
 */

Map<std::string, rangeFunc> RangeExp::functionMap = {
    {"MIN", min},         {"MAX", max},         {"SUM", sum},
    {"PRODUCT", product}, {"AVERAGE", average}, {"MEAN", average},
    {"MEDIAN", median},   {"STDEV", stdev}};

bool RangeExp::containsFunc(std::string &op) {
    if (functionMap.containsKey(op)) {
        return true;
    }
    return false;
}

RangeExp::RangeExp(const string &op, const string &st, const string &ed,
                   SSModel &context) {
    this->op = op;
    this->st = st;
    this->ed = ed;
    this->dependencies = context.getCellInRange(st, ed);
}

double RangeExp::eval(SSModel &context) const {
    Vector<double> res;
    for (string name : dependencies)
        res.add(context.getCellValue(name));
    return functionMap[op](res);
}

string RangeExp::toString() const { return op + '(' + st + ':' + ed + ')'; }

ExpressionType RangeExp::getType() const { return RANGE; }

Vector<std::string> RangeExp::getDependencies() const { return dependencies; }

/**
 * Implementation notes: EvaluationContext
 * ---------------------------------------
 * The methods in the EvaluationContext class simply call the appropriate
 * method on the map used to represent the symbol table.
 */

// void EvaluationContext::setValue(const string &var, double value) {
//    symbolTable.put(var, value);
//}

// double EvaluationContext::getValue(const string &var) const {
//    return symbolTable.get(var);
//}

// bool EvaluationContext::isDefined(const string &var) const {
//    return symbolTable.containsKey(var);
//}

// bool EvaluationContext::isNameValid(const std::string name) const {
//    return nameIsValid(name);
//}
