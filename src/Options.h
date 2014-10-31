/*
 * Options.h
 *
 * Part of hbcxx - executable C++ source code
 *
 * Copyright (C) 2014 Daniel Thompson <daniel@redfelineninja.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef HBCXX_OPTIONS_H_
#define HBCXX_OPTIONS_H_

#include <string>

namespace Options {

bool verbose();
bool saveTemps();
const std::string& cxx();
const std::string& debugger();
const std::string& executable();
const std::string& optimization();

/*!
 * Filter arguments and process hbcxx arguments.
 *
 * \returns true if the argument is a hash bang argument, false otherwise.
 */
bool checkArgument(const std::string& args);

/*!
 * Read arguments from a file.
 */
void parseOptionsFile(const std::string& fname);

void showUsage();

}; // namespace Options

#endif // HBCXX_OPTIONS_H_
