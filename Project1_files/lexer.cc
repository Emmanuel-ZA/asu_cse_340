/*
 * Copyright (C) Rida Bazzi
 *
 * Do not share this file with anyone
 *
 */

// Project done By:Emmanuel Zelaya-Armenta
#include <cctype>
#include <cstddef>
#include <iostream>
#include <istream>
#include <string>
#include <vector>

#include "inputbuf.h"
#include "lexer.h"

using namespace std;

string reserved[] = {
    "END_OF_FILE", "IF",     "WHILE",    "DO",      "THEN",      "PRINT",     "PLUS",  "MINUS",
    "DIV",         "MULT",   "EQUAL",    "COLON",   "COMMA",     "SEMICOLON", "LBRAC", "RBRAC",
    "LPAREN",      "RPAREN", "NOTEQUAL", "GREATER", "LESS",      "LTEQ",      "GTEQ",  "DOT",
    "NUM",         "ID",     "ERROR",    "REALNUM", "BASE08NUM", "BASE16NUM" // TODO: Add labels for new token types
                                                                             // here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = {"IF", "WHILE", "DO", "THEN", "PRINT"};

void Token::Print()
{
    cout << "{" << this->lexeme << " , " << reserved[(int)this->token_type] << " , " << this->line_no << "}\n";
}

bool isdigit8(char c) { return (unsigned)c - '0' < 8; }
bool isdigit16(char c) { return ((unsigned)c - '0' < 10) || ((unsigned)c - 'A' < 6); }

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}

// THING TO NOTE! getchar gets the next char from the buffer first and if buffer is empty then grab from input.
// unget char - put onto the the buffer, when getchar is called it would grab from the buffer first.
bool IsNum(string temp)
{
    for (char c : temp) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool IsRealNum(string temp)
{
    std::size_t s = temp.find(".");
    if (s == std::string::npos || !isdigit(temp[s + 1]) || !isdigit(temp[s - 1]))
        return false;

    return true;
}

bool IsBase16Num(string temp)
{
    std::size_t s = temp.find("x16");
    if (s == std::string::npos)
        return false;
    for (std::size_t i = 0; i < s; i++) {
        if (!isdigit16(temp[i])) {
            return false;
        }
    }
    return true;
}

bool IsBase8Num(string temp)
{
    std::size_t s = temp.find("x08");
    if (s == std::string::npos)
        return false;
    for (std::size_t i = 0; i < s; i++) {
        if (!isdigit8(temp[i])) {
            return false;
        }
    }
    if (isalnum(temp[s + 3]) || (temp[s + 3] == '.'))
        return false;

    return true;
}

//HOW THIS WORKS. take string input and put it into a temporary string, then check all token types. If non match remove a char and keep going until error or something.
Token LexicalAnalyzer::ScanNumber()
{
    char c;
    string temp = "";

    input.GetChar(c);
    if (isdigit(c)) {
        if (c == '0') {
            tmp.lexeme = "";
            while (!input.EndOfInput() && (isdigit16(c) || c == '.' || c == 'x')) {
                temp += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput())
                input.UngetChar(c);

            while (temp.size() != 0) {
                if (IsRealNum(temp)) {
                    tmp.token_type = REALNUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                if (IsBase8Num(temp)) {
                    tmp.token_type = BASE08NUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                if (IsBase16Num(temp)) {
                    tmp.token_type = BASE16NUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                if (IsNum(temp)) {
                    tmp.token_type = NUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                input.UngetChar(temp.back());
                temp.pop_back();
            }
        }
        else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && (isdigit16(c) || c == '.' || c == 'x')) {
                temp += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput())
                input.UngetChar(c);

            while (temp.size() != 0) {
                if (IsRealNum(temp)) {
                    tmp.token_type = REALNUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                if (IsBase8Num(temp)) {
                    tmp.token_type = BASE08NUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                if (IsBase16Num(temp)) {
                    tmp.token_type = BASE16NUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                if (IsNum(temp)) {
                    tmp.token_type = NUM;
                    tmp.lexeme += temp;
                    tmp.line_no = line_no;
                    return tmp;
                }
                input.UngetChar(temp.back());
                temp.pop_back();
            }
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);
    ;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
    case '.':
        tmp.token_type = DOT;
        return tmp;
    case '+':
        tmp.token_type = PLUS;
        return tmp;
    case '-':
        tmp.token_type = MINUS;
        return tmp;
    case '/':
        tmp.token_type = DIV;
        return tmp;
    case '*':
        tmp.token_type = MULT;
        return tmp;
    case '=':
        tmp.token_type = EQUAL;
        return tmp;
    case ':':
        tmp.token_type = COLON;
        return tmp;
    case ',':
        tmp.token_type = COMMA;
        return tmp;
    case ';':
        tmp.token_type = SEMICOLON;
        return tmp;
    case '[':
        tmp.token_type = LBRAC;
        return tmp;
    case ']':
        tmp.token_type = RBRAC;
        return tmp;
    case '(':
        tmp.token_type = LPAREN;
        return tmp;
    case ')':
        tmp.token_type = RPAREN;
        return tmp;
    case '<':
        input.GetChar(c);
        if (c == '=') {
            tmp.token_type = LTEQ;
        }
        else if (c == '>') {
            tmp.token_type = NOTEQUAL;
        }
        else {
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            tmp.token_type = LESS;
        }
        return tmp;
    case '>':
        input.GetChar(c);
        if (c == '=') {
            tmp.token_type = GTEQ;
        }
        else {
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            tmp.token_type = GREATER;
        }
        return tmp;
    default:
        if (isdigit(c)) {
            input.UngetChar(c);
            return ScanNumber();
        }
        else if (isalpha(c)) {
            input.UngetChar(c);
            return ScanIdOrKeyword();
        }
        else if (input.EndOfInput())
            tmp.token_type = END_OF_FILE;
        else
            tmp.token_type = ERROR;

        return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE) {
        token = lexer.GetToken();
        token.Print();
    }
}
/* if(c == '.'){
            temp += c;
            input.GetChar(c);
            input.GetChar(c);
            if(!isdigit(c) || input.EndOfInput()){
                temp += c;
                input.UngetChar(temp.back());
                temp.pop_back();
                input.UngetChar(temp.back());
                temp.pop_back();
                tmp.token_type = NUM;
                tmp.line_no = line_no;
                return tmp;
            }
            while (!input.EndOfInput() && isdigit(c)){
                temp += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput())
                input.UngetChar(c);
            tmp.token_type = REALNUM;
            tmp.lexeme.append(temp);
            tmp.line_no = line_no;
            return tmp;
        }
        */
