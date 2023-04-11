#ifndef __PARSER__H__
#define __PARSER__H__

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "compiler.h"
#include "lexer.h"

using namespace std;

class Parser
{
    private:
        
    public:
        string varNames[1000];
        LexicalAnalyzer lexer;
        //struct InstructionNode* currIns;

        void syntax_error(TokenType expected, Token actual);
		void expect(TokenType token);
        void addToMem(Token t, int value);

        struct InstructionNode* parse_program();
        void parse_var_section();
        struct InstructionNode* parse_body();
        void parse_inputs();
        void parse_id_list();
        struct InstructionNode* parse_stmt_list();
        struct InstructionNode* parse_stmt();
        struct InstructionNode* parse_assign_stmt();
        struct InstructionNode* parse_while_stmt();
        struct InstructionNode* parse_if_stmt();
        struct InstructionNode* parse_switch_stmt();
        struct InstructionNode* parse_for_stmt();
        struct InstructionNode* parse_output_stmt();
        struct InstructionNode* parse_input_stmt();
        int parse_primary();
        void parse_expr(struct InstructionNode* node);
        ArithmeticOperatorType parse_op();
        void parse_condition(struct InstructionNode* node);
        ConditionalOperatorType parse_relop();
        struct InstructionNode* parse_case_list(int operand1_index);
        struct InstructionNode* parse_default_case();
        struct InstructionNode* parse_case(int operand1_index);
        void parse_num_list();

        int location(string varName);
};

#endif  //__PARSER__H__