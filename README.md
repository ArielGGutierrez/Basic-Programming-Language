# CSE340Project3
This project was created by @ArielGGutierrez, and @Sgomez47-ASU for a university class focusing on programming languages.

The program is an attempt to make a small compiler that takes in a program text file as input and executes the instructions of the program described.  The language is defined by the following CFG:

```
program      → var_section body inputs
var_section  → id_list SEMICOLON
id_list      → ID COMMA id_list | ID
body         → LBRACE stmt_list RBRACE
stmt_list    → stmt stmt_list | stmt
stmt         → assign_stmt | while_stmt | if_stmt | switch_stmt | for_stmt
stmt         → output_stmt | input_stmt
assign_stmt  → ID EQUAL primary SEMICOLON
assign_stmt  → ID EQUAL expr SEMICOLON
expr         → primary op primary
primary      → ID | NUM
op           → PLUS | MINUS | MULT | DIV
output_stmt  → output ID SEMICOLON
input_stmt   → input ID SEMICOLON
while_stmt   → WHILE condition body
if_stmt      → IF condition body
condition    → primary relop primary
relop        → GREATER | LESS | NOTEQUAL
switch_stmt  → SWITCH ID LBRACE case_list RBRACE
switch_stmt  → SWITCH ID LBRACE case_list default_case RBRACE
for_stmt     → FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
case_list    → case case_list | case
case         → CASE NUM COLON body
default_case → DEFAULT COLON body
inputs       → num_list
num_list     → NUM
num_list     → NUM num_list
```

You can look at example inputs in the tests folder.
