/*
 * DefaultLauncher.cpp
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

#include "DefaultLauncher.h"

#include <iostream>

#include "system.h"
#include "Options.h"

int DefaultLauncher::launch(const CompilationUnit& unit,
                             const std::list<std::string>& args)
{
    auto verbose = Options::verbose();

    auto executable = unit.getExecutableFileName();
    auto command = unit.getInputFileName();

    auto fullArgs = std::list<std::string>{};
    fullArgs.push_back(command);
    for (auto& arg : args) {
	fullArgs.push_back(arg);

	if (verbose) {
            command += " '";
            command += arg;
            command += "'";
	}
    }

    if (verbose)
        std::cerr << "hbcxx: running " << executable << " as: " << command
                  << std::endl;
    return hbcxx::system(executable, fullArgs);
}

