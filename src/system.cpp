/*
 * system.cpp
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

// For popen()
#define _POSIX_C_SOURCE 2

#include "system.h"

#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <memory>
#include <iostream>
#include <vector>

static int forkExecAndWait(const char *path, const char* const argv[])
{
    auto childPid = fork();
    if (-1 == childPid)
	return -1;

    if (0 == childPid) {
	(void) execv(path, const_cast<char**>(argv));
	std::exit(127); // ::execv returns only on error
    }

    auto res = int{};
    auto waitPid = wait(&res);
    if (-1 == waitPid)
	return -1;

    return res;
}

int hbcxx::system(const std::string& command, std::unique_ptr<std::stringstream>& output)
{
    FILE *p = popen(command.c_str(), "r");

    if (!output)
	output.reset(new std::stringstream{});

    while (!std::feof(p)) {
	char line[256];
	if (nullptr != fgets(line, sizeof(line), p))
            *output << line;
    }

    return pclose(p);
}

int hbcxx::system(const std::string& command, const std::list<std::string>& args)
{
    // prepare the arguments for ::execv before forking whilst it is easier
    // for us to handle the errors.
    auto path = command.c_str();
    auto argv = std::vector<const char*>{};
    for (auto& arg : args)
	argv.push_back(arg.c_str());
    argv.push_back(nullptr);

    return forkExecAndWait(path, argv.data());
}

