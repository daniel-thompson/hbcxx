/*
 * Launcher.cpp
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

#include "Launcher.h"

#include <utility>

#include <boost/algorithm/string.hpp>

#include "DefaultLauncher.h"
#include "GdbLauncher.h"
#include "Options.h"
#include "NoArgsLauncher.h"
#include "WrapperLauncher.h"

std::unique_ptr<Launcher> makeLauncher()
{
    auto debugger = Options::debugger();

    if (debugger.empty())
	return std::unique_ptr<Launcher>{new DefaultLauncher{}};

    if (debugger == "gdb")
	return std::unique_ptr<Launcher>{new GdbLauncher{}};

    if (debugger == "valgrind"
               || boost::starts_with(debugger, "valgrind "))
        return std::unique_ptr<Launcher>{new WrapperLauncher{debugger}};

    return std::unique_ptr<Launcher>{new NoArgsLauncher{debugger}};
}



