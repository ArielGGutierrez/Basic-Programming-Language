#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"

string tokenString[] =
{
    "END_OF_FILE",
    "VAR", "FOR", "IF", "WHILE", "SWITCH", "CASE", "DEFAULT", "INPUT", "OUTPUT", "ARRAY",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
    "NOTEQUAL", "GREATER", "LESS",
    "NUM", "ID", "ERROR"
};

void Parser::syntax_error(TokenType expected, Token actual)
{
    debug("SYNTAX ERROR !!!\nExpected (%s), Got (%s), on Line %d",
        tokenString[expected], tokenString[actual.token_type], actual.line_no);
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
        syntax_error(token, t);
    }
}

void Parser::addToMem(Token t, int value)
{
    /* Dealt with variable section */
    if (next_available < 1000)
    {
        varNames[next_available] = t.lexeme;
        mem[next_available] = value;
        next_available++;
    }
    else
    {
        debug("MEMORY ERROR !!!\nRan out of memory\n");
        exit(EXIT_FAILURE);
    }
}

int Parser::location(string name)
{
    for (int i = 0; i < 1000; i++)
    {
        if (!varNames[i].empty())
        {
            if (varNames[i] == name)
            {
                return i;
            }
        }
    }

    return -1;
}

/* Parser */
//-------------------------------------------------------------------------------------------------
struct InstructionNode* Parser::parse_program()
{
    struct InstructionNode* head;
    parse_var_section();
    head = parse_body();
    parse_inputs();
    return head;
}

void Parser::parse_var_section()
{
    parse_id_list();
    Token t = lexer.peek(1);
    if (t.token_type == SEMICOLON)
    {
        expect(SEMICOLON);
    }

    else syntax_error(SEMICOLON, t);
}

struct InstructionNode* Parser::parse_body()
{
    struct InstructionNode* node;

    Token t = lexer.peek(1);
    if (t.token_type == LBRACE)
    {
        expect(LBRACE);

        node = parse_stmt_list();

        t = lexer.peek(1);
        if (t.token_type == RBRACE)
        {
            expect(RBRACE);
        }

        else syntax_error(RBRACE, t);
    }

    else syntax_error(LBRACE, t);

    return node;
}

void Parser::parse_inputs()
{
    parse_num_list();
}

void Parser::parse_id_list()
{
    Token t = lexer.peek(1);
    if (t.token_type == ID)
    {
        expect(ID);

        addToMem(t, 0);

        t = lexer.peek(1);
        if (t.token_type == COMMA)
        {
            expect(COMMA);
            parse_id_list();
        }
    }
    else syntax_error(ID, t);
}

struct InstructionNode* Parser::parse_stmt_list()
{
    struct InstructionNode* node = parse_stmt();

    Token t = lexer.peek(1);
    if (t.token_type == ID ||
        t.token_type == OUTPUT ||
        t.token_type == INPUT ||
        t.token_type == WHILE ||
        t.token_type == IF ||
        t.token_type == SWITCH ||
        t.token_type == FOR)
    {
        if (node->type == CJMP)
        {
            struct InstructionNode* next = node;
            while (next->next != nullptr)
            {
                next = next->next;
            }
            next->next = parse_stmt_list();
        }
        else node->next = parse_stmt_list();
    }
    return node;
}

struct InstructionNode* Parser::parse_stmt()
{
    struct InstructionNode* node;
    Token t = lexer.peek(1);
    if (t.token_type == ID)          node = parse_assign_stmt();
    else if (t.token_type == OUTPUT) node = parse_output_stmt();
    else if (t.token_type == INPUT)  node = parse_input_stmt();
    else if (t.token_type == WHILE)  node = parse_while_stmt();
    else if (t.token_type == IF)     node = parse_if_stmt();
    else if (t.token_type == SWITCH) node = parse_switch_stmt();
    else if (t.token_type == FOR)    node = parse_for_stmt();
    else
    {
        debug("SYNTAX ERROR !!!\nInvalid statement on Line %d", t.line_no);
        exit(EXIT_FAILURE);
    }
    return node;
}

