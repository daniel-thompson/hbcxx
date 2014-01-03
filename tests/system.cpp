#!/usr/bin/env hbcxx
#! -DDISABLE_SHOW

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

/*!
 * \file system.cpp
 *
 * Unit test for system().
 */

#include "../src/system.h"

#include <cassert>
#include <string>
#include <iostream>
#include <memory>

using namespace hbcxx;

int main()
{
    auto p = std::unique_ptr<std::stringstream>{};
    
    auto res = system("cat /proc/filesystems", p);
    
    auto line = std::string{};
    while (p->good()) {
	std::getline(*p, line);
	std::cout << line << std::endl;
    }

    return res;
}
