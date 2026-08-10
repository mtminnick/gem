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

#include <cctype>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

class CommandParser {
public:
    enum class Result {
        Continue,
        Stop,
        Start,
        Quit,
        MaybeStart,
        RedirectInput
    };

    using CommandArgs = std::vector<std::string>;
    using CommandFunc = std::function<Result(const CommandArgs& args)>;

    void registerCommand(const std::string& name, CommandFunc func)
    {
        m_commands[name] = func;
    }

    bool GetAutoSubmit() const
    {
        return m_auto_submit;
    }

    Result run()
    {
        std::string line;
        Result result{ Result::Continue };

        while (true) {
            if (m_input_redirected == 0) {
                // Print the prompt.
                std::cout << "> ";
                std::cout.flush();
            }

            if (!std::getline(std::cin, line)) {
                // EOF or stdin closed, possibly due to load command.
                if (m_input_redirected == 0) {
                    std::cerr << "Error reading input. Exiting.\n";
                    return Result::Quit;
                } else {
                    --m_input_redirected;
                    result = Result::RedirectInput;
                }
                break;
            }

            if (!line.empty() && line[0] == '#') {
                continue;
            }

            CommandArgs tokens;
            if (!tokenize(line, tokens)) {
                continue;
            }

            if (tokens.empty()) {
                continue;
            }

            const std::string& command = tokens[0];

            if (command == "autosubmit") {
				m_auto_submit = !m_auto_submit;
				std::cout << "autosubmit is now " << (m_auto_submit ? "on" : "off") << "\n";
                continue;
			}

            auto it = m_commands.find(command);
            if (it == m_commands.end()) {
                std::cerr << "Unknown command: " << command << "\n";
                continue;
            }

            CommandArgs args(tokens.begin(), tokens.end());
            result = it->second(args);
            if (result == Result::RedirectInput) {
                ++m_input_redirected;
                continue;
            }
            if (result == Result::Quit || result == Result::Stop || result == Result::Start) {
                break;
			} else if (result == Result::MaybeStart) {
                if (m_auto_submit && m_input_redirected == 0) {
                    result = Result::Start;
                    break;
				}
            }
        }

        return result;
    }

private:
    std::unordered_map<std::string, CommandFunc> m_commands;
    bool m_auto_submit = false;
    size_t m_input_redirected = 0;

    static bool tokenize(const std::string& line, CommandArgs& tokens)
    {
        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            toLower(token);

            if (!isValidToken(token)) {
                std::cerr << "Invalid token: " << token << "\n";
                return false;
            }

            tokens.push_back(token);
        }

        return true;
    }

    static void toLower(std::string& s)
    {
        for (char& ch : s) {
            ch = static_cast<char>(
                std::tolower(static_cast<unsigned char>(ch)));
        }
    }

    static bool isValidToken(const std::string& token)
    {
        if (token.empty()) {
            return false;
        }

        for (unsigned char ch : token) {
            if (ch < 0x21 || ch > 0x7e) {
                return false;
            }
        }

        return true;
    }
};
