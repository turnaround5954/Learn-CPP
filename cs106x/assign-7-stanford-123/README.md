Note that in parser.cpp, I created a vector named track to trace the Expression objects.
When error occurs, I delete expressions in order, so that memory leak can be avoided.
Maybe keeping track of the expression root in recursion is better. XD
