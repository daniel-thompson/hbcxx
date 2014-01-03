/*
 * WrapperLauncher.cpp
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

#include "WrapperLauncher.h"

#include <cstdlib>
#include <iostream>
#include <utility>

#include "Options.h"

WrapperLauncher::WrapperLauncher(std::string wrapper)
    : _wrapper{std::move(wrapper)}
{
}

int WrapperLauncher::launch(const CompilationUnit& unit,
                             const std::list<std::string>& args)
{
    auto command = _wrapper;
    command += ' ';
    command += unit.getExecutableFileName();
    for (auto& arg : args)
	command += std::string{" '"} + arg + "'"; 

    if (Options::verbose())
	std::cerr << "hbcxx: running: " << command << std::endl;
    // NOTE: _wrapper needs to undergo shell-like lexing. std::system() will
    //       do this automatically but if we switch to a more robust approach
    //       then we must pass _wrapper through hbcxx:shlex()
    return std::system(command.c_str());
}

