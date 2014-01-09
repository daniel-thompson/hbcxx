#!/usr/bin/env hbcxx
#! -DDISABLE_SHOW

/*
 * startswith.cpp
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

/*!
 * \file startswith.cpp
 *
 * Unit test for one of the batteries we can the distro to include.
 */

#include <boost/algorithm/string.hpp>

#include <cassert>
#include <iostream>

using boost::starts_with;

template<class C, class D>
void do_test(const C& full, const D& sub, bool expected)
{
    using std::cout;
    using std::endl;

    bool result = starts_with(full, sub);
#ifndef DISABLE_SHOW
    cout << "starts_with(\"" << full << "\", \"" << sub << "\")  ->  ";
    if (result == expected)
	cout << (result ? "true" : "false") << endl;
    else
	cout << (result ? "TRUE" : "FALSE");
#endif
    assert(result == expected);
}

int main()
{
    do_test("This is long", "This is", true);
    do_test("This is exact", "This is exact", true);
    do_test("This is too", "This is too long", false);
    do_test("Total mismatch", "Fish", false);

    auto s1 = std::string{"One"};
    auto s2 = std::string{"Two"};
    auto s12 = std::string{"One Two"};

    do_test(s1, "O", true);
    do_test(s1, "One Two", false);
    do_test(s1, s2, false);
    do_test(s12, s1, true);

    return 0;
}
