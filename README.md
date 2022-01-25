# cs280-PA4
Programming Assingment 4 CS-280 : Implementing an Interpreter - Recursive Descent Parser


In this programming assignment, you will be implementing an interpreter for our small programming language and write a C++ program to test it. You are required to modify the parser you have implemented for the small programming language to implement the simple language interpreter. The syntax definitions and tokens of the programming language were given in Programming Assignment 2, and are given below using EBNF notations.

Prog := begin StmtList end
StmtList := Stmt; { Stmt;}
Stmt := PrintStmt | AssignStmt | IfStmt
PrintStmt := print ExprList
IfStmt := if (Expr) then Stmt
AssignStmt := Var = Expr
ExprList := Expr {, Expr}
Expr := Term {(+|-) Term}
Term := Factor {(*|/) Factor}
Var := ident
Factor := ident | iconst | rconst | sconst | (Expr)

The following points describe the programming language. You have already implemented all of the syntactic rules of the language as part of the parser. The points related to the dynamic semantics (i.e. runt-time checks) of the language must be implemented in your interpreter. These include points 6-13 in the following list.
1. The language has three types: Integer, Real, and String.
2. The PLUS and MINUS operators in Expr represent addition and subtraction.
3. The MULT and DIV operators in Term represent multiplication and division.
4. The PLUS, MINUS, MULT and DIV operators are left associative.
5. MULT and DIV have higher precedence than PLUS and MINUS.
6. The EQ operator in the AssignStmt assigns a value to a variable. It evaluates the Expr on the right-hand side, determines the type of the value and saves its value in memory associated with the left-hand side (an IDENT). If the IDENT does not exist, it is created. If the IDENT already exists, its value is replaced.
7. The type of a variable is the type of the value assigned to it in an assignment statement. The type of a variable is either Integer, Real, or String; and it is determined from the first time it has been assigned a value in an assignment statement.
8. Any reassignments for the same variable have to be for a value of the same type or a compatible one. For example, it is an error to assign a numeric value to a variable of a string type, or to assign a string value to a numeric variable (i.e., integer or real). However, an integer variable can be assigned a real value, and a real variable can be assigned an integer value. In either case, conversion of the value to the type of the variable must be applied at run-time.
9. An IfStmt evaluates the expression (Expr), which must be an integer. If the expression value is nonzero, then the Stmt is executed, otherwise it is not.
10. An IfStmt statement whose Expr is not of an integer type is an error.
11. A PrintStmt evaluates the list of expressions (ExprList), and prints their values in sequence from left to right, followed by a newline.
12. The binary operations for addition, subtraction, multiplication, and division are performed upon two numeric operands (i.e., integer or real) of the same or different types. If the operands are of the same type, the type of the result is the same type as the operator’s operands. Otherwise, the type of the result is real. It is an error to have string operand to any of the arithmetic operations.
13. It is an error to use a variable in an expression before it has been assigned.

Interpreter Requirements:
Implement an interpreter for the simple programming language based on the recursive-descent parser developed in Programming Assignment 3. You need to modify the parser functions to include the required actions of the interpreter for evaluating expressions, determining the type of expression values, executing the statements, and checking run-time errors. You may use the lexical analyzer you wrote for Programming Assignment 2 and the parser you wrote for Programming Assignment 3. Otherwise you may use the provided implementations for the lexical analyzer and parser when they are posted. Rename the parse.cpp file as parseRun.cpp to reflect the applied changes on the current parser implementation for building an interpreter. The interpreter should provide the following:
• The interpreter performs syntax analysis of the input source code statement by statement, then executes the statement if there is no syntactic or semantic error.
• The interpreter builds a symbol table for all the defined variables.
• The interpreter evaluates expressions and determines their values and types.
• The results of an unsuccessful parsing are a set of error messages printed by the parser functions, as well as the error messages that might be detected by the lexical analyzer.
• Any failures due to the process of parsing or interpreting the input program should cause the process of interpretation to stop and return back.
• In addition to the error messages generated due to parsing, the interpreter generates error messages due to its semantics check. The assignment does not specify the exact error messages that should be printed out by the interpreter; however, the format of the messages should be the line number, followed by a colon and a space, followed by some descriptive text, similar to the format used in Programming Assignment 3. Suggested messages of the interpreter’s
semantics check might include messages such as "Run-Time Error-Illegal Type for If Statement Expression", " Run-Time Error-Illegal Assignment Operation", etc.
You are given a header file for the process of interpretation, called “val.h”, the modified “parse.h” file, renamed as “parseRun.h”, and the “lex.h”.
“val.h” includes the following:
• A class, called Value, representing a value object in the interpreted source code for constants, variables or evaluated expressions. It includes:
o Three data members of the Value class for holding a value as either an integer, a real, or a string,
o A data member holding the type of the value using an enum type defined as:
enum ValType { VINT, VREAL, VSTR, VERR };
o Getter functions to return the value of an object
o Getter function to return the type of the value.
o Member functions to check the type of the Value object.
o Overloaded member functions for the arithmetic operators (+, -, *, and /)
o A friend function for overloading the operator<<.
• The implementation of the overloaded operators’ functions: operator+(), operator-(), operator*(), and operator\().
• The declaration of a map container for the construction of a symbol table, called symbolTable. Each entry of symbolTable is a pair of a string and a Value object, representing a variable name and its corresponding value.
• The declaration of a pointer variable to a queue container of Value objects.
“parseRun.h” modifications include the following changes:
The definitions of function headers of Var, Expr, Term, and Factor functions have been changed as follows

extern bool Var(istream& in, int& line, LexItem & tok);

Var function determines if the current identifier has been seen before or not. If it did not, it creates an entry for it in the defVar container of defined variables, creates a Value object initialized with error value (VERR), since it has not been assigned value yet, and inserts it in the symbol table container. A reference to LexItem of the IDENT token is returned through tok parameter to the AssignStmt function.

extern bool Expr(istream& in, int& line, Value & retVal);
The retVal reference parameter returns the value of computing the Expr’s Terms for the addition/subtraction operations to the caller.

extern bool Term(istream& in, int& line, Value & retVal);
The retVal reference parameter returns the value of computing the Term’s Factors for the multiplication/division operations to the Expr function.
• extern bool Factor(istream& in, int& line, Value & retVal);


Based on the next token, retVal returns the value of a constant (e.g., ICONST, RCONST, SCONST), the value of a defined identifier (IDENT) from the symbol table, or the value of evaluating an expression (Expr) to the Factor function.
Program Requirements:
1. There can be at most one file name specified on the command line. If more than one filename is provided, the program should print “ONLY ONE FILE NAME ALLOWED” and it should stop running.
2. If the program cannot open a filename that is given, the program should print “CANNOT OPEN THE FILE {arg}”, where {arg} is the filename given, and it should stop running.
3. The result of an unsuccessful parsing is a set of error messages printed by the parser functions.
4. If the call to Prog() function fails, the program should stop and display a message as "Unsuccessful Interpretation ", and display the number of errors detected. For example:
Unsuccessful Interpretation
Number of Errors 3
5. If the call to Prog() function succeeds, the program should stop and display a message such as "Successful Execution", and the program stops.
6. Testing files are provided which represent various cases of run-time sematic error types. They can be used for testing your implementation. You can also come up with your own testing cases.
Submission Guidelines


