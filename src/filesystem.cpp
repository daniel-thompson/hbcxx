/*
 * filesystem.cpp
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

#include "filesystem.h"

#include <fstream>

bool hbcxx::touch(const std::string& fname)
{
    std::ofstream f{fname, std::ios::app};
    return f.is_open();
}
