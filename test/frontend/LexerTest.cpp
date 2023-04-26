#include <iostream>
#include <string>
#include <cctype>

enum TokenType {
    TOKEN_INVALID,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIVIDE,
    TOKEN_COMMENT,
    TOKEN_END
};

struct Token {
    TokenType type;
    std::string value;
};

enum State {
    STATE_START,
    STATE_NUMBER,
    STATE_DIVIDE_OR_COMMENT,
    STATE_COMMENT
};

class Lexer {
public:
    Lexer(const std::string &input) : input(input), position(0), state(STATE_START) {}

    Token next_token() {
        Token token;
        std::string buffer;

        while (position < input.length()) {
            char current_char = input[position];

            switch (state) {
                case STATE_START:
                    if (std::isdigit(current_char)) {
                        state = STATE_NUMBER;
                        buffer.push_back(current_char);
                    } else if (current_char == '+') {
                        token.type = TOKEN_PLUS;
                        token.value = "+";
                        position++;
                        return token;
                    } else if (current_char == '-') {
                        token.type = TOKEN_MINUS;
                        token.value = "-";
                        position++;
                        return token;
                    } else if (current_char == '/') {
                        state = STATE_DIVIDE_OR_COMMENT;
                    } else {
                        if (std::isspace(current_char)) {
                            position++;
                            continue;
                        }
                        token.type = TOKEN_INVALID;
                        token.value = std::string(1, current_char);
                        position++;
                        return token;
                    }
                    position++;
                    break;

                case STATE_NUMBER:
                    if (!std::isdigit(current_char)) {
                        token.type = TOKEN_NUMBER;
                        token.value = buffer;
                        buffer.clear();
                        state = STATE_START;
                        return token;
                    } else {
                        buffer.push_back(current_char);
                        position++;
                    }
                    break;

                case STATE_DIVIDE_OR_COMMENT:
                    if (current_char == '/') {
                        state = STATE_COMMENT;
                    } else {
                        token.type = TOKEN_DIVIDE;
                        token.value = "/";
                        state = STATE_START;
                        return token;
                    }
                    position++;
                    break;

                case STATE_COMMENT:
                    if (current_char == '\n') {
                        state = STATE_START;
                    }
                    position++;
                    break;
            }
        }

        if (state == STATE_NUMBER) {
            token.type = TOKEN_NUMBER;
            token.value = buffer;
        } else {
            token.type = TOKEN_END;
            token.value = "";
        }

        return token;
    }

private:
    std::string input;
    size_t position;
    State state;
};

int main() {
    std::string input = "12 + 34 // This is a comment\n - 5 / 2";
    Lexer lexer(input);

    Token token;
    while ((token = lexer.next_token()).type != TOKEN_END) {
        std::cout << "Token: " << token.value << std::endl;
    }

    return 0;
}