struct InstructionNode* Parser::parse_assign_stmt()
{
    struct InstructionNode* node = new InstructionNode;
    node->type = ASSIGN;

    Token t = lexer.peek(1);
    if (t.token_type == ID)
    {
        expect(ID);

        node->assign_inst.left_hand_side_index = location(t.lexeme);

        t = lexer.peek(1);
        if (t.token_type == EQUAL)
        {
            expect(EQUAL);

            t = lexer.peek(1);
            Token t2 = lexer.peek(2);
            if (t2.token_type == SEMICOLON)
            {
                node->assign_inst.op = OPERATOR_NONE;
                node->assign_inst.operand1_index = parse_primary();
            }
            else
            {
                parse_expr(node);
            }

            t = lexer.peek(1);
            if (t.token_type == SEMICOLON)
            {
                expect(SEMICOLON);
            }
            else syntax_error(SEMICOLON, t);
        }
        else syntax_error(EQUAL, t);
    }
    else syntax_error(ID, t);

    return node;
}

struct InstructionNode* Parser::parse_while_stmt()
{
    struct InstructionNode* node = new InstructionNode;
    node->type = CJMP;

    Token t = lexer.peek(1);
    if (t.token_type == WHILE)
    {
        expect(WHILE);
        parse_condition(node);
        node->next = parse_body();

        struct InstructionNode* jmp = new InstructionNode;
        jmp->type = JMP;
        jmp->jmp_inst.target = node;

        struct InstructionNode* iterator = node;
        while (iterator->next != nullptr)
        {
            iterator = iterator->next;
        }
        iterator->next = jmp;

        struct InstructionNode* noop = new InstructionNode;
        noop->type = NOOP;

        node->cjmp_inst.target = noop;
    }
    else syntax_error(WHILE, t);
    return node;
}

struct InstructionNode* Parser::parse_if_stmt()
{
    struct InstructionNode* node = new InstructionNode;
    node->type = CJMP;

    Token t = lexer.peek(1);
    if (t.token_type == IF)
    {
        expect(IF);
        parse_condition(node);
        node->next = parse_body();

        struct InstructionNode* noop = new InstructionNode;
        noop->type = NOOP;

        struct InstructionNode* iterator = node;
        while (iterator->next != nullptr)
        {
            iterator = iterator->next;
        }
        iterator->next = noop;

        node->cjmp_inst.target = noop;
    }
    else syntax_error(IF, t);
    return node;
}

struct InstructionNode* Parser::parse_switch_stmt()
{
    struct InstructionNode* node;

    Token t = lexer.peek(1);
    if (t.token_type == SWITCH)
    {
        expect(SWITCH);

        t = lexer.peek(1);
        if (t.token_type == ID)
        {
            expect(ID);
            addToMem(t, stoi(t.lexeme));
            int operand1_index = location(t.lexeme);

            t = lexer.peek(1);
            if (t.token_type == LBRACE)
            {
                expect(LBRACE);

                node = parse_case_list(operand1_index);

                t = lexer.peek(1);
                if (t.token_type == DEFAULT)
                {
                    struct InstructionNode* iteratorLast = node;
                    while (iteratorLast->next != nullptr)
                    {
                        iteratorLast = iteratorLast->next;
                    }

                    struct InstructionNode* iteratorSecondLast = node;
                    while (iteratorSecondLast->next->next != nullptr)
                    {
                        iteratorSecondLast = iteratorSecondLast->next;
                    }

                    struct InstructionNode* defaultCase = parse_default_case();

                    defaultCase->next = iteratorLast;
                    iteratorSecondLast->next = defaultCase;

                    t = lexer.peek(1);
                    if (t.token_type == RBRACE)
                    {
                        expect(RBRACE);
                    }
                    else syntax_error(RBRACE, t);
                }
                else if (t.token_type == RBRACE)
                {
                    expect(RBRACE);
                }
                else syntax_error(RBRACE, t);
            }
            else syntax_error(LBRACE, t);
        }
        else syntax_error(ID, t);
    }
    else syntax_error(SWITCH, t);

