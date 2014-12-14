/*
 * GdbLauncher.h
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

#ifndef HBCXX_GDB_LAUNCHER_H_
#define HBCXX_GDB_LAUNCHER_H_

#include "Launcher.h"

class GdbLauncher : public Launcher {
public:
    GdbLauncher(std::string gdb);
    virtual ~GdbLauncher() override {};

    virtual int launch(const CompilationUnit& unit,
                        const std::list<std::string>& args) override;

private:
    std::string _gdb;
};

#endif // HBCXX_GDB_LAUNCHER_H_
