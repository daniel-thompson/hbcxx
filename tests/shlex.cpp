#!/usr/bin/env hbcxx
//#! -DDISABLE_SHOW

/*
 * shlex.cpp
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
 * \file shlex.cpp
 *
 * Unit test for shlex.
 */

#include "../src/string.h"

#include <cassert>
#include <iostream>

using namespace hbcxx;

template<class C>
void show(const C& container, std::ostream& out=std::cout)
{
#ifndef DISABLE_SHOW
    out << "Sequence:" << std::endl;
    for (const auto& i : container)
	out << "  " << i << std::endl;
    out << std::endl;
#endif
}

int main()
{
    auto tokens = shlex("One Two Three Four");
    assert(4 == tokens.size());
    show(tokens);

    tokens = shlex("One\\ One Two\\ Two");
    assert(2 == tokens.size());
    show(tokens);

    tokens = shlex("'One One' \"Two Two\"");
    assert(2 == tokens.size());
    show(tokens);

    tokens = shlex("'One\\'One' \"Two\\\"Two\"");
    assert(2 == tokens.size());
    show(tokens);

    tokens = shlex("'One\"One' \"Two'Two\"");
    assert(2 == tokens.size());
    show(tokens);

    auto vectorOfTokens = shlex<std::list<std::string>>("One Two Three Four");
    assert(4 == vectorOfTokens.size());
    show(vectorOfTokens);

    return 0;
}
