/*
 * Toolset.cpp
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

#include "Toolset.h"

#include <cstdlib>
#include <iostream>
#include <utility>

#include "CompilationUnit.h"
#include "Options.h"

Toolset::Toolset() : _cxx{"ccache g++"}
{
	auto cxx = std::getenv("CXX");
	if (nullptr != cxx)
		_cxx = cxx;

	auto debugger = Options::debugger();
	if (!debugger.empty())
	    pushFlag(std::string{"-g"});

	auto level = Options::optimization();
	if (!level.empty())
	    pushFlag(std::string{"-O"} + level, FlagLate);
}

Toolset::~Toolset()
{
}

void Toolset::pushFlag(std::string flag, FlagPosition position)
{
    switch (position) {
    case FlagEarly:
	_flags.push_front(std::move(flag));
	break;
    case FlagNormal:
	_flags.push_back(std::move(flag));
	break;
    case FlagLate:
	_lateFlags.push_back(std::move(flag));
	break;
    default:
	assert(0);
    }
}

void Toolset::pushFlags(const std::list<std::string>& flags, FlagPosition position)
{
    for (auto flag : flags)
	pushFlag(flag, position);
}

void Toolset::compile(const CompilationUnit& unit,
                      const std::list<std::string>& privateFlags)
{
    auto command = _cxx + " -std=c++11 -c " + unit.getProcessedFileName()
                   + " -o " + unit.getObjectFileName();

    for (const auto& flag : _flags)
	command += std::string{" '"} + flag + "'";
    for (const auto& flag : privateFlags)
	command += std::string{" '"} + flag + "'";
    for (const auto& flag : _lateFlags)
	command += std::string{" '"} + flag + "'";

    if (Options::verbose())
	std::cerr << "hbcxx: running: " << command << std::endl;
    auto res = system(command.c_str());
    if (0 != res)
	throw ToolsetError{};
}

void Toolset::link(const std::list<CompilationUnit>& units)
{
    auto command = _cxx + " -std=c++11 -o "
                   + units.front().getExecutableFileName();
    for (auto& i : units) {
	command += ' ';
        command += i.getObjectFileName();
    }

    for (const auto& flag : _flags)
	command += std::string{" '"} + flag + "'";

    if (Options::verbose())
	std::cerr << "hbcxx: running: " << command << std::endl;
    auto res = system(command.c_str());
    if (0 != res)
	throw ToolsetError{};
}
