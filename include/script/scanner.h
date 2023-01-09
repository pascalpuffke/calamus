#pragma once

#include <util/types.h>

namespace calamus::script {

enum class TokenType {
    // Single-character tokens.
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Colon,
    Slash,
    Star,
    // One or two character tokens.
    Bang,
    BangEqual,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    // Literals.
    Identifier,
    String,
    Number,
    // Keywords.
    And,
    Class,
    Else,
    False,
    For,
    Fun,
    If,
    Import,
    Nil,
    Or,
    Print,
    Return,
    Super,
    This,
    True,
    Var,
    While,

    Error,
    Eof
};

struct Token {
    TokenType type;
    const char* start;
    i32 length;
    i32 line;
};

void init_scanner(const char* source);
Token scan_token();

}