    return node;
}

struct InstructionNode* Parser::parse_for_stmt()
{
    struct InstructionNode* node;

    Token t = lexer.peek(1);
    if (t.token_type == FOR)
    {
        expect(FOR);

        t = lexer.peek(1);
        if (t.token_type == LPAREN)
        {
            expect(LPAREN);
            node = parse_assign_stmt();

            struct InstructionNode* condition = new InstructionNode;
            condition->type = CJMP;
            parse_condition(condition);
            node->next = condition;

            t = lexer.peek(1);
            if (t.token_type == SEMICOLON)
            {
                expect(SEMICOLON);

                struct InstructionNode* assign2 = parse_assign_stmt();

                t = lexer.peek(1);
                if (t.token_type == RPAREN)
                {
                    expect(RPAREN);

                    condition->next = parse_body();

                    struct InstructionNode* jmp = new InstructionNode;
                    jmp->type = JMP;
                    jmp->jmp_inst.target = condition;

                    struct InstructionNode* iterator = node;
                    while (iterator->next != nullptr)
                    {
                        iterator = iterator->next;
                    }
                    iterator->next = assign2;
                    iterator->next->next = jmp;

                    struct InstructionNode* noop = new InstructionNode;
                    noop->type = NOOP;

                    node->cjmp_inst.target = noop;
                }
                else syntax_error(RPAREN, t);
            }
            else syntax_error(SEMICOLON, t);
        }
        else syntax_error(LPAREN, t);
    }
    else syntax_error(FOR, t);
    return node;
}

struct InstructionNode* Parser::parse_output_stmt()
{
    struct InstructionNode* node = new InstructionNode;
    node->type = OUT;

    Token t = lexer.peek(1);
    if (t.token_type == OUTPUT)
    {
        expect(OUTPUT);

        t = lexer.peek(1);
        if (t.token_type == ID)
        {
            expect(ID);
            node->output_inst.var_index = location(t.lexeme);

            t = lexer.peek(1);
            if (t.token_type == SEMICOLON)
            {
                expect(SEMICOLON);
            }
            else syntax_error(SEMICOLON, t);
        }
        else syntax_error(ID, t);
    }
    else syntax_error(OUTPUT, t);
    return node;
}

struct InstructionNode* Parser::parse_input_stmt()
{
    struct InstructionNode* node = new InstructionNode;
    node->type = IN;

    Token t = lexer.peek(1);
    if (t.token_type == INPUT)
    {
        expect(INPUT);

        t = lexer.peek(1);
        if (t.token_type == ID)
        {
            expect(ID);
            node->input_inst.var_index = location(t.lexeme);

            t = lexer.peek(1);
            if (t.token_type == SEMICOLON)
            {
                expect(SEMICOLON);
            }
            else syntax_error(SEMICOLON, t);
        }
        else syntax_error(ID, t);
    }
    else syntax_error(INPUT, t);
    return node;
}

int Parser::parse_primary()
{
    Token t = lexer.peek(1);
    if (t.token_type == ID)
    {
        expect(ID);
        return location(t.lexeme);
    }
    else if (t.token_type == NUM)
    {
        expect(NUM);
        addToMem(t, stoi(t.lexeme));
        return location(t.lexeme);
    }
    else
    {
        debug("SYNTAX ERROR !!!\nInvalid primary on Line %d", t.line_no);
        exit(EXIT_FAILURE);
    }
}

void Parser::parse_expr(struct InstructionNode* node)
{
    node->assign_inst.operand1_index = parse_primary();
    node->assign_inst.op = parse_op();
    node->assign_inst.operand2_index = parse_primary();
}

