/*
 * string.h
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

#ifndef HBCXX_STRING_H_
#define HBCXX_STRING_H_

#include <cctype>
#include <list>
#include <string>
#include <utility>

namespace hbcxx {

/*!
 * Perform shell style lexing and tokenization.
 *
 * Does not expand globs.
 *
 * \todo Currently shlex does not expand environment variables.
 */
template<class C = std::list<std::string>>
C shlex(std::string s)
{
    auto result = C{};

    auto accumulator = std::string{};
    auto quote = char{};
    auto escape = bool{};

    auto evictAccumulator = [&]() {
        if (!accumulator.empty()) {
            result.push_back(std::move(accumulator));
            accumulator = "";
        }
    };

    for (auto c : s) {
	if (escape) { 
	    // start escape
	    escape = false;
	    accumulator += c;
	} else if (c == '\\') {
	    // complete escape
	    escape = true;
        } else if ((quote == '\0' && c == '\'') ||
	           (quote == '\0' && c == '\"')) {
	    // start quoted sequence
	    quote = c;
        } else if ((quote == '\'' && c == '\'') || 
	           (quote == '"'  && c == '"')) {
	    // end quoted sequence
	    quote = '\0';
	} else if (!isspace(c) || quote != '\0' ) {
	    // accumulate character (which is either non-whitespace or quoted)
	    accumulator += c;
	} else {
	    // evict accumulator
	    evictAccumulator();
	}
    }

    evictAccumulator();

    return result;
}

}; // namespace hpcxx

#endif // HPCXX_STRING_H_
