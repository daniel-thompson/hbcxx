/*
 * util.h
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

#ifndef HBCXX_UTIL_H_
#define HBCXX_UTIL_H_

#include <functional>

namespace hbcxx {

/*!
 * A std::make_unique() workalike.
 *
 * std::make_unique() was introduced in C++14 but hbcxx must target
 * C++11 for a year to two longer...
 */
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

/*!
 * Utility class to run clean up at scope exit.
 *
 * Example:
 *
 *     SomeClass sc;
 *     ScopeExit cleanup{[&] { sc.removeTemporaryFiles(); }};
 */
class ScopeExit {
private:
    std::function<void()> _action;
    ScopeExit(const ScopeExit&);
    ScopeExit& operator=(const ScopeExit&);

public:
    explicit ScopeExit(std::function<void()> f) : _action{f} {}
    ~ScopeExit() { if (nullptr != _action) _action(); }
    void runEarly(void) { _action(); _action = nullptr; }
};

}; // namespace hbcxx

#endif // HBCXX_UTIL_H_
