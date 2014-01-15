/*
 * main.cpp
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

#include <signal.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>

#include "string.h"
#include "system.h"
#include "CompilationUnit.h"
#include "Launcher.h"
#include "Options.h"
#include "PrePreProcessor.h"
#include "Toolset.h"

namespace file = boost::filesystem;

class ArgumentError : public std::exception {
};

static std::string handleArguments(std::list<std::string>& args, Toolset& toolset)
{
    auto showHelp = bool{false};

    while (!args.empty()) {
        auto arg = args.front();
	args.pop_front();

	// cherry pick arguments that might be used to trigger help information.
	if (arg == "--help" || arg == "-h")
	    showHelp = true;

	// if the file exists we will try to execute it
	if (file::exists(arg))
	    return arg;

	// otherwise we pass on the flag to the toolchain
	toolset.pushFlag(arg);
    }

    if (showHelp)
	Options::showUsage(); // calls std::exit()

    throw ArgumentError{};
}

static int run(std::list<std::string>& args)
{
    auto ppp = PrePreProcessor{};
    auto toolset = Toolset{};

    auto primaryFile = handleArguments(args, toolset);
    if (primaryFile.empty())
	return 1;

    auto compilationUnits = std::list<CompilationUnit>{primaryFile};

    for (auto& unit : compilationUnits) {
	auto extraUnits = ppp.process(unit);
	toolset.pushFlags(unit.getFlags());

	// add any discovered units that are not already included
	for (auto& extraUnit : extraUnits) {
            auto i = std::find_if(std::begin(compilationUnits),
                                  std::end(compilationUnits),
                                  [&](const CompilationUnit& u) {
                return u.getInputFileName() == extraUnit.getInputFileName();
            });
            if (i == std::end(compilationUnits)) {
		if (Options::verbose())
                    std::cerr << "hbcxx: auto-discovered: "
                              << extraUnit.getInputFileName() << '\n';
                compilationUnits.push_back(extraUnit);
	    }
        }
    }

    for (auto& unit : compilationUnits)
	toolset.compile(unit);

    toolset.link(compilationUnits);

    for (auto& unit : compilationUnits)
        unit.removeTemporaryFiles();

    if (!Options::executable().empty())
        return 0;

    auto launcher = makeLauncher();
    auto& primaryUnit = compilationUnits.front();

    auto res = launcher->launch(primaryUnit, args);
    if (!Options::saveTemps())
        file::remove(primaryUnit.getExecutableFileName());
    return hbcxx::propagate_status(res);
}

int main(int argc, const char* argv[])
{
    // convert the arguments into an easily mutable form
    auto privateArgs = std::list<std::string>{};
    auto args = std::list<std::string>{};
    for (auto i=1; i<argc; i++) {
	auto arg = std::string{argv[i]};
	if (!Options::checkArgument(arg))
	    args.push_back(std::move(arg));
    }

    // command line tools can have *very* simple stop the world
    // exception handling models (or they could just call exit())
    try {
	return run(args);
    }
    catch (PrePreProcessorError& te) {
	// PrePreProcessor should already have logged an error report
	return 127;
    }
    catch (ToolsetError& te) {
	// Toolset should already have logged an error report
	return 126;
    }
    catch (ArgumentError& ae) {
	std::cerr << PACKAGE_NAME << ": cannot find file to execute\n"
	          << "Try '" << PACKAGE_NAME << " --hbcxx-help' for more information.\n";
	return 125;
    }
    catch (std::exception& ex) {
	std::cerr << "Internal error: " << ex.what() << std::endl;
	return 124;
    }

}
