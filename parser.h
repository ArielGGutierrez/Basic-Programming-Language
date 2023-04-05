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

class Parser
{
    private:

    public:
        LexicalAnalyzer lexer;
        struct InstructionNode* head;
        struct InstructionNode* currIns;

        void syntax_error();
		void expect(TokenType token);

        void parse_program();
        void parse_var_section();
        void parse_body();
        void parse_inputs();
        void parse_id_list();
        void parse_stmt_list();
        void parse_stmt();
        void parse_assign_stmt();
        void parse_while_stmt();
        void parse_if_stmt();
        void parse_switch_stmt();
        void parse_for_stmt();
        void parse_output_stmt();
        void parse_input_stmt();
        void parse_primary();
        void parse_expr();
        void parse_op();
        void parse_condition();
        void parse_relop();
        void parse_case_list();
        void parse_default_case();
        void parse_case();
        void parse_num_list();
};

#endif  //__PARSER__H__