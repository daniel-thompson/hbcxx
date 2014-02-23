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
#include <iostream>

#include <boost/algorithm/string.hpp>

using boost::starts_with;

static struct {
    bool verbose;
    bool saveTemps;
    std::string debugger;
    std::string executable;
    std::string optimization;
} optionStore = { false, false };

bool Options::verbose() { return optionStore.verbose; }
bool Options::saveTemps() { return optionStore.saveTemps; }
const std::string& Options::debugger() { return optionStore.debugger; }
const std::string& Options::executable() { return optionStore.executable; }
const std::string& Options::optimization() { return optionStore.optimization; }

/*!
 * Filter arguments and process hbcxx arguments.
 *
 * \returns true if the argument is a hash bang argument, false otherwise.
 */
bool Options::checkArgument(const std::string& arg)
{
    if (arg == "--hbcxx-version") {
	std::cout << PACKAGE_STRING << std::endl;
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
<< "  --hbcxx-debugger=DBG    Use DBG to debug the program\n"
<< "  --hbcxx-executable=EXE  Write executable file to EXE, then exit\n"
<< "  --hbcxx-help            Show this help, then exit\n"
<< "  --hbcxx-save-temps      Do not delete temporary files\n"
<< "  --hbcxx-Ox              Override the optimization level, set to x\n"
<< "  --hbcxx-verbose         Show commands as they are executed\n"
<< "  --hbcxx-version         Show hbcxx version information, then exit\n";

    std::exit(0);
}
