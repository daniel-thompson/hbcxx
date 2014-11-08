/*
 * CompilationUnit.h
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

#ifndef HBCXX_COMPILATION_UNIT_H_
#define HBCXX_COMPILATION_UNIT_H_

#include <list>
#include <string>

#include <boost/filesystem.hpp>

class CompilationUnit {
public:
    CompilationUnit(const CompilationUnit& that);
    CompilationUnit(std::string fname);
    ~CompilationUnit();

    std::string getInputFileName() const;
    std::unique_ptr<std::ofstream> openProcessedFile();
    std::string getProcessedFileName() const;
    std::string getObjectFileName();
    std::string getExecutableFileName() const;

    void removeTemporaryFiles();

    const std::list<std::string>& getFlags() const;
    void pushFlags(std::string flags);

    const std::list<std::string>& getPrivateFlags() const;
    void pushPrivateFlags(std::string flags);

private:
    bool _hasProcessedFile;
    bool _hasObjectFile;
    std::string _originalFileName;
    std::string _processedFileName;
    std::list<std::string> _flags;
    std::list<std::string> _privateFlags;
};

#endif // HBCXX_COMPILATION_UNIT_H_
