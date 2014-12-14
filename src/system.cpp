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

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <memory>
#include <iostream>
#include <system_error>
#include <vector>

static int forkExecAndWait(const char *path, const char* const argv[])
{
    using namespace hbcxx;

    block_signals();

    auto childPid = fork();
    if (-1 == childPid)
	return -1;

    if (0 == childPid) {
        // restore normal signal handling within the child
	unblock_signals();

        (void) execv(path, const_cast<char**>(argv));

	// ::execv returns only on error (and having forked we've no
	// longer got anyone to throw an exception at
	std::exit(127);
    }

    auto res = int{};
    auto waitPid = wait(&res);
    if (-1 == waitPid)
	return -1;

    return res;
}

const std::string& hbcxx::unique(void)
{
    static auto uniq = std::string{"-hbcxx-"} + std::to_string(getpid());
    return uniq;
}

int hbcxx::system(const std::string& command)
{
    block_signals();

    auto childPid = fork();
    if (-1 == childPid)
	return -1;

    if (0 == childPid) {
        // restore normal signal handling within the child
	unblock_signals();
	std::exit(propagate_status(::system(command.c_str())));
    }

    auto res = int{};
    auto waitPid = wait(&res);
    if (-1 == waitPid)
	return -1;

    hbcxx::poll_signals();
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

void hbcxx::exec(const std::string& command,
                  const std::list<std::string>& args)
{
    // prepare the arguments for ::execv before forking whilst it is easier
    // for us to handle the errors.
    auto path = command.c_str();
    auto argv = std::vector<const char*>{};
    for (auto& arg : args)
        argv.push_back(arg.c_str());
    argv.push_back(nullptr);

    (void)execv(path, const_cast<char**>(argv.data()));

    // ::execv only returns if there is an error
    throw std::system_error{std::error_code{errno, std::system_category()}};
}

void hbcxx::setenv(const std::string& name, const std::string& value,
                   bool overwrite)
{
	auto res = ::setenv(name.c_str(), value.c_str(), overwrite);
	if (0 != res)
            throw std::system_error{
                std::error_code{errno, std::system_category()}};
}

void hbcxx::unsetenv(const std::string& name)
{
	auto res = ::unsetenv(name.c_str());
	if (0 != res)
            throw std::system_error{
                std::error_code{errno, std::system_category()}};
}

int hbcxx::propagate_status(int status)
{
    // -1 means failure to fork or wait for the child process
    if (-1 == status)
	throw std::system_error{std::error_code{errno, std::system_category()}};

    // propagate the signal handling decision of the child process to
    // our caller if the sub-process terminated abnormally.
    if (WIFSIGNALED(status)) {
	auto signum = WTERMSIG(status);

        if (signum == SIGQUIT) {
	    // the child has dumped core... make sure we avoid this
	    struct rlimit rlim;
	    (void) getrlimit(RLIMIT_CORE, &rlim);
	    rlim.rlim_cur = 0;
	    (void) setrlimit(RLIMIT_CORE, &rlim);
	}

	// re-pend the signal and restore normal signal handling
        (void) kill(getpid(), signum);
        unblock_signals();
    }

    assert(WIFEXITED(status));
    return WEXITSTATUS(status);
}

void hbcxx::block_signals()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    (void) sigprocmask(SIG_BLOCK, &set, NULL);
}

void hbcxx::unblock_signals()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    (void) sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void hbcxx::poll_signals()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    struct timespec t{};

    int signum = sigtimedwait(&set, NULL, &t);
    if (signum > 0) {
	// re-pend the signal
        (void)kill(getpid(), signum);
        throw signal_exception{};
    }
}
