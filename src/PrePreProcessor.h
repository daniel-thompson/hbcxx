/*
 * PrePreProcessor.h
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

#ifndef HBCXX_PRE_PRE_PROCESSOR_H_
#define HBCXX_PRE_PRE_PROCESSOR_H_

#include <list>
#include <string>

class CompilationUnit;

class PrePreProcessor {
public:
    PrePreProcessor();
    ~PrePreProcessor();

    std::list<CompilationUnit> process(CompilationUnit& unit);

private:
    std::string handleRequires(const std::string& requires);
    std::string handleSourceDirective(const std::string& requires);
    std::string findSourceFile(const std::string& header);
    std::string checkForMagicIncludes(const std::string& header);

    std::string _inputFileName;
    int _lineno;
};

class PrePreProcessorError : public std::exception {
public:
    const char* what() const noexcept override
    {
        return "PrePreProcessorError";
    }
};

#endif // HBCXX_PRE_PRE_PROCESSOR_H_
