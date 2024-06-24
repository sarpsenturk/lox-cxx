#include "lexer.h"

#include "error.h"

#include <cctype>
#include <unordered_map>

namespace lox
{
    using namespace std::string_view_literals;
    const std::unordered_map<std::string_view, TokenType> Lexer::keywords = std::unordered_map{
        std::make_pair("and"sv, TokenType::And),
        std::make_pair("or"sv, TokenType::Or),
        std::make_pair("true"sv, TokenType::True),
        std::make_pair("false"sv, TokenType::False),
        std::make_pair("var"sv, TokenType::Var),
        std::make_pair("fun"sv, TokenType::Fun),
        std::make_pair("class"sv, TokenType::Class),
        std::make_pair("if"sv, TokenType::If),
        std::make_pair("else"sv, TokenType::Else),
        std::make_pair("for"sv, TokenType::For),
        std::make_pair("while"sv, TokenType::While),
        std::make_pair("super"sv, TokenType::Super),
        std::make_pair("nil"sv, TokenType::Nil),
    };

    Lexer::Lexer(std::string_view source)
        : source_(source)
    {
    }

    Token Lexer::tokenize_next()
    {
        while (!is_eof()) {
            start_position_ = current_position_;
            start_location_ = current_location_;

            const char c = consume();
            switch (c) {
                case '\n':
                    current_location_.new_line();
                    [[fallthrough]];
                case ' ':
                case '\r':
                case '\t':
                    continue;
                case '+':
                    return make_token(TokenType::Plus);
                case '-':
                    return make_token(TokenType::Minus);
                case '*':
                    return make_token(TokenType::Star);
                case '/':
                    if (consume_expected('/')) {
                        while (!is_eof() && peek() != '\n') {
                            consume();
                        }
                        continue;
                    }
                    return make_token(TokenType::Slash);
                case '(':
                    return make_token(TokenType::LeftParen);
                case ')':
                    return make_token(TokenType::RightParen);
                case '{':
                    return make_token(TokenType::LeftBrace);
                case '}':
                    return make_token(TokenType::RightBrace);
                case ';':
                    return make_token(TokenType::Semicolon);
                case ',':
                    return make_token(TokenType::Comma);
                case '.':
                    return make_token(TokenType::Dot);
                case '!':
                    if (consume_expected('=')) {
                        return make_token(TokenType::BangEqual);
                    }
                    return make_token(TokenType::Bang);
                case '=':
                    if (consume_expected('=')) {
                        return make_token(TokenType::EqualEqual);
                    }
                    return make_token(TokenType::Equal);
                case '>':
                    if (consume_expected('=')) {
                        return make_token(TokenType::GreaterEqual);
                    }
                    return make_token(TokenType::Greater);
                case '<':
                    if (consume_expected('=')) {
                        return make_token(TokenType::LessEqual);
                    }
                    return make_token(TokenType::Less);
                case '"':
                    return make_string_literal();
                default:
                    if (std::isdigit(c)) {
                        return make_number_literal();
                    }
                    if (std::isalpha(c) || c == '_') {
                        return make_keyword_or_identifier();
                    }
                    throw_error("Unexpected character");
            }
        }
        return make_token(TokenType::Eof);
    }

    std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;
        do {
            auto token = tokenize_next();
            if (token.type == TokenType::Eof) {
                break;
            }
            tokens.push_back(token);
        } while (true);
        return tokens;
    }

    bool Lexer::is_eof() const
    {
        return current_position_ >= source_.length();
    }

    char Lexer::consume()
    {
        if (is_eof()) {
            return '\0';
        }
        current_location_.column += 1;
        return source_[current_position_++];
    }

    char Lexer::peek() const
    {
        if (is_eof()) {
            return '\0';
        }
        return source_[current_position_];
    }

    char Lexer::peek_next() const
    {
        if (current_position_ + 1 >= source_.length()) {
            return '\0';
        }
        return source_[current_position_ + 1];
    }

    bool Lexer::consume_expected(char expected)
    {
        if (peek() == expected) {
            consume();
            return true;
        }
        return false;
    }

    std::string_view Lexer::current_lexeme() const
    {
        return source_.substr(start_position_, current_position_ - start_position_);
    }

    Token Lexer::make_token(TokenType type) const
    {
        return {.type = type, .lexeme = current_lexeme(), .location = start_location_};
    }

    Token Lexer::make_string_literal()
    {
        while (peek() != '"' && !is_eof()) {
            if (peek() == '\n') {
                current_location_.new_line();
            }
            consume();
        }

        if (is_eof()) {
            throw_error("Unterminated string");
        }

        // Consume the terminating "
        consume();

        return make_token(TokenType::String);
    }

    Token Lexer::make_number_literal()
    {
        while (std::isdigit(peek())) {
            consume();
        }

        if (peek() == '.' && std::isdigit(peek_next())) {
            consume();
            while (std::isdigit(peek())) {
                consume();
            }
        }

        return make_token(TokenType::Number);
    }

    Token Lexer::make_keyword_or_identifier()
    {
        while (std::isalnum(peek()) || peek() == '_') {
            consume();
        }
        if (auto iter = keywords.find(current_lexeme()); iter != keywords.end()) {
            return make_token(iter->second);
        }
        return make_token(TokenType::Identifier);
    }

    void Lexer::throw_error(const char* message) const
    {
        throw LoxError(message, current_location_);
    }
} // namespace lox
