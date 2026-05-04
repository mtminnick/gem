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
        Quit
    };

    using CommandArgs = std::vector<std::string>;
    using CommandFunc = std::function<int(const CommandArgs& args)>;

    void registerCommand(const std::string& name, CommandFunc func)
    {
        commands_[name] = func;
    }

    Result run()
    {
        std::string line;
        Result result{ Result::Continue };

        while (true) {
            std::cout << "> ";
            std::cout.flush();

            if (!std::getline(std::cin, line)) {
                break;  // EOF or stdin closed
            }

            CommandArgs tokens;
            if (!tokenize(line, tokens)) {
                continue;
            }

            if (tokens.empty()) {
                continue;
            }

            const std::string& command = tokens[0];

            if (command == "quit" || command == "exit") {
                result = Result::Quit;
                break;
            }
            if (command == "stop") {
                result = Result::Stop;
                break;
            }
            if (command == "start" || command == "submit") {
                result = Result::Start;
                break;
            }

            auto it = commands_.find(command);
            if (it == commands_.end()) {
                std::cerr << "Unknown command: " << command << "\n";
                continue;
            }

            CommandArgs args(tokens.begin() + 1, tokens.end());
            it->second(args);
        }

        return result;
    }

private:
    std::unordered_map<std::string, CommandFunc> commands_;

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
