/*
 * CompilationUnit.cpp
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

#include "CompilationUnit.h"

#include <utility>

#include <boost/filesystem.hpp>

#include "filesystem.h"
#include "string.h"
#include "system.h"
#include "Options.h"
#include "PrePreProcessor.h"

using hbcxx::shlex;
namespace file = boost::filesystem;

/*!
 * Find somewhere safe to write the file.
 *
 * We work on the assumption that the user's own home directory
 * is always writeable. Strictly speaking this is not always
 * true but is true enough for most realistics cases.
 */
static file::path makeWriteable(const file::path& path)
{
    auto exists = file::exists(path);
    if (hbcxx::touch(path.string())) {
	if (!exists)
            file::remove(path);
        return file::path{path};
    }

    auto home = std::getenv("HOME");
    if (nullptr == home)
	throw PrePreProcessorError{};

    auto filename = file::path{home};
    filename /= ".hbcxx";
    filename /= file::absolute(path);

    (void) file::create_directories(filename.parent_path());

    exists = file::exists(filename);
    if (!hbcxx::touch(filename.string()))
	throw PrePreProcessorError{};

    if (!exists)
	file::remove(filename);

    return filename;
}

CompilationUnit::CompilationUnit(const CompilationUnit& that)
    : _compileInPlace{that._compileInPlace}
    , _originalFileName{that._originalFileName}
    , _processedFileName{that._processedFileName}
    , _flags{that._flags}
    , _privateFlags{that._privateFlags}
{
}

CompilationUnit::CompilationUnit(std::string fname)
    : _compileInPlace{false}
    , _originalFileName{fname}
    , _processedFileName{}
    , _flags{}
    , _privateFlags{}
{
    auto unique = hbcxx::unique();
    auto original = file::path{fname};
    auto parent = original.parent_path();
    auto candidate = file::path{_originalFileName + unique + ".cpp"};

    // improve the candidate file name if needed
    auto extension = original.extension();
    for (auto sourceExtension : {".cpp", ".c++", ".C", ".cc", ".c"}) {
        if (extension == sourceExtension) {
            auto filename = parent / original.stem();
            filename += unique;
            filename += extension;
	    candidate = filename;
	    break;
        }
    }

    // we will be forced to compile from a different directory so we must
    // use -iquote to ensure relative paths to #include directives work as
    // expected.
    auto filename = makeWriteable(candidate);
    if (filename != candidate)
	pushPrivateFlags(std::string{"-iquote "} + parent.string());

    _processedFileName = filename.string();
}

CompilationUnit::~CompilationUnit()
{
}

std::string CompilationUnit::getInputFileName() const
{
    return _originalFileName;
}

std::string CompilationUnit::getProcessedFileName() const
{
    if (_compileInPlace)
	return _originalFileName;
    return _processedFileName;
}

std::string CompilationUnit::getObjectFileName() const
{
    auto processed = file::path{_processedFileName};
    auto filename = processed.parent_path() / processed.stem();
    filename += ".o";
    return makeWriteable(filename).string();
}

std::string CompilationUnit::getExecutableFileName() const
{
    auto executable = Options::executable();
    if (!executable.empty())
	return executable;

    auto original = file::path{_originalFileName};
    auto extension = original.extension();
    if (extension == ".exe") {
	auto filename = original.parent_path() / original.stem();
        filename += "-hbcxx.exe";
	return makeWriteable(filename).string();
    }

    original += ".exe";
    return makeWriteable(original).string();
}

void CompilationUnit::setCompileInPlace(bool cip)
{
    _compileInPlace = cip;
}

void CompilationUnit::removeTemporaryFiles() const
{
    if (Options::saveTemps())
	return;

    auto files = std::string{};

    if (!_compileInPlace) {
        file::remove(getProcessedFileName());
	files += std::string{' '} + getProcessedFileName();
    }
    file::remove(getObjectFileName());
    files += std::string{' '} + getObjectFileName();

    if (Options::verbose())
	std::cerr << "hbcxx: removed temporary files:" << files << '\n';
}

const std::list<std::string>& CompilationUnit::getFlags() const
{
    return _flags;
}

void CompilationUnit::pushFlags(std::string flags)
{
    for (auto& flag : shlex(flags))
	_flags.push_back(std::move(flag));
}

const std::list<std::string>& CompilationUnit::getPrivateFlags() const
{
    return _privateFlags;
}

void CompilationUnit::pushPrivateFlags(std::string flags)
{
    for (auto& flag : shlex(flags))
	_privateFlags.push_back(std::move(flag));
}

