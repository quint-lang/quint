//
// Created by BY210033 on 2023/4/18.
//

#ifndef QUINT_LEXER_H
#define QUINT_LEXER_H

#include "llvm/ADT/StringRef.h"
#include "parser/keyword_map.h"
#include "llvm/ADT/StringExtras.h"

#include <memory>
#include <string>
#include <vector>
#include <cctype>
#include <iostream>

namespace quint {

    struct Location {
        std::shared_ptr<std::string> file;
        std::string rep;
        int line;
        int col;
    };

    enum TokenState {
        state_start,
        state_name,
        state_number,
        state_plus_or_other,
        state_minus_or_other,
        state_star_or_other,
        state_mod_or_other,
        state_div_or_other,
        state_div_or_comment,
        state_and_or_other,
        state_or_or_other,
        state_xor_or_other,
        state_not_or_other,
        state_lt_or_other,
        state_gt_or_other,
        state_eq_or_other,
    };

    class Lexer {
    public:
        Lexer(std::string filename)
            : last_location({ std::make_shared<std::string>(std::move(filename)), "", 0, 0 }) {}
        virtual ~Lexer() = default;

        Token getCurToken() { return cur_tok; }

        Token getNextToken() { return cur_tok = get_tok(); }

        llvm::StringRef getIdentifier() {
            assert(cur_tok == tok_name);
            return identifier;
        }

        void consume(Token tok) {
//            assert(tok == cur_tok && "consume Token mismatch expectation");
            if (tok != cur_tok) {
                std::cerr << "Parse error (line: "<< cur_line << ", col: " << cur_column << "): expect " <<
                (char)tok << "but get " << (char)tok;
                assert(0);
            }
            getNextToken();
        }

        void consumeParenPair()
        {
            assert(cur_tok == tok_lparen && "consume lparen mismatch");
            assert(getNextToken() == tok_rparen && "consume rparen mismatch");
            getNextToken();
        }

        void consumeBracketPair()
        {
            assert(cur_tok == tok_lbracket && "consume lbracket mismatch");
            assert(getNextToken() == tok_rbracket && "consume rbracket mismatch");
            getNextToken();
        }

        double getValue() {
            assert(cur_tok == tok_number);
            return num_value;
        }

        Location getLocation() {
            return last_location;
        }

        int getLine() {
            return cur_line;
        }

        int getColumn() {
            return cur_column;
        }

        Location getLastLocation()
        {
            last_location.line = cur_line;
            last_location.col = cur_column;
            return last_location;
        }

    private:
        virtual llvm::StringRef readNextLine() = 0;

        int getNextChar() {
            if (cur_line_buffer.empty()) {
                return EOF;
            }
            ++cur_column;
            auto next_char = cur_line_buffer.front();
            cur_line_buffer = cur_line_buffer.drop_front();
            if (cur_line_buffer.empty()) {
                cur_line_buffer = readNextLine();
            }
            if (next_char == '\n') {
                ++cur_line;
                cur_column = 0;
            }
            return next_char;
        }

        int peekChar(int distance) {
            assert(distance >= 0);
            if (cur_column - 1 + distance < cur_line_buffer.size()) {
                return cur_line_buffer[cur_column-1+distance];
            } else {
                return '\0';
            }
        }

        void skip_whitespace() {
            while (isspace(last_char)) {
                last_char = Token(getNextChar());
            }
        }

