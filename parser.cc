#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"

void Parser::syntax_error()
{
    debug("SYNTAX ERROR !!!\n");
    exit(EXIT_FAILURE);
}

void Parser::expect(TokenType token)
{
    Token t = lexer.peek(1);
    if (token == t.token_type)
    {
        t = lexer.GetToken();
    }
    else
    {
        syntax_error();
    }
}

/* Parser */
//-------------------------------------------------------------------------------------------------
void Parser::parse_program()
{
    parse_var_section();
    parse_body();
    parse_inputs();
}

void Parser::parse_var_section()
{
    parse_id_list();
    Token t = lexer.peek(1);
    if (t.token_type == SEMICOLON)
    {
        expect(SEMICOLON);
    }

    else
    {
        syntax_error();
    }
}

void Parser::parse_body() {}
void Parser::parse_inputs() {}
void Parser::parse_id_list() {}
void Parser::parse_stmt_list() {}
void Parser::parse_stmt() {}
void Parser::parse_assign_stmt() {}
void Parser::parse_while_stmt() {}
void Parser::parse_if_stmt() {}
void Parser::parse_switch_stmt() {}
void Parser::parse_for_stmt() {}
void Parser::parse_output_stmt() {}
void Parser::parse_input_stmt() {}
void Parser::parse_primary() {}
void Parser::parse_expr() {}
void Parser::parse_op() {}
void Parser::parse_condition() {}
void Parser::parse_relop() {}
void Parser::parse_case_list() {}
void Parser::parse_default_case() {}
void Parser::parse_case() {}
void Parser::parse_num_list() {}
//-------------------------------------------------------------------------------------------------

struct InstructionNode * parse_generate_intermediate_representation()
{

}