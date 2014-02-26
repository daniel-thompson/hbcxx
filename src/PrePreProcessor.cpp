/*
 * PrePreProcessor.cpp
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

#include "PrePreProcessor.h"

#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

#include "system.h"
#include "CompilationUnit.h"
#include "Options.h"

#ifdef HAVE_STD_REGEX
#include <regex>
namespace re = std;
#else
#include <boost/regex.hpp>
namespace re = boost;
#endif

namespace file = boost::filesystem;

PrePreProcessor::PrePreProcessor()
    : _inputFileName{}
    , _lineno{}
{
}

PrePreProcessor::~PrePreProcessor()
{
}

std::list<CompilationUnit> PrePreProcessor::process(CompilationUnit& unit)
{
    auto extraUnits = std::list<CompilationUnit>{};
    auto failed = bool{false};

    _inputFileName = unit.getInputFileName();
    std::ifstream in{_inputFileName};
    auto outputFileName = unit.getProcessedFileName();
    std::ofstream out{outputFileName};

    if (Options::verbose())
        std::cerr << "hbcxx: prepreprocessing: " << _inputFileName
                  << " -> " << outputFileName << '\n';

    // These regular expressions redundantly place square brackets around the
    // hash character when it is found within the sequence / / # !. This is to
    // allow the pre-pre-processor to operate over itself (that is, it prevents
    // it from reporting an unknown directive error when run upon itself.
    // 
    // For similar reasons the rawHashBangRegex places the #! within a
    // sub-expression (so the #! character sequence can be recalled without
    // being a full / / # ! sequence).
    auto rawHashBangRegex = re::regex{"^([ \t]*)(#!)"};
    auto hashBangRegex = re::regex{"//[#]![ \t]*(.*)$"};
    auto interpreterRegex = re::regex{"//[#]![ \t]*/"};
    auto flagsRegex = re::regex{"//[#]![ \t]*(-.*)$"};
    auto directiveRegex = re::regex{"//[#]![ \t]*([a-zA-Z_]*)[ \t]*:[ \t]*(.*)$"};
    auto localIncludeRegex = re::regex{"^[ \t]*#[ \t]*include[ \t][ \t]*\"([^\"]*)\""};
    auto systemIncludeRegex = re::regex{"^[ \t]*#[ \t]*include[ \t][ \t]*<([^\"]*)>"};

    auto line = std::string{};
    auto match = re::smatch{};

    _lineno = 0;
    out << "#line 1 \"" << _inputFileName << "\"\n";

    while (in.good()) {
	std::getline(in, line);
	_lineno += 1;

	try {
            // phase 1: make compilable by commenting out the #! directives
            line = re::regex_replace(line, rawHashBangRegex, "$1//$2");

            // phase 2: track whether a hash bang directive has been processed
            auto pendingDirective = re::regex_search(line, hashBangRegex);

            // phase 3: process raw flags
            if (re::regex_search(line, match, flagsRegex)) {
                assert(pendingDirective);
                pendingDirective = false;

                unit.pushFlags(match[1]);
            }

            // phase 4: process directives
            if (re::regex_search(line, match, directiveRegex)) {
                assert(pendingDirective);
                pendingDirective = false;

                auto directive = match[1];
                auto value = match[2];
		if (directive == "cxx")
		    unit.pushFlags(std::string{"--hbcxx-cxx="} + value);
                else if (directive == "private")
                    unit.pushPrivateFlags(value);
                else if (directive == "requires")
                    unit.pushFlags(handleRequires(value));
                else if (directive == "source")
                    extraUnits.emplace_back(handleSourceDirective(value));
                else
                    pendingDirective = true; // didn't consume it after all
            }

            // phase 5: identify local includes
            if (re::regex_search(line, match, localIncludeRegex)) {
                auto includeFile = match[1];
                auto sourceFile = findSourceFile(includeFile);
                if (!sourceFile.empty())
                    extraUnits.emplace_back(sourceFile);
            }

            // phase 6: identify "magic" includes
            if (re::regex_search(line, match, systemIncludeRegex)) {
                auto includeFile = match[1];
                auto extraFlags = checkForMagicIncludes(includeFile);
                if (!extraFlags.empty())
                    unit.pushFlags(extraFlags);
            }

	    // phase 7: identify interpreter directives
	    if (re::regex_search(line, interpreterRegex)) {
                assert(pendingDirective);
                pendingDirective = false;
	    }
		

            // phase 8: error reporting
            if (pendingDirective) {
                auto found = re::regex_search(line, match, hashBangRegex);
                // we are *re*matching so this cannot fail
		assert(found == true); 
                std::cerr << _inputFileName << ':' << _lineno
                          << ":1: error: unknown directive: " << match[1]
                          << std::endl;
                failed = true;
            }
        }
	catch (PrePreProcessorError& e) {
	    // contain the error until pre-pre-processing is complete
	    failed = true;
	}

        out << line << '\n';
    }

    if (failed)
	throw PrePreProcessorError{};

    return extraUnits;
}

