/*
 * system.h
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

#ifndef HBCXX_SYSTEM_H_
#define HBCXX_SYSTEM_H_

#include <list>
#include <string>
#include <memory>
#include <sstream>

namespace hbcxx {

/*!
 * Get a string unique(ish) to this invokation of hbcxx.
 */
const std::string& unique(void);

/*!
 * A std::system() workalike that restore normal signal handling within the
 * child.
 */
int system(const std::string& command);

/*!
 * A std::system() workalike that captures the subprocess' standard output.
 *
 * \todo This command must be used with care. It does *not* restore normal
 *       signal handling before launching child processes meaning the
 *       system may appear unresponsive during sub-process execution.
 */
int system(const std::string& command, std::unique_ptr<std::stringstream>& io);

/*!
 * A std::system() workalike without shell argument parsing.
 *
 * \todo This code currently requires a list be passed in. It would be better
 *       to use a template function here. However we do not want to call
 *       ::execv from the header (would require too many raw C headers).
 *       However we could prepare a vector<const char *> and pass that to
 *       a non-template helper function.
 */
int system(const std::string& command, const std::list<std::string>& args);

/*!
 * Simple ::execv() wrapper.
 *
 * Failures within ::execv with are reported as a std::system_error.
 */
void exec(const std::string& command, const std::list<std::string>& args);

/*!
 * Simple ::setenv() wrapper.
 */
void setenv(const std::string& name, const std::string& value,
            bool overwrite = true);

/*!
 * Simple ::unsetenv() wrapper.
 */
void unsetenv(const std::string& name);

/*!
 * Propagate signals or return exit code.
 *
 * Examine a status value returned from ::wait and its friends.
 *
 * If the status reflects abnormal process termination due to signal
 * handling then propagate this (by abnormally terminating the current
 * process).
 *
 * \returns the exit status of the child
 */
int propagate_status(int res);

/*!
 * Prevent SIGINT and SIGQUIT signals from killing the process.
 */
void block_signals();

/*!
 * Re-enable signals previously blocked using hbcxx::block_signals.
 */
void unblock_signals();

/*!
 * Check is any blocked signals are pending.
 * 
 * Raises an exception if signals are pending.
 */
void poll_signals();

/*!
 * Report that an exception has occured.
 */
class signal_exception : public std::exception {
public:
    const char* what() const noexcept override { return "signal_exception"; }
};

}; // namespace hbcxx

#endif // HBCXX_SYSTEM_H_
