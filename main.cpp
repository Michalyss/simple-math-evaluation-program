#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <stdexcept>
#include <cctype>
#include <unordered_map>
#include <algorithm>

constexpr int MAX_INPUT_SIZE = 100;

struct Number { double value; };
struct Operator { char symbol; };
struct Parenthesis { char symbol; };
using Token = std::variant<Number, Operator, Parenthesis>;

constexpr int precedence(const char op) {
    switch (op) {
        case '^': return 3;
        case '*': case '/': return 2;
        case '+': case '-': return 1;
        default: return 0;
    }
}

double apply_operator(const double a, const double b, const char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': if (b == 0) throw std::runtime_error("Division by zero"); return a / b;
        case '^': return std::pow(a, b);
        default: throw std::invalid_argument("Unknown operator");
    }
}

std::vector<Token> tokenize(const std::string& expr) {
    std::vector<Token> tokens;
    std::istringstream stream(expr);
    char ch;
    while (stream >> ch) {
        if (std::isdigit(ch) || ch == '.') {
            stream.putback(ch);
            double num;
            stream >> num;
            tokens.emplace_back(Number{num});
        } else if (std::strchr("+-*/^", ch)) {
            tokens.emplace_back(Operator{ch});
        } else if (ch == '(' || ch == ')') {
            tokens.emplace_back(Parenthesis{ch});
        }
    }
    return tokens;
}

std::vector<Token> infix_to_postfix(const std::vector<Token>& tokens) {
    std::vector<Token> postfix;
    std::stack<Token> operators;
    for (const auto& token : tokens) {
        if (std::holds_alternative<Number>(token)) {
            postfix.push_back(token);
        } else if (std::holds_alternative<Operator>(token)) {
            const auto op = std::get<Operator>(token).symbol;
            while (!operators.empty() && std::holds_alternative<Operator>(operators.top())) {
                if (const auto top_op = std::get<Operator>(operators.top()).symbol; precedence(top_op) >= precedence(op)) {
                    postfix.push_back(operators.top());
                    operators.pop();
                } else {
                    break;
                }
            }
            operators.push(token);
        } else if (std::get<Parenthesis>(token).symbol == '(') {
            operators.push(token);
        } else if (std::get<Parenthesis>(token).symbol == ')') {
            while (!operators.empty() && !std::holds_alternative<Parenthesis>(operators.top())) {
                postfix.push_back(operators.top());
                operators.pop();
            }
            operators.pop();
        }
    }
    while (!operators.empty()) {
        postfix.push_back(operators.top());
        operators.pop();
    }
    return postfix;
}

double evaluate_postfix(const std::vector<Token>& tokens) {
    std::stack<double> values;
    for (const auto& token : tokens) {
        if (std::holds_alternative<Number>(token)) {
            values.push(std::get<Number>(token).value);
        } else if (std::holds_alternative<Operator>(token)) {
            const auto op = std::get<Operator>(token).symbol;
            const auto b = values.top(); values.pop();
            const auto a = values.top(); values.pop();
            values.push(apply_operator(a, b, op));
        }
    }
    return values.top();
}

int main() {
    std::string input;
    std::cout << "Welcome to SMEP, the simplest math evaluation program.\n";
    std::cout << "Enter an expression (type help to see the instructions).\n";
    while (true) {
        std::cout << "SIC>";
        std::getline(std::cin, input);

        if (input == "exit") break;
        if (input == "help") {
            std::cout << "-----HELP-----\nSupported symbols: +, -, /, ^, *\nTo exit type 'exit'\nTo see the version type 'version'\n";
            continue;
        }
        if (input == "version") {
            std::cout << "SMEP version 1.0! First release!\n";
            continue;
        }

        try {
            auto tokens = tokenize(input);
            auto postfix = infix_to_postfix(tokens);
            const auto result = evaluate_postfix(postfix);
            std::cout << "Result: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
