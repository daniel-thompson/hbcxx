/*
 * GdbLauncher.cpp
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

#include "GdbLauncher.h"

#include <cstdlib>
#include <iostream>

#include "Options.h"

int GdbLauncher::launch(const CompilationUnit& unit,
                             const std::list<std::string>& args)
{
    auto executable = unit.getExecutableFileName();
    auto command = std::string{"gdb "};
    command += executable;

    if (!args.empty()) {
        command += " -ex 'break main' -ex 'run";
        for (auto& arg : args) {
            command += " '\"'\"'";
            command += arg;
            command += "'\"'\"'";
        }
        command += "'";
    }

    if (Options::verbose())
	std::cerr << "hbcxx: running: " << command << std::endl;
    return std::system(command.c_str());
}

