/**
 * @file Printer.h
 * @brief Declaration of a prettyprinter for the sample language.
 */
#ifndef PRINTER_H_
#define PRINTER_H_

#include <iostream>
#include "AST.h"
using namespace std;

namespace Printer {
/// Prettyprints the given expression on the given output stream.
void Print(const Expr *node, ostream &o);
}

#endif // PRINTER_H_
