#!/usr/bin/env hbcxx
#!source: ./startswith.cpp

/*
 * source.cpp
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
 * \file source.cpp
 *
 * Unit test for source directive
 */

void dummy();

// this file has no main() function, so if source works then we run the
// startswith.cpp test case (that means this test will spuriously fail if
// startswith() regresses).
