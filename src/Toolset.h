/*
 * Toolset.h
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

#ifndef HBCXX_TOOLSET_H_
#define HBCXX_TOOLSET_H_

class CompilationUnit;

#include <exception>
#include <list>
#include <string>

class Toolset {
public:
    enum FlagPosition {
        FlagEarly,
	FlagNormal,
        FlagLate
    };

    Toolset();
    ~Toolset();

    void pushFlag(std::string flag, FlagPosition position = FlagNormal);

    /*!
     * Update the toolset flags.
     *
     * These flags affect both compile and link stages and can be sent in
     * two forms, single token ("-DTHIS=that") and double token ("-D",
     * "THIS=that").
     */
    void pushFlags(const std::list<std::string>& flags,
                   FlagPosition position = FlagNormal);

    void compile(const CompilationUnit& unit);
    void link(const std::list<CompilationUnit>& units);

private:
    std::string _cxx;
    bool _hasCcache;
    std::list<std::string> _flags;
    std::list<std::string> _lateFlags;
};

class ToolsetError : public std::exception {
public:
    const char* what() const noexcept override
    {
        return "ToolsetError";
    }
}; 

#endif // HBCXX_TOOLSET_H_
