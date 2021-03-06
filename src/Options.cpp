/*
 * Options.cpp
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

#include "Options.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>

using boost::starts_with;
using boost::trim_left;

static struct {
    bool verbose;
    bool saveTemps;
    std::string commandName;
    std::string cxx;
    std::string debugger;
    std::string executable;
    std::string optimization;
} optionStore = { false, false };bool Options::verbose() { return optionStore.verbose; }
bool Options::saveTemps() { return optionStore.saveTemps; }
const std::string& Options::commandName() { return optionStore.commandName; }
const std::string& Options::cxx() { return optionStore.cxx; }
const std::string& Options::debugger() { return optionStore.debugger; }
const std::string& Options::executable() { return optionStore.executable; }
const std::string& Options::optimization() { return optionStore.optimization; }

void Options::handleArg0(const std::string& arg)
{
    optionStore.commandName = arg;
}

bool Options::checkArgument(const std::string& arg)
{
    if (arg == "--hbcxx-version") {
	std::exit(0);
    }

    if (arg == "--hbcxx-help") {
	Options::showUsage();
    }

    if (arg == "--hbcxx-verbose") {
	optionStore.verbose = true;
	return true;
    }

    if (arg == "--hbcxx-save-temps") {
	optionStore.saveTemps = true;
	return true;
    }

    if (starts_with(arg, "--hbcxx-cxx=")) {
	optionStore.cxx = arg.substr(sizeof("--hbcxx-cxx=")-1);
	return true;
    }

    if (starts_with(arg, "--hbcxx-debugger=")) {
	optionStore.debugger = arg.substr(sizeof("--hbcxx-debugger=")-1);
	return true;
    }

    if (starts_with(arg, "--hbcxx-executable=")) {
	optionStore.executable = arg.substr(sizeof("--hbcxx-executable=")-1);
	return true;
    }

    if (starts_with(arg, "--hbcxx-O")) {
	optionStore.optimization = arg.substr(sizeof("--hbcxx-O")-1);
	return true;
    }

    return false;
}

void Options::parseOptionsFile(const std::string& fname)
{
    std::ifstream in{fname};
    auto ln = std::string{};
    auto option = std::string{};
    auto lineno = 0;

    while (in.good()) {
	std::getline(in, ln);
	lineno++;
	trim_left(ln);

        if (ln.empty() || starts_with(ln, "#"))
	    continue;

	/* extended shortened forms */
	if (starts_with(ln, "hbcxx-"))
	    option = std::string{"--"} + ln;
	else if (!starts_with(ln, "--hbcxx-"))
	    option = std::string{"--hbcxx-"} + ln;
	else
	    option = ln;

	if (!checkArgument(option))
            std::cerr << "WARNING: Bad option at line " << lineno << ": "
                      << ln << '\n';
    }
}

void Options::showUsage()
{
    std::cout
<< "USAGE: hbcxx [TOOLSET OPTION]... [SOURCE FILE] [PROGRAM OPTION]...\n"
<< "Compile, link and execute the supplied source file. Run C++ source\n"
<< "code as a script.\n"
<< '\n'
<< "The following arguments control hbcxx and can be included anywhere on\n"
<< "the command line.\n"
<< '\n'
<< "  --hbcxx-cxx=COMPILER    User COMPILER to compile and link the program\n"
<< "  --hbcxx-debugger=DBG    Use DBG to debug the program\n"
<< "  --hbcxx-executable=EXE  Write executable file to EXE, then exit\n"
<< "  --hbcxx-help            Show this help, then exit\n"
<< "  --hbcxx-save-temps      Do not delete temporary files\n"
<< "  --hbcxx-Ox              Override the optimization level, set to x\n"
<< "  --hbcxx-verbose         Show commands as they are executed\n"
<< "  --hbcxx-version         Show hbcxx version information, then exit\n";

    std::exit(0);
}
