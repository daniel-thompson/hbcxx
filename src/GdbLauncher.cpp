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

#include "system.h"
#include "Options.h"

GdbLauncher::GdbLauncher(std::string gdb)
    : _gdb{std::move(gdb)}
{
}

int GdbLauncher::launch(const CompilationUnit& unit,
                             const std::list<std::string>& args)
{
    auto executable = unit.getExecutableFileName();
    auto command = _gdb;

    // set up the exec wrapper to fixup arg0 for us
    hbcxx::setenv("HBCXX_SUBSTITUTE_ARG0", unit.getInputFileName());
    command += std::string{" -ex 'set exec-wrapper "};
    command += Options::commandName();
    command += '\'';

    command += " --args ";
    command += executable;
    for (auto& arg : args)
        command += std::string{" '"} + arg + "' ";

    if (Options::verbose())
	std::cerr << "hbcxx: running: " << command << std::endl;
    return hbcxx::system(command.c_str());
}