std::string PrePreProcessor::handleRequires(const std::string& requires)
{

    auto versionCheck = std::string{
        "pkg-config --print-errors --exists '"} + requires + '\'';
    auto versionOutput = std::unique_ptr<std::stringstream>{};
    if (Options::verbose())
	std::cerr << "hbcxx: running: " << versionCheck << std::endl;
    auto versionRes = hbcxx::system(versionCheck, versionOutput);
    if (0 != versionRes) {
        std::cerr << _inputFileName << ':' << _lineno
                  << ":1: error: pkg-config failed\n";
	std::cerr << "     requires: " << requires << std::endl;
        std::cerr << versionOutput->str(); // usually this is empty but just in case...
	throw PrePreProcessorError{};
    }

    // make a copy of requires removing any version number checks
    auto versionCheckRegex = re::regex{"[ \t]+[><=]=[ \t]+[^ \t]+"};
    auto cleanRequires = re::regex_replace(requires, versionCheckRegex, "");

    auto command = std::string{"pkg-config --cflags --libs "} + cleanRequires;
    auto output = std::unique_ptr<std::stringstream>{};
    if (Options::verbose())
	std::cerr << "hbcxx: running: " << command << std::endl;
    int res = hbcxx::system(command, output);
    if (0 != res) {
        std::cerr << _inputFileName << ':' << _lineno
                  << ":1: internal error: pkg-config failed unexpectedly\n";
	std::cerr << "     requires: " << requires << std::endl;
        std::cerr << output->str(); // usually this is empty but just in case...
	throw PrePreProcessorError{};
    }

    return output->str();
}

std::string PrePreProcessor::handleSourceDirective(const std::string& source)
{
    auto sourcePath = file::path{source};

    if (sourcePath.is_relative()) {
	auto unitPath = file::path{_inputFileName};
	sourcePath = unitPath.parent_path() / sourcePath;
    }

    if (Options::verbose())
	std::cerr << "hbcxx: sourcing: " << sourcePath.native() << '\n';
    return sourcePath.native();
}

std::string PrePreProcessor::findSourceFile(const std::string& header)
{
    auto headerPath = file::path{header};
    if (headerPath.is_relative()) {
	auto unitPath = file::path{_inputFileName};
	headerPath = unitPath.parent_path() / headerPath;
    }
    auto stemPath = headerPath.parent_path() / headerPath.stem();

    for (auto extension : { ".cpp", ".c++", ".C", ".cc", ".c" }) {
	auto sourcePath = stemPath;
	sourcePath += file::path{extension};
	if (file::exists(sourcePath)) {
            return sourcePath.native();
	}
    }

    return std::string{};
}

/*!
 * Trigger special "magic" actions when specific header files are included.
 *
 * Boost provides one of the key motivations for including magic within the
 * build system. Boost has a special relationship to C++ and needs to work well
 * out of the box.
 */
std::string PrePreProcessor::checkForMagicIncludes(const std::string& header)
{
    // Automatically include boost libraries
    //
    // TODO: Boost doesn't have nearly as many libraries as it does headers
    //       but nevertheless this list is woefully incomplete...
    //       A list of candidates to be checked (generated by processing the
    //       output of rpm -qa | grep ^boost):
    //         boost-atomic
    //         boost-chrono
    //         boost-context
    //         boost-date-time
    //         boost-graph
    //         boost-iostreams
    //         boost-locale
    //         boost-log
    //         boost-math
    //         boost-python
    //         boost-python3
    //         boost-random
    //         boost-signals
    //         boost-serialization
    //         boost-system
    //         boost-thread
    //         boost-timer
    //         boost-test
    //         boost-wave
    if (header == "boost/filesystem.hpp")
	return "-lboost_filesystem -lboost_system";
    if (header == "boost/program_options.hpp")
	return "-lboost_program_options";
    if (header == "boost/regex.hpp")
	return "-lboost_regex";

    // Recognise ALSA headers and deploy pkg-config
    if (header == "alsa/asoundlib.h" || header == "asoundlib.h") {
	try {
            return handleRequires("alsa");
	}
	catch (PrePreProcessorError &e) {
            // Suppress propagation of errors from handleRequires(). This
            // avoids hbcxx becoming unusable if the header is included but
            // alsa is not integrated with pkg-config. The error will still
            // be reported but it will become non-fatal.
        }
    }

    return std::string{};
}