        Token get_tok() {
            skip_whitespace();

            last_location.line = cur_line;
            last_location.col = cur_column;

            if (last_char == '+') {
                if (peekChar(1) == '=') {
                    last_char = Token(getNextChar());
                    return tok_aassign;
                } else {
                    return tok_plus;
                }
            }

            if (last_char == '-') {
                auto advance = peekChar(1);
                if (advance == '=') {
                    last_char = Token(getNextChar());
                    return tok_sassign;
                } else if (advance == '>') {
                    last_char = Token(getNextChar());
                    return tok_arrow;
                } else {
                    return tok_minus;
                }
            }

            if (last_char == '*') {
                auto advance = peekChar(1);
                if (advance == '=') {
                    last_char = Token(getNextChar());
                    return tok_massign;
                } else if (advance == '*') {
                    last_char = Token(getNextChar());
                    return tok_pow;
                }
                return tok_star;
            }

            if (last_char == '/') {
                if (peekChar(1) == '=') {
                    last_char = Token(getNextChar());
                    return tok_dassign;
                }
                return tok_slash;
            }

            if (last_char == '^') {
                if (peekChar(1) == '=') {
                    last_char = Token(getNextChar());
                    return tok_cassign;
                }
                return tok_caret;
            }

            if (last_char == '%') {
                if (peekChar(1) == '=') {
                    last_char = Token(getNextChar());
                    return tok_passign;
                }
                return tok_percent;
            }

            if (last_char == '|') {
                auto advance = peekChar(1);
                if (advance == '=') {
                    last_char = Token(getNextChar());
                    return tok_vassign;
                }
                if (advance == '|') {
                    last_char = Token(getNextChar());
                    return tok_lor;
                }
                return tok_vbar;
            }

            if (last_char == '&') {
                auto advance = peekChar(1);
                if (advance == '=') {
                    last_char = Token(getNextChar());
                    return tok_augassign;
                }
                if (advance == '&') {
                    last_char = Token(getNextChar());
                    return tok_land;
                }
                return tok_amp;
            }

            if (last_char == '(')
                return tok_lparen;
            if (last_char == ')')
                return tok_rparen;
            if (last_char == '{')
                return tok_lbrace;
            if (last_char == '}')
                return tok_rbrace;
            if (last_char == ']')
                return tok_rbracket;
            if (last_char == '[')
                return tok_lbracket;
            if (last_char == ':')
                return tok_colon;
            if (last_char == ',')
                return tok_comma;
            if (last_char == '.')
                return tok_dot;

            if (last_char == '~')
                return tok_tilde;
            if (last_char == '@')
                return tok_at;

            if (last_char == '<') {
                auto advance = peekChar(1);
                if (advance == '<') {
                    last_char = Token(getNextChar());
                    if (peekChar(2) == '=') {
                        last_char = Token(getNextChar());
                        return tok_lassign;
                    }
                    return tok_lshift;
                } else if (advance == '=') {
                    last_char = Token(getNextChar());
                    return tok_le;
                }
                return tok_lt;
            }

            if (last_char == '>') {
                auto advance = peekChar(1);
                if (advance == '>') {
                    last_char = Token(getNextChar());
                    if (peekChar(2) == '=') {
                        last_char = Token(getNextChar());
                        return tok_rassign;
                    }
                    return tok_rshift;
                } else if (advance == '=') {
                    last_char = Token(getNextChar());
                    return tok_ge;
                }
                return tok_gt;
            }

            if (last_char == '=') {
                if (peekChar(1) == '=') {
                    last_char = Token(getNextChar());
                    return tok_eq;
                }
                return tok_assign;
            }

            if (last_char == '!') {
                if (peekChar(1) == '=') {
                    last_char = Token(getNextChar());
                    return tok_notequal;
                }
                return tok_not;
            }


            if (isalpha(last_char))
            {
                identifier = (char)last_char;
                while (isalnum((last_char = Token(getNextChar()))) || last_char == '_')
                    identifier += (char)last_char;
                if (name_to_token.find(identifier) != name_to_token.end())
                    return name_to_token[identifier];
                return tok_name;
            }

            if (isdigit(last_char) || last_char == '.')
            {
                std::string numStr;
                do {
                    numStr += last_char;
                    last_char = Token(getNextChar());
                } while (isdigit(last_char) || last_char == '.');

                num_value = strtod(numStr.c_str(), nullptr);
                return tok_number;
            }

            if (last_char == '\"')
            {
                identifier = "";
                last_char = Token(getNextChar());
                while (last_char != '\"' && last_char != EOF) {
                    identifier += (char)last_char;
                    last_char = Token(getNextChar());
                }

                return tok_chars;
            }

            if (last_char == '#')
            {
                do
                {
                    last_char = Token(getNextToken());
                } while (last_char != EOF && last_char != '\r' && last_char != '\n');

                if (last_char != EOF)
                    return get_tok();
            }

            if (last_char == EOF)
                return tok_eof;

            last_char = Token(getNextToken());
            return tok_unknown;
        }

        Location last_location;
        Token cur_tok = tok_eof;
        std::string identifier;
        double num_value = 0;
        Token last_char = Token(' ');
        int cur_line = 0;
        int cur_column = 0;
        llvm::StringRef cur_line_buffer = "\n";
    };

