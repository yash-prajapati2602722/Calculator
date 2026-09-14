#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
constexpr double pi = 3.14159265358979323846;
constexpr double euler = 2.71828182845904523536;

class Parser {
public:
    Parser(const std::string& source, bool degrees)
        : input(source), cursor(0), useDegrees(degrees) {}

    double parse() {
        double value = expression();
        spaces();
        if (cursor == 0 || cursor != input.size()) {
            throw std::runtime_error("invalid expression");
        }
        return value;
    }

private:
    const std::string& input;
    std::size_t cursor;
    bool useDegrees;

    void spaces() {
        while (cursor < input.size() && std::isspace(static_cast<unsigned char>(input[cursor]))) {
            ++cursor;
        }
    }

    double expression() {
        double value = term();
        while (true) {
            spaces();
            if (cursor < input.size() && input[cursor] == '+') {
                ++cursor;
                value += term();
            } else if (cursor < input.size() && input[cursor] == '-') {
                ++cursor;
                value -= term();
            } else {
                return value;
            }
        }
    }

    double term() {
        double value = power();
        while (true) {
            spaces();
            if (cursor < input.size() && input[cursor] == '*') {
                ++cursor;
                value *= power();
            } else if (cursor < input.size() && input[cursor] == '/') {
                ++cursor;
                const double divisor = power();
                if (divisor == 0.0) throw std::runtime_error("division by zero");
                value /= divisor;
            } else {
                return value;
            }
        }
    }

    double power() {
        spaces();
        if (cursor < input.size() && input[cursor] == '+') {
            ++cursor;
            return power();
        }
        if (cursor < input.size() && input[cursor] == '-') {
            ++cursor;
            return -power();
        }
        double value = primary();
        spaces();
        if (cursor < input.size() && input[cursor] == '^') {
            ++cursor;
            value = std::pow(value, power());
        }
        return value;
    }

    double primary() {
        spaces();
        if (cursor < input.size() && input[cursor] == '(') {
            ++cursor;
            double value = expression();
            spaces();
            if (cursor >= input.size() || input[cursor++] != ')') throw std::runtime_error("missing )");
            return value;
        }

        char* end = nullptr;
        const char* start = input.c_str() + cursor;
        const double number = std::strtod(start, &end);
        if (end != start) {
            cursor += static_cast<std::size_t>(end - start);
            return number;
        }

        const std::size_t begin = cursor;
        while (cursor < input.size() && std::isalpha(static_cast<unsigned char>(input[cursor]))) ++cursor;
        const std::string name = input.substr(begin, cursor - begin);
        if (name == "pi") return pi;
        if (name == "e") return euler;
        spaces();
        if (cursor >= input.size() || input[cursor++] != '(') throw std::runtime_error("unknown symbol");
        const double argument = expression();
        spaces();
        if (cursor >= input.size() || input[cursor++] != ')') throw std::runtime_error("missing )");
        return function(name, argument);
    }

    double function(const std::string& name, double value) const {
        const double radians = useDegrees ? value * pi / 180.0 : value;
        if (name == "sin") return std::sin(radians);
        if (name == "cos") return std::cos(radians);
        if (name == "tan") return std::tan(radians);
        if (name == "sqrt") return std::sqrt(value);
        if (name == "log") return std::log10(value);
        if (name == "ln") return std::log(value);
        if (name == "abs") return std::fabs(value);
        if (name == "asin") return useDegrees ? std::asin(value) * 180.0 / pi : std::asin(value);
        if (name == "acos") return useDegrees ? std::acos(value) * 180.0 / pi : std::acos(value);
        if (name == "atan") return useDegrees ? std::atan(value) * 180.0 / pi : std::atan(value);
        throw std::runtime_error("unknown function");
    }
};
}

int main() {
    bool degrees = true;
    std::string input;
    std::cout << "FX-991CW-style C++ calculator\nType help for commands.\n";
    while (std::cout << '[' << (degrees ? "DEG" : "RAD") << "] > " && std::getline(std::cin, input)) {
        if (input == "quit" || input == "exit") break;
        if (input == "help") {
            std::cout << "Expressions: + - * / ^ and parentheses\nFunctions: sin cos tan asin acos atan sqrt log ln abs\nCommands: mode deg, mode rad, quit\n";
            continue;
        }
        if (input == "mode deg") { degrees = true; continue; }
        if (input == "mode rad") { degrees = false; continue; }
        try {
            const double answer = Parser(input, degrees).parse();
            if (!std::isfinite(answer)) throw std::runtime_error("undefined result");
            std::cout << "= " << std::setprecision(12) << answer << '\n';
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
}