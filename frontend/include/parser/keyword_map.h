//
// Created by BY210033 on 2023/4/19.
//

#ifndef QUINT_KEYWORD_MAP_H
#define QUINT_KEYWORD_MAP_H

#include <unordered_map>

namespace quint {

    enum Token :int {
        tok_semicolon = ';',
        tok_lparen= '(',
        tok_rparen = ')',
        tok_lbrace = '{',
        tok_rbrace = '}',
        tok_lbracket = '[',
        tok_rbracket = ']',
        tok_colon = ':',
        tok_comma = ',',
        tok_dot = '.',
        tok_plus = '+',
        tok_minus = '-',
        tok_star = '*',
        tok_slash = '/',
        tok_percent = '%',
        tok_caret = '^',
        tok_vbar = '|',
        tok_amp = '&',
        tok_lt = '<',
        tok_gt = '>',
        tok_eq = '=',
        tok_notequal = '!=',
        tok_le = '<=',
        tok_ge = '>=',
        tok_tilde = '~',
        tok_lshift = '<<',
        tok_rshift = '>>',
        tok_at = '@',
        tok_arrow = '->',
        tok_fatarrow = '=>',
        tok_not = '!',

        tok_eof = -1,
        tok_name = -2,
        tok_number = -3,
        tok_whitespace = -4,
        tok_comment = -5,
        tok_if = -6,
        tok_else = -7,
        tok_while = -8,
        tok_for = -9,
        tok_return = -10,
        tok_break = -11,
        tok_continue = -12,
        tok_module = -13,
        tok_from = -14,
        tok_import = -15,
        tok_as = -16,
        tok_let = -17,
        tok_try = -18,
        tok_catch = -19,
        tok_finally = -20,
        tok_panic = -21,

        /// type
        tok_qubit = -22,
        tok_bit = -23,
        tok_int = -24,
        tok_double = -25,
        tok_bool = -26,
        tok_string = -27,
        tok_uint = -28,
        tok_void = -29,
        tok_map = -30,
        tok_tuple = -31,
        tok_complex = -32,
        tok_circuit = -33,
        tok_unknown = -34,
    };

    static std::unordered_map<std::string_view, Token> name_to_token = {
            {"let",  tok_let},
            {"if",   tok_if},
            {"else", tok_else},
            {"while", tok_while},
            {"for", tok_for},
            {"return", tok_return},
            {"break", tok_break},
            {"continue", tok_continue},
            {"module", tok_module},
            {"from", tok_from},
            {"import", tok_import},
            {"as", tok_as},
            {"try", tok_try},
            {"catch", tok_catch},
            {"finally", tok_finally},
            {"panic", tok_panic},

            {"qubit", tok_qubit},
            {"bit", tok_bit},
            {"int", tok_int},
            {"double", tok_double},
            {"bool", tok_bool},
            {"string", tok_string},
            {"uint", tok_uint},
            {"void", tok_void},
            {"map", tok_map},
            {"tuple", tok_tuple},
            {"complex", tok_complex},
            {"circuit", tok_circuit}
    };

}

#endif //QUINT_KEYWORD_MAP_H
