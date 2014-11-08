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

#include <fstream>
#include <utility>

#include <boost/filesystem.hpp>

#include "filesystem.h"
#include "string.h"
#include "system.h"
#include "util.h"
#include "Options.h"
#include "PrePreProcessor.h"

using hbcxx::make_unique;
using hbcxx::shlex;
namespace file = boost::filesystem;

/*!
 * Find somewhere safe to write the file.
 *
 * We work on the assumption that the user's own home directory
 * is always writeable. Strictly speaking this is not always
 * true but is true enough for most realistic cases.
 *
 * This function may create empty files as a side-effect. It should
 * only be called when we know if will use the resulting file.
 */
static file::path makeWriteable(const file::path& path)
{
    if (hbcxx::touch(path.string()))
        return file::path{path};

    auto home = std::getenv("HOME");
    if (nullptr == home)
	throw PrePreProcessorError{};

    auto filename = file::path{home};
    filename /= ".hbcxx";
    filename /= file::absolute(path);

    (void) file::create_directories(filename.parent_path());

    if (hbcxx::touch(filename.string()))
        return filename;

    throw PrePreProcessorError{};
}

CompilationUnit::CompilationUnit(const CompilationUnit& that)
    : _hasProcessedFile{that._hasProcessedFile}
    , _hasObjectFile{that._hasObjectFile}
    , _originalFileName{that._originalFileName}
    , _processedFileName{that._processedFileName}
    , _flags{that._flags}
    , _privateFlags{that._privateFlags}
{
}

CompilationUnit::CompilationUnit(std::string fname)
    : _hasProcessedFile{false}
    , _hasObjectFile{false}
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

    _processedFileName = candidate.string();
}

CompilationUnit::~CompilationUnit()
{
}

std::string CompilationUnit::getInputFileName() const
{
    return _originalFileName;
}

std::unique_ptr<std::ofstream> CompilationUnit::openProcessedFile()
{
    auto filename = makeWriteable(_processedFileName);

    if (filename != _processedFileName) {
	// if we are forced to compile from a different directory then we must
	// use -iquote to ensure relative paths to #include directives work as
        // expected.
	auto original = file::path{_originalFileName};
        pushPrivateFlags(std::string{"-iquote"}
                         + original.parent_path().string());
        _processedFileName = filename.string();
    }

    _hasProcessedFile = true;
    return make_unique<std::ofstream>(_processedFileName);
}

std::string CompilationUnit::getProcessedFileName() const
{
    if (!_hasProcessedFile)
	return _originalFileName;
    return _processedFileName;
}

std::string CompilationUnit::getObjectFileName()
{
    auto processed = file::path{_processedFileName};
    auto filename = processed.parent_path() / processed.stem();
    filename += ".o";
    _hasObjectFile = true;
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

void CompilationUnit::removeTemporaryFiles()
{
    if (Options::saveTemps())
	return;

    auto remove = [](const std::string& fname) {
	file::remove(fname);
	if (Options::verbose())
		std::cerr << "hbcxx: removed " << fname << '\n';
    };

    if (_hasProcessedFile)
	remove(getProcessedFileName());
    if (_hasObjectFile)
	remove(getObjectFileName());
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

