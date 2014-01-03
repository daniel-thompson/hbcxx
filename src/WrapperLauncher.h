/*
 * WrapperLauncher.h
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

#ifndef HBCXX_WRAPPER_LAUNCHER_H_
#define HBCXX_WRAPPER_LAUNCHER_H_

#include "Launcher.h"

class WrapperLauncher : public Launcher {
public:
    WrapperLauncher(std::string wrapper);
    virtual ~WrapperLauncher() override {};

    virtual int launch(const CompilationUnit& unit,
                        const std::list<std::string>& args) override;

private:
    std::string _wrapper;

};

#endif // HBCXX_WRAPPER_LAUNCHER_H_
