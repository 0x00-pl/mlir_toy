//===- Lexer.h - Lexer for the Toy language -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a simple Lexer for the Toy language.
//
//===----------------------------------------------------------------------===//

#ifndef TOY_LEXER_H
#define TOY_LEXER_H

#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>

namespace toy {

/// Structure definition a location in a file.
    struct Location {
        std::shared_ptr<std::string> file; ///< filename.
        int line;                          ///< line number.
        int col;                           ///< column number.
    };

// List of Token returned by the lexer.
    enum Token : int {
        tok_semicolon = ';',
        tok_parenthese_open = '(',
        tok_parenthese_close = ')',
        tok_bracket_open = '{',
        tok_bracket_close = '}',
        tok_sbracket_open = '[',
        tok_sbracket_close = ']',

        tok_eof = -1,

        // commands
        tok_return = -2,
        tok_var = -3,
        tok_def = -4,

        // primary
        tok_identifier = -5,
        tok_number = -6,
    };

    namespace {
        [[maybe_unused]] std::string_view to_string(const Token &tok) {
          switch (tok) {
            case tok_semicolon:
              return "tok_semicolon";
            case tok_parenthese_open:
              return "tok_parenthese_open";
            case tok_parenthese_close:
              return "tok_parenthese_close";
            case tok_bracket_open:
              return "tok_bracket_open";
            case tok_bracket_close:
              return "tok_bracket_close";
            case tok_sbracket_open:
              return "tok_sbracket_open";
            case tok_sbracket_close:
              return "tok_sbracket_close";
            case tok_eof:
              return "tok_eof";
            case tok_return:
              return "tok_return";
            case tok_var:
              return "tok_var";
            case tok_def:
              return "tok_def";
            case tok_identifier:
              return "tok_identifier";
            case tok_number:
              return "tok_number";
          }
        }
    }
/// The Lexer is an abstract base class providing all the facilities that the
/// Parser expects. It goes through the stream one token at a time and keeps
/// track of the location in the file for debugging purposes.
/// It relies on a subclass to provide a `readNextLine()` method. The subclass
/// can proceed by reading the next line from the standard input or from a
/// memory mapped file.
    class Lexer {
    public:
        /// Create a lexer for the given filename. The filename is kept only for
        /// debugging purposes (attaching a location to a Token).
        explicit Lexer(std::string filename)
                : lastLocation(
                {std::make_shared<std::string>(std::move(filename)), 0, 0}) { (void) filename; }

        virtual ~Lexer() = default;

        /// Look at the current token in the stream.
        Token getCurToken() { return curTok; }

        /// Move to the next token in the stream and return it.
        Token getNextToken() { return curTok = getTok(); }

        /// Move to the next token in the stream, asserting on the current token
        /// matching the expectation.
        void consume(Token tok) {
          assert(tok == curTok && "consume Token mismatch expectation");
          getNextToken();
        }

        /// Return the current identifier (prereq: getCurToken() == tok_identifier)
        llvm::StringRef getId() {
          assert(curTok == tok_identifier);
          return identifierStr;
        }

        /// Return the current number (prereq: getCurToken() == tok_number)
        double getValue() {
          assert(curTok == tok_number);
          return numVal;
        }

        /// Return the location for the beginning of the current token.
        Location getLastLocation() { return lastLocation; }

        // Return the current line in the file.
        [[nodiscard]] int getLine() const { return curLineNum; }

        // Return the current column in the file.
        [[nodiscard]] int getCol() const { return curCol; }

    private:
        /// Delegate to a derived class fetching the next line. Returns an empty
        /// string to signal end of file (EOF). Lines are expected to always finish
        /// with "\n"
        virtual llvm::StringRef readNextLine() = 0;

        /// Return the next character from the stream. This manages the buffer for the
        /// current line and request the next line buffer to the derived class as
        /// needed.
        int getNextChar() {
          // The current line buffer should not be empty unless it is the end of file.
          if (curLineBuffer.empty())
            return EOF;
          ++curCol;
          auto next_char = curLineBuffer.front();
          curLineBuffer = curLineBuffer.drop_front();
          if (curLineBuffer.empty())
            curLineBuffer = readNextLine();
          if (next_char == '\n') {
            ++curLineNum;
            curCol = 0;
          }
          return next_char;
        }

        ///  Return the next token from standard input.
        Token getTok() {
          // Skip any whitespace.
          while (isspace(last_char))
            last_char = Token(getNextChar());

          // Save the current location before reading the token characters.
          lastLocation.line = curLineNum;
          lastLocation.col = curCol;

          // Identifier: [a-zA-Z][a-zA-Z0-9_]*
          if (isalpha(last_char)) {
            identifierStr = (char) last_char;
            while (isalnum((last_char = Token(getNextChar()))) || last_char == '_')
              identifierStr += (char) last_char;

            if (identifierStr == "return")
              return tok_return;
            if (identifierStr == "def")
              return tok_def;
            if (identifierStr == "var")
              return tok_var;
            return tok_identifier;
          }

          // Number: [0-9.]+
          if (isdigit(last_char) || last_char == '.') {
            std::string numStr;
            do {
              numStr += std::to_string(last_char);
              last_char = Token(getNextChar());
            } while (isdigit(last_char) || last_char == '.');

            numVal = strtod(numStr.c_str(), nullptr);
            return tok_number;
          }

          if (last_char == '#') {
            // Comment until end of line.
            do {
              last_char = Token(getNextChar());
            } while (last_char != EOF && last_char != '\n' && last_char != '\r');

            if (last_char != EOF)
              return getTok();
          }

          // Check for end of file.  Don't eat the EOF.
          if (last_char == EOF)
            return tok_eof;

          // Otherwise, just return the character as its ascii value.
          auto thisChar = Token(last_char);
          last_char = Token(getNextChar());
          return thisChar;
        }

        /// The last token read from the input.
        Token curTok = tok_eof;

        /// Location for `curTok`.
        Location lastLocation;

        /// If the current Token is an identifier, this string contains the value.
        std::string identifierStr;

        /// If the current Token is a number, this contains the value.
        double numVal = 0;

        /// The last value returned by getNextChar(). We need to keep it around as we
        /// always need to read ahead one character to decide when to end a token and
        /// we can't put it back in the stream after reading from it.
        Token last_char = Token(' ');

        /// Keep track of the current line number in the input stream
        int curLineNum = 0;

        /// Keep track of the current column number in the input stream
        int curCol = 0;

        /// Buffer supplied by the derived class on calls to `readNextLine()`
        llvm::StringRef curLineBuffer = "\n";
    };

/// A lexer implementation operating on a buffer in memory.
    class LexerBuffer final : public Lexer {
    public:
        LexerBuffer(const char *begin, const char *end, std::string filename)
                : Lexer(std::move(filename)), current(begin), end(end) { (void) filename; }

    private:
        /// Provide one line at a time to the Lexer, return an empty string when
        /// reaching the end of the buffer.
        llvm::StringRef readNextLine() override {
          auto *begin = current;
          while (current <= end && *current && *current != '\n')
            ++current;
          if (current <= end && *current)
            ++current;
          llvm::StringRef result{begin, static_cast<size_t>(current - begin)};
          return result;
        }

        const char *current, *end;
    };
} // namespace toy

#endif // TOY_LEXER_H
