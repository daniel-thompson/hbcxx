#!/usr/bin/env hbcxx

/*
 * touch.cpp
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

#include <cassert>

#include <boost/filesystem.hpp>

#include "../src/filesystem.h"

namespace file = boost::filesystem;

int main()
{
    file::remove("empty");
    assert(!file::exists("empty"));
    assert(hbcxx::touch("empty"));
    assert(file::exists("empty"));
    file::remove("empty");

    assert(!hbcxx::touch("/this/path/does/not/exist"));

    return 0;
}
