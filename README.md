# Simple compiler written in C++

## 1.1 Syntax
```ebnf
<program> ::= <statement>*

<statement> ::= <var_declaration>
              | <print_statement>
              | <if_statement>
              | <while_statement>
              | <block_statement>
              | <expression_statement>
              | <function_declaration>
              | <return_statement>

<var_declaration> ::= "var" <identifier> ("=" <expression>)? ";"

<print_statement> ::= "print" <expression> ";"

<if_statement> ::= "if" "(" <expression> ")" <statement> ("else" <statement>)?

<while_statement> ::= "while" "(" <expression> ")" <statement>

<block_statement> ::= "{" <statement>* "}"

<expression_statement> ::= <expression> ";"

<function_declaration> ::= "fun" <identifier> "(" <parameters>? ")" <block_statement>

<parameters> ::= <identifier> ("," <identifier>)*

<return_statement> ::= "return" <expression>? ";"

<expression> ::= <assignment>

<assignment> ::= <logical_or> ("=" <assignment>)?

<logical_or> ::= <logical_and> ("||" <logical_and>)*

<logical_and> ::= <equality> ("&&" <equality>)*

<equality> ::= <comparison> (("==" | "!=") <comparison>)*

<comparison> ::= <term> (("<" | ">" | "<=" | ">=") <term>)*

<term> ::= <factor> (("+" | "-") <factor>)*

<factor> ::= <unary> (("*" | "/") <unary>)*

<unary> ::= ("!" | "-") <unary>
          | <primary>

<primary> ::= <number>
            | <string>
            | "true"
            | "false"
            | <identifier>
            | <call>
            | "(" <expression> ")"
            | <array_literal>
            | <array_access>

<array_literal> ::= "[" <expression> ("," <expression>)* "]"

<array_access> ::= <identifier> "[" <expression> "]"

<array_assignment> ::= <identifier> "[" <expression> "]" "=" <expression>

<call> ::= <identifier> "(" <arguments>? ")"

<arguments> ::= <expression> ("," <expression>)*

<number> ::= <digit>+ ("." <digit>+)?

<string> ::= '"' <any_char>* '"'

<identifier> ::= <letter> (<letter> | <digit> | "_")*

<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

<letter> ::= "a" | "b" | ... | "z" | "A" | "B" | ... | "Z"
```

### What to do?
--- 
1) Lexer
2) Parser
3) AST
4) Semantic Analyzer
5) Interpreter
6) Optimizer