    class LexerBuffer final : public Lexer {
    public:
        LexerBuffer(const char *begin, const char *end, std::string filename)
            : Lexer(std::move(filename)), current(begin), end(end) {}

    private:
        llvm::StringRef readNextLine() override {
            auto *begin = current;
            while (current <= end && *current && *current != '\n') {
                ++current;
            }
            if (current <= end && *current)
                ++current;
            llvm::StringRef result{begin, static_cast<size_t>(current - begin)};
            return result;
        }

        const char *current, *end;
    };

    class LexerFile final : public Lexer {
    public:
        LexerFile(std::string filename)
            : Lexer(std::move(filename)), position(0), state(state_start) {
            char * code_buff;
            FILE *fp = fopen(filename.c_str(), "r");
            if (fp) {
                fseek(fp, 0, SEEK_END);
                size_t size = ftell(fp);
                code_buff = (char *) malloc(size + 1);
                code_buff[size] = '\0';
                fseek(fp, 0, SEEK_SET);
//                buffer.resize(size);
                fread(code_buff, size, 1, fp);
                fclose(fp);
            } else {
                printf("%s open failed\n", filename.c_str());
                assert(0);
            }
            source = code_buff;
        }

        Token next_token() {
            Token token;
            std::string buffer;

            while (position < source.length()) {
                char current_char = source[position];

                switch (state) {
                    case state_start:
                        if (std::isalpha(current_char)) {
                            state = state_name;
                            buffer.push_back(current_char);
                        } else if (std::isdigit(current_char)) {
                            state = state_number;
                            buffer.push_back(current_char);
                        } else if (current_char == '(') {
                            token = tok_lparen;
                            position++;
                            return token;
                        } else if (current_char == ')') {
                            token = tok_rparen;
                            position++;
                            return token;
                        } else if (current_char == '{') {
                            token = tok_lbrace;
                            position++;
                            return token;
                        } else if (current_char == '}') {
                            token = tok_rbrace;
                            position++;
                            return token;
                        } else if (current_char == '[') {
                            token = tok_lbracket;
                            position++;
                            return token;
                        } else if (current_char == ']') {
                            token = tok_rbracket;
                            position++;
                            return token;
                        } else if (current_char == ',') {
                            token = tok_comma;
                            position++;
                            return token;
                        } else if (current_char == ';') {
                            token = tok_semicolon;
                            position++;
                            return token;
                        } else if (current_char == ':') {
                            token = tok_colon;
                            position++;
                            return token;
                        } else if (current_char == '.') {
                            token = tok_dot;
                            position++;
                            return token;
                        } else if (current_char == '~') {
                            token = tok_tilde;
                            position++;
                            return token;
                        } else if (current_char == '+') {
                            state = state_plus_or_other;
                        } else if (current_char == '-') {
                            state = state_minus_or_other;
                        } else if (current_char == '*') {
                            state = state_star_or_other;
                        } else if (current_char == '/') {
                            state = state_div_or_other;
                        } else if (current_char == '%') {
                            state = state_mod_or_other;
                        } else if (current_char == '&') {
                            state = state_and_or_other;
                        } else if (current_char == '|') {
                            state = state_or_or_other;
                        } else if (current_char == '^') {
                            state = state_xor_or_other;
                        } else if (current_char == '!') {
                            state = state_not_or_other;
                        } else if (current_char == '=') {
                            state = state_eq_or_other;
                        } else if (current_char == '<') {
                            state = state_lt_or_other;
                        } else if (current_char == '>') {
                            state = state_gt_or_other;
                        } else if (std::isspace(current_char)) {
                            position++;
                            continue;
                        } else if (current_char == '\0' || current_char == EOF) {
                            token = tok_eof;
                            return token;
                        } else {
                            token = tok_unknown;
                            position++;
                            return token;
                        }
                        position++;
                        break;
                    case state_name:
                        if (std::isalnum(current_char)) {
                            buffer.push_back(current_char);
                        } else {
                            token = tok_name;
//                            token.name = buffer;
                            state = state_start;
                            return token;
                        }
                        position++;
                        break;
                    case state_number:
                    default:
                        break;
                }
            }
            return token;
        }

    private:
        std::string_view source;
        size_t position;
        TokenState state;
    };

}

#endif //QUINT_LEXER_H
