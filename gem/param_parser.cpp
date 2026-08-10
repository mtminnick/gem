/*
 * Copyright (c) 2022, Michael Minnick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// -----------------------------------------------------------------------------
// Common parsing helpers
// -----------------------------------------------------------------------------

static bool ParseInt(std::string_view text, int& value)
{
    const auto result = std::from_chars(
        text.data(),
        text.data() + text.size(),
        value);

    return result.ec == std::errc{} &&
        result.ptr == text.data() + text.size();
}

static bool ParseFloat(std::string_view text, float& value)
{
    const auto result = std::from_chars(
        text.data(),
        text.data() + text.size(),
        value);

    return result.ec == std::errc{} &&
        result.ptr == text.data() + text.size();
}

static bool ParseIntParameter(
    const std::string& token,
    std::string_view keyword,
    int& value)
{
    const std::string prefix = std::string(keyword) + "=";

    if (!token.starts_with(prefix))
        return false;

    const std::string_view text(
        token.data() + prefix.size(),
        token.size() - prefix.size());

    return ParseInt(text, value);
}

static bool ParseFloatParameter(
    const std::string& token,
    std::string_view keyword,
    float& value)
{
    const std::string prefix = std::string(keyword) + "=";

    if (!token.starts_with(prefix))
        return false;

    const std::string_view text(
        token.data() + prefix.size(),
        token.size() - prefix.size());

    return ParseFloat(text, value);
}

static bool ParseIntListParameter(
    const std::string& token,
    std::string_view keyword,
    std::vector<int>& values)
{
    const std::string prefix = std::string(keyword) + "=";

    if (!token.starts_with(prefix))
        return false;

    std::string_view text(
        token.data() + prefix.size(),
        token.size() - prefix.size());

    if (text.empty())
        return false;

    values.clear();

    while (!text.empty()) {
        const std::size_t comma = text.find(',');

        std::string_view element;

        if (comma == std::string_view::npos) {
            element = text;
            text = {};
        }
        else {
            element = text.substr(0, comma);
            text.remove_prefix(comma + 1);
        }

        int value;

        if (element.empty() || !ParseInt(element, value))
            return false;

        values.push_back(value);
    }

    return !values.empty();
}

bool ParseAddBlock(
    int argc,
    const std::vector<std::string>& argv,
    int& voice)
{
    if (argc != 3 ||
        argv[0] != "add" ||
        argv[1] != "block") {
        std::cerr << "Usage: add block voice=<n>\n";
        return false;
    }

    if (!ParseIntParameter(argv[2], "voice", voice)) {
        std::cerr << "Invalid voice parameter.\n";
        return false;
    }

    return true;
}

static bool ParseBlockValue(
    int argc,
    const std::vector<std::string>& argv,
    std::string_view parameter,
    int& voice,
    int& block,
    std::vector<int>& values)
{
    if (argc != 5 ||
        argv[0] != "set" ||
        argv[1] != parameter) {
        std::cerr
            << "Usage: set " << parameter
            << " voice=<n> block=<n> value=<n,n,...>\n";
        return false;
    }

    if (!ParseIntParameter(argv[2], "voice", voice)) {
        std::cerr << "Invalid voice parameter.\n";
        return false;
    }

    if (!ParseIntParameter(argv[3], "block", block)) {
        std::cerr << "Invalid block parameter.\n";
        return false;
    }

    if (!ParseIntListParameter(argv[4], "value", values)) {
        std::cerr << "Invalid value parameter.\n";
        return false;
    }

    return true;
}

bool ParseSetRhythm(
    int argc,
    const std::vector<std::string>& argv,
    int& voice,
    int& block,
    std::vector<int>& values)
{
    return ParseBlockValue(
        argc, argv, "rhythm", voice, block, values);
}

bool ParseSetPitch(
    int argc,
    const std::vector<std::string>& argv,
    int& voice,
    int& block,
    std::vector<int>& values)
{
    return ParseBlockValue(
        argc, argv, "pitch", voice, block, values);
}

bool ParseSetVelocity(
    int argc,
    const std::vector<std::string>& argv,
    int& voice,
    int& block,
    std::vector<int>& values)
{
    return ParseBlockValue(
        argc, argv, "velocity", voice, block, values);
}

bool ParseSetChord(
    int argc,
    const std::vector<std::string>& argv,
    int& voice,
    int& block,
    std::vector<int>& values)
{
    return ParseBlockValue(
        argc, argv, "chord", voice, block, values);
}

bool ParseSetAutosubmit(
    int argc,
    const std::vector<std::string>& argv,
    bool& enabled)
{
    if (argc != 3 ||
        argv[0] != "set" ||
        argv[1] != "autosubmit") {
        std::cerr << "Usage: set autosubmit {on|off}\n";
        return false;
    }

    if (argv[2] == "on") {
        enabled = true;
    }
    else if (argv[2] == "off") {
        enabled = false;
    }
    else {
        std::cerr << "Expected 'on' or 'off'.\n";
        return false;
    }

    return true;
}

bool ParseSetMute(
    int argc,
    const std::vector<std::string>& argv,
    int& voice,
    bool& mute)
{
    if (argc != 4 ||
        argv[0] != "set" ||
        argv[1] != "mute") {
        std::cerr << "Usage: set mute voice=<n> {on|off}\n";
        return false;
    }

    if (!ParseIntParameter(argv[2], "voice", voice)) {
        std::cerr << "Invalid voice parameter.\n";
        return false;
    }

    if (argv[3] == "on") {
        mute = true;
    }
    else if (argv[3] == "off") {
        mute = false;
    }
    else {
        std::cerr << "Expected 'on' or 'off'.\n";
        return false;
    }

    return true;
}

bool ParseSetDuration(
    int argc,
    const std::vector<std::string>& argv,
    int& voice,
    int& block,
    float& seconds,
    bool& loop)
{
    if (argc != 5 ||
        argv[0] != "set" ||
        argv[1] != "duration") {
        std::cerr
            << "Usage: set duration voice=<n> block=<n> "
            "{seconds=<n.n>|loop}\n";
        return false;
    }

    if (!ParseIntParameter(argv[2], "voice", voice)) {
        std::cerr << "Invalid voice parameter.\n";
        return false;
    }

    if (!ParseIntParameter(argv[3], "block", block)) {
        std::cerr << "Invalid block parameter.\n";
        return false;
    }

    if (argv[4] == "loop") {
        loop = true;
        seconds = 0.0f;
        return true;
    }

    if (!ParseFloatParameter(argv[4], "seconds", seconds)) {
        std::cerr
            << "Expected seconds=<n.n> or loop.\n";
        return false;
    }

    loop = false;
    return true;
}

bool ParseSetChannel(
    int argc,
    const std::vector<std::string>& argv,
    int& voice,
    int& channel)
{
    if (argc != 4 ||
        argv[0] != "set" ||
        argv[1] != "channel") {
        std::cerr
            << "Usage: set channel voice=<n> channel=<n>\n";
        return false;
    }

    if (!ParseIntParameter(argv[2], "voice", voice)) {
        std::cerr << "Invalid voice parameter.\n";
        return false;
    }

    if (!ParseIntParameter(argv[3], "channel", channel)) {
        std::cerr << "Invalid channel parameter.\n";
        return false;
    }

    return true;
}