ArithmeticOperatorType Parser::parse_op()
{
    Token t = lexer.peek(1);
    if (t.token_type == PLUS)
    {
        expect(PLUS);
        return OPERATOR_PLUS;
    }
    else if (t.token_type == MINUS)
    {
        expect(MINUS);
        return OPERATOR_MINUS;
    }
    else if (t.token_type == MULT)
    {
        expect(MULT);
        return OPERATOR_MULT;
    }
    else if (t.token_type == DIV)
    {
        expect(DIV);
        return OPERATOR_DIV;
    }
    else
    {
        debug("SYNTAX ERROR !!!\nInvalid operator on Line %d", t.line_no);
        exit(EXIT_FAILURE);
    }
}

void Parser::parse_condition(struct InstructionNode* node)
{
    node->cjmp_inst.operand1_index = parse_primary();
    node->cjmp_inst.condition_op = parse_relop();
    node->cjmp_inst.operand2_index = parse_primary();
}

ConditionalOperatorType Parser::parse_relop()
{
    Token t = lexer.peek(1);
    if (t.token_type == GREATER)
    {
        expect(GREATER);
        return CONDITION_GREATER;
    }
    else if (t.token_type == LESS)
    {
        expect(LESS);
        return CONDITION_LESS;
    }
    else if (t.token_type == NOTEQUAL)
    {
        expect(NOTEQUAL);
        return CONDITION_NOTEQUAL;
    }
    else
    {
        debug("SYNTAX ERROR !!!\nInvalid relop on Line %d", t.line_no);
        exit(EXIT_FAILURE);
    }
}

struct InstructionNode* Parser::parse_case_list(int operand1_index)
{
    struct InstructionNode* node = parse_case(operand1_index);

    Token t = lexer.peek(1);
    if (t.token_type == CASE)
    {
        struct InstructionNode* iterator = node;
        while (iterator->next != nullptr)
        {
            iterator = iterator->next;
        }
        struct InstructionNode* jmp = new InstructionNode;
        jmp->type = JMP;
        iterator->next = jmp;

        struct InstructionNode* noop = new InstructionNode;
        noop->type = NOOP;
        iterator->next->next = noop;
        
        iterator->next->next->next = parse_case_list(operand1_index);

        node->cjmp_inst.target = iterator->next->next->next;

        while (iterator->next != nullptr)
        {
            iterator = iterator->next;
        }
        jmp->jmp_inst.target = iterator;
    }
    return node;
}

struct InstructionNode* Parser::parse_default_case()
{
    struct InstructionNode* node;

    Token t = lexer.peek(1);
    if (t.token_type == DEFAULT)
    {
        expect(DEFAULT);

        t = lexer.peek(1);
        if (t.token_type == COLON)
        {
            expect(COLON);
            node = parse_body();
        }
        else syntax_error(COLON, t);
    }
    else syntax_error(DEFAULT, t);
    return node;
}

struct InstructionNode* Parser::parse_case(int operand1_index)
{
    struct InstructionNode* node = new InstructionNode;
    node->type = CJMP;
    node->cjmp_inst.operand1_index = operand1_index;

    Token t = lexer.peek(1);
    if (t.token_type == CASE)
    {
        expect(CASE);

        t = lexer.peek(1);
        if (t.token_type == NUM)
        {
            expect(NUM);
            addToMem(t, stoi(t.lexeme));
            int operand2_index = location(t.lexeme);
            node->cjmp_inst.operand2_index = operand2_index;

            t = lexer.peek(1);
            if (t.token_type == COLON)
            {
                expect(COLON);
                node->next = parse_body();
            }
            else syntax_error(COLON, t);
        }
        else syntax_error(NUM, t);
    }
    else syntax_error(CASE, t);
    return node;
}

void Parser::parse_num_list()
{
    Token t = lexer.peek(1);
    if (t.token_type == NUM)
    {
        expect(NUM);

        /* Deal with Input section at the end */
        int input = stoi(t.lexeme);
        inputs.insert(inputs.end(), input);

        t = lexer.peek(1);
        if (t.token_type == NUM)
        {
            parse_num_list();
        }
    }
    else syntax_error(NUM, t);
}
//-------------------------------------------------------------------------------------------------

struct InstructionNode * parse_generate_intermediate_representation()
{
    Parser parser;
    return parser.parse_program();
}