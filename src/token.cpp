#include "token.h"

namespace lox
{
    const char* format_as(TokenType type)
    {
        switch (type) {
            case TokenType::Eof:
                return "EOF";
            case TokenType::Plus:
                return "Plus";
            case TokenType::Minus:
                return "Minus";
            case TokenType::Star:
                return "Star";
            case TokenType::Slash:
                return "Slash";
            case TokenType::LeftParen:
                return "LeftParen";
            case TokenType::RightParen:
                return "RightParen";
            case TokenType::LeftBrace:
                return "LeftBrace";
            case TokenType::RightBrace:
                return "RightBrace";
            case TokenType::Semicolon:
                return "Semicolon";
            case TokenType::Comma:
                return "Comma";
            case TokenType::Dot:
                return "Dot";
            case TokenType::Bang:
                return "Bang";
            case TokenType::BangEqual:
                return "BangEqual";
            case TokenType::Equal:
                return "Equal";
            case TokenType::EqualEqual:
                return "EqualEqual";
            case TokenType::Greater:
                return "Greater";
            case TokenType::GreaterEqual:
                return "GreaterEqual";
            case TokenType::Less:
                return "Less";
            case TokenType::LessEqual:
                return "LessEqual";
            case TokenType::Identifier:
                return "Identifier";
            case TokenType::String:
                return "String";
            case TokenType::Number:
                return "Number";
            case TokenType::And:
                return "And";
            case TokenType::Or:
                return "Or";
            case TokenType::True:
                return "True";
            case TokenType::False:
                return "False";
            case TokenType::Var:
                return "Var";
            case TokenType::Fun:
                return "Fun";
            case TokenType::Class:
                return "Class";
            case TokenType::If:
                return "If";
            case TokenType::Else:
                return "Else";
            case TokenType::For:
                return "For";
            case TokenType::While:
                return "While";
            case TokenType::Super:
                return "Super";
            case TokenType::Nil:
                return "Nil";
        }
    }
} // namespace lox
