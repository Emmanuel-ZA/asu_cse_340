#include "compiler.h"
#include "lexer.h"
#include <exception>
#include <execution>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

LexicalAnalyzer lexer;

// Forward declarations
void parse_program();
void parse_var_section();
void parse_id_list();
void parse_body();
void parse_stmt_list();
void parse_stmt();
void parse_assign_stmt();
void parse_expr();
void parse_primary();
void parse_op();
void parse_output_stmt();
void parse_input_stmt();
void parse_while_stmt();
void parse_relop();
void parse_if_stmt();
void parse_condition();
void parse_switch_stmt();
void parse_case_list();
void parse_case();
void parse_default_case();
void parse_for_stmt();
void parse_inputs();
void parse_num_list();

void parse_program()
{
    parse_var_section();
    parse_body();
    parse_inputs();
}

void parse_var_section()
{
    parse_id_list();
    Token t = lexer.GetToken();
    if (t.token_type != SEMICOLON)
        exit(1);
}

void parse_id_list()
{
    Token t = lexer.GetToken();
    if (t.token_type != ID)
        exit(1);

    t = lexer.peek(1);
    if (t.token_type == COMMA) {
        lexer.GetToken();
        parse_id_list();
    }
}

void parse_body()
{
    Token t = lexer.GetToken();
    if (t.token_type != LBRACE)
        exit(1);
    parse_stmt_list();
    t = lexer.GetToken();
    if (t.token_type != RBRACE)
        exit(1);
}

void parse_stmt_list()
{
    parse_stmt();
    Token t = lexer.peek(1);
    if (t.token_type != END_OF_FILE && t.token_type != RBRACE)
        parse_stmt_list();
}

void parse_stmt()
{
    Token t = lexer.peek(1);
    switch (t.token_type) {
    case WHILE:
        parse_while_stmt();
        break;
    case IF:
        parse_if_stmt();
        break;
    case SWITCH:
        parse_switch_stmt();
        break;
    case FOR:
        parse_for_stmt();
        break;
    case OUTPUT:
        parse_output_stmt();
        break;
    case INPUT:
        parse_input_stmt();
        break;
    case ID:
        parse_assign_stmt();
        break;
    default:
        exit(1);
    }
}

void parse_assign_stmt()
{
    Token t = lexer.GetToken();
    if (t.token_type != ID)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != EQUAL)
        exit(1);
    parse_primary();
    t = lexer.peek(1);
    if (t.token_type == PLUS || t.token_type == MINUS ||
        t.token_type == MULT  || t.token_type == DIV) {
        parse_op();
        parse_primary();
    }
    t = lexer.GetToken();
    if (t.token_type != SEMICOLON)
        exit(1);
}

void parse_expr()
{
    parse_primary();
    parse_op();
    parse_primary();
}

void parse_primary()
{
    Token t = lexer.GetToken();
    if (t.token_type != ID && t.token_type != NUM)
        exit(1);
}

void parse_op()
{
    Token t = lexer.GetToken();
    if (t.token_type != PLUS && t.token_type != MINUS && t.token_type != MULT &&
        t.token_type != DIV)
        exit(1);
}

void parse_output_stmt()
{
    Token t = lexer.GetToken();
    if (t.token_type != OUTPUT)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != ID)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != SEMICOLON)
        exit(1);
}

void parse_input_stmt()
{
    Token t = lexer.GetToken();
    if (t.token_type != INPUT)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != ID)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != SEMICOLON)
        exit(1);
}

void parse_while_stmt()
{
    Token t = lexer.GetToken();
    if (t.token_type != WHILE)
        exit(1);
    parse_condition();
    parse_body();
}

void parse_if_stmt()
{
    Token t = lexer.GetToken();
    if (t.token_type != IF)
        exit(1);
    parse_condition();
    parse_body();
}
void parse_condition()
{
    parse_primary();
    parse_relop();
    parse_primary();
}

void parse_relop()
{
    Token t = lexer.GetToken();
    if (t.token_type != GREATER && t.token_type != LESS && t.token_type != NOTEQUAL)
        exit(1);
}
void parse_switch_stmt()
{
    Token t = lexer.GetToken();
    if (t.token_type != SWITCH)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != ID)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != LBRACE)
        exit(1);
    parse_case_list();
    t = lexer.peek(1);
    if (t.token_type == DEFAULT)
        parse_default_case();
    t = lexer.GetToken();
    if (t.token_type != RBRACE) exit(1);
}

void parse_for_stmt()
{
    Token t = lexer.GetToken();
    if (t.token_type != FOR)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != LPAREN)
        exit(1);
    parse_assign_stmt();
    parse_condition();
    t = lexer.GetToken();
    if (t.token_type != SEMICOLON)
        exit(1);
    parse_assign_stmt();
    t = lexer.GetToken();
    if (t.token_type != RPAREN)
        exit(1);
    parse_body();
}

void parse_case_list()
{
    parse_case();
    Token t = lexer.peek(1);
    if (t.token_type == CASE)
        parse_case_list();
}
void parse_case()
{
    Token t = lexer.GetToken();
    if (t.token_type != CASE)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != NUM)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != COLON)
        exit(1);
    parse_body();
}

void parse_default_case()
{
    Token t = lexer.GetToken();
    if (t.token_type != DEFAULT)
        exit(1);
    t = lexer.GetToken();
    if (t.token_type != COLON)
        exit(1);
    parse_body();
}
void parse_inputs() { parse_num_list(); }
void parse_num_list()
{
    Token t = lexer.GetToken();
    if (t.token_type != NUM)
        exit(1);
    t = lexer.peek(1);
    if (t.token_type == NUM)
        parse_num_list();
}

struct InstructionNode *parse_generate_intermediate_representation()
{
    return NULL;
}
