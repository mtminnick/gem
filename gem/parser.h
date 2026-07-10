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

    Result run()
    {
        std::string line;
        Result result{ Result::Continue };

        while (true) {
            if (!m_input_redirected) {
                // Print the prompt.
                std::cout << "> ";
                std::cout.flush();
            }

            if (!std::getline(std::cin, line)) {
                // EOF or stdin closed, possibly due to load command.
                if (m_input_redirected) {
                    m_input_redirected = false;
                    result = Result::RedirectInput;
                } else {
                    result = Result::Quit;
                }
                break;
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

            CommandArgs args(tokens.begin() + 1, tokens.end());
            result = it->second(args);
            if (result == Result::RedirectInput) {
                m_input_redirected = true;
                continue;
            }
            if (result == Result::Quit || result == Result::Stop || result == Result::Start) {
                break;
			} else if (result == Result::MaybeStart) {
                if (m_auto_submit) {
                    result = Result::Start;
                    break;
				}
            }
        }

        return result;
    }

private:
    std::unordered_map<std::string, CommandFunc> m_commands;
    bool m_auto_submit = true;
    bool m_input_redirected = false;

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
