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
#include <fstream>
#include <iostream>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "CompilationUnit.h"
#include "Options.h"

namespace file = boost::filesystem;

Toolset::Toolset()
    : _cxx{}
    , _hasCcache{false}
    , _flags{}
    , _lateFlags{}
{
	auto cxx = std::getenv("CXX");
	if (nullptr != cxx) {
            // setting the C compiler in the environment overrides the default
            // (and disables ccache because it might interfere with scan-build)
            _cxx = cxx;
	} else {
	    if (0 == system("ccache --version >/dev/null 2>&1")) {
		_cxx += "ccache ";
		_hasCcache = true;
	    } else {
                std::cerr << PACKAGE_NAME << ": warning: cannot auto-detect "
                                             "ccache (build will be slow)\n";
            }

	    if (cxx11Check("g++")) {
		_cxx += "g++";
	    } else if (cxx11Check("clang++")) {
		_cxx += "clang++";
	    } else {
                std::cerr << PACKAGE_NAME
                          << ": error: cannot auto-detect a C++ compiler\n";
                throw ToolsetError{};
	    }
	}

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
    // handle special flags that are intercepted by the Toolset class
    // rather than being handed to the compiler/linker
    if (boost::starts_with(flag, "--hbcxx-cxx=")) {
	if (std::getenv("CXX"))
	    return; // cxx: is overriden by environment

	if (_hasCcache)
	    _cxx = "ccache ";
	else
	    _cxx = "";

	_cxx += flag.substr(sizeof("--hbcxx-cxx=")-1);

	return;
    }

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

void Toolset::compile(const CompilationUnit& unit)
{
    auto command = _cxx + " -std=c++11 -c " + unit.getProcessedFileName()
                   + " -o " + unit.getObjectFileName();

    for (const auto& flag : _flags)
	command += std::string{" '"} + flag + "'";
    for (const auto& flag : unit.getPrivateFlags())
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

bool Toolset::cxx11Check(std::string cxx)
{
    auto home = std::getenv("HOME");
    if (nullptr == home)
	throw ToolsetError{};

    auto filename = file::path{home};
    filename /= ".hbcxx";
    filename /= "cxx11check.cpp";

    (void) file::create_directories(filename.parent_path());

    // this small C++ program exercises three critical C++11 features
    // (auto types, smart pointers and lambdas) whilst using sufficiently
    // few templates to allow fast syntax checking.
    std::ofstream f{filename.native()};
    f << "#include <memory>\n"
      << "int main()\n"
      << "{\n"
      << "    auto p = std::unique_ptr<int>{new int{0}};\n"
      << "    auto deref = [&](){ return *p; };\n"
      << "    return deref();\n"
      << "}\n";
    f.close();

    auto command = cxx + " -std=c++11 -c " + filename.native() + " >/dev/null 2>&1";
    return (0 == system(command.c_str()));
}
