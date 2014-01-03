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
 * A std::system() workalike that captures the subprocess' standard output.
 */
int system(const std::string& command, std::unique_ptr<std::stringstream>& io);

/*!
 * A std::system() workalike without shell argument parsing.
 *
 * \todo We cannot call ::execv (too many raw C headers) from here but 
 *       we could translate args into a vector<const char *> and pass that
 *       to a non-template helper function. However I would prefer to wait
 *       for concepts to come along in C++14 before doing this.
 */
int system(const std::string& command, const std::list<std::string>& args);

}; // namespace hbcxx

#endif // HBCXX_SYSTEM_H_
