/* Implementation of Interpreter
 * parseRun.cpp
 * Programming Assignment 4
 *
 * modification of parser.cpp
*/
#include "parseRun.h"
#include "val.h"

//Program is: Prog := begin StmtList end
inline bool Prog(istream& in, int& line)
{
	bool sl = false;
	LexItem tok = Parser::GetNextToken(in, line);
	//cout << "in Prog" << endl;
	
	if (tok.GetToken() == BEGIN) {
		sl = StmtList(in, line);
		if( !sl  )
			ParseError(line, "No statements in program");
		if( error_count > 0 )
			return false;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok.GetToken() == END)
		return true;
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else
		return false;
}

// StmtList is a Stmt followed by semicolon followed by a StmtList
inline bool StmtList(istream& in, int& line) {
 	//cout << "in StmtList" << endl;
	bool status = Stmt(in, line);
	
	if( !status )
		return false;
	LexItem tok = Parser::GetNextToken(in, line);
	
	if( tok == SCOMA ) {
		status = StmtList(in, line);
	}
	else if (tok == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if (tok == END) {
		Parser::PushBackToken(tok);
		return true;
	}
	else {
		ParseError(line, "Missing semicolon");
		return false;
	}
	return status;
}

//Stmt is either a PrintStmt, IfStmt, or an AssigStmt
inline bool Stmt(istream& in, int& line) {
	bool status;
	//cout << "in Stmt" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		//cout << "status: " << (status? true:false) <<endl;
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
        Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		break;

	case END:
		Parser::PushBackToken(t);
		return true;
	case ERR:
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << t.GetLexeme() << ")" << endl;
		return false;
	case DONE:
		return false;

	default:
		ParseError(line, "Invalid statement");
		return false;
	}

	return status;
}

//PrintStmt:= print ExpreList 
inline bool PrintStmt(istream& in, int& line) {
   
    //cout << "in Print Stmt" << endl;
        /*create an empty queue of Value objects.*/
    ValQue = new queue<Value>;
    
    bool ex = ExprList(in, line);
    if( !ex )
    {
        ParseError(line, "Missing expression after print"); //Empty the queue and delete.
        while(!(*ValQue).empty())
        {
            ValQue->pop();
        
        }
            delete ValQue;
            return false;
    }
    //Evaluate: print out the list of expressions' values
    LexItem t = Parser::GetNextToken(in, line);
    if(t.GetToken() == SCOMA)
    {
        //Execute the statement after making sure
        //the semicolon is seen.
        while (!(*ValQue).empty())
        {
            Value nextVal = (*ValQue).front();
            cout << nextVal;
            ValQue->pop();
        }
         cout << endl;
    }
       Parser::PushBackToken(t);
    return ex;
}

//IfStmt:= if (Expr) then Stmt
inline bool IfStmt(istream& in, int& line) {
    Value newvalIF;
    bool nonzer = true;         // nonzer will be used to determine if expr in if statement is true
    bool ex = false;
	LexItem t;
    
	//cout << "in IfStmt" << endl;
	if( (t=Parser::GetNextToken(in, line)) != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	ex = Expr(in, line, newvalIF);
    
	if( !ex ) {
		ParseError(line, "Missing if statement expression");
		return false;
	}
    if(newvalIF.GetType() != VINT )        // If expression in if statement is not integer
    {
        ParseError(line, "Run-Time Error-Illegal Type for If statement Expression");
        return false;
    }
    else if(newvalIF.GetType() == VINT)
    {
        if(newvalIF.GetInt() == 0)
        {
            nonzer = false;             // Nonzero(false) if statment
        }
    }
	if((t=Parser::GetNextToken(in, line)) != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	if((t=Parser::GetNextToken(in, line)) != THEN ) {
		
		ParseError(line, "Missing THEN");
		return false;
	}
    // if expression is false in if statemnt skip over it.
    // doing this by completing rest of if statement but do not print out
    // by going through full if statement then you can recognize other errors
    
        t = Parser::GetNextToken(in, line);
        if((t == PRINT && nonzer == false))
        {
            ValQue = new queue<Value>;
            
            bool ex = ExprList(in, line);
            if( !ex )
            {
                ParseError(line, "Missing expression after print"); //Empty the queue and delete.
                while(!(*ValQue).empty())
                {
                    ValQue->pop();                                  // no cout
                
                }
                    delete ValQue;
                    return false;
            }
            //Evaluate: print out the list of expressions' values
            LexItem t = Parser::GetNextToken(in, line);
            if(t.GetToken() == SCOMA)
            {
                //Execute the statement after making sure //the semicolon is seen.
                while (!(*ValQue).empty())
                {
                    Value nextVal = (*ValQue).front();
                    ValQue->pop();
                }
            }
               Parser::PushBackToken(t);
            return ex;
        }
    Parser::PushBackToken(t);
	bool st = Stmt(in, line);
	if( !st ) {
		ParseError(line, "Missing statement for if");
		return false;
	}
	
	//Evaluate: execute the if statement
	
	return st;
}

//Var:= ident
inline bool Var(istream& in, int& line, LexItem & tk)
{
	//called only from the AssignStmt function
	string identstr;
    
	//cout << "in Var" << endl;
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
        string identstr = tok.GetLexeme();
        if(defVar.find(identstr) == defVar.end())     // if token is not already defined
        {                                              // define it
            symbolTable.insert({identstr, VERR});
            defVar[identstr] = true;
        }
        
        return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}

//AssignStmt:= Var = Expr
inline bool AssignStmt(istream& in, int& line) {
    Value retVal;
    //cout << "in AssignStmt" << endl;
	bool varstatus = false, status = false;
    LexItem t;
    t = Parser::GetNextToken(in, line);
    string identstr = t.GetLexeme();
    Parser::PushBackToken(t);
    Value tv;
    ValType type;
   
    // if var has been seen already
    // used to check type
    bool seen = false;
    if(defVar.find(identstr) != defVar.end())
    {
        tv = symbolTable[identstr];
        type = tv.GetType();
        seen = true;
    }
    
	varstatus = Var( in, line, t);
	
	if (varstatus){
		if ((t=Parser::GetNextToken(in, line)) == EQ){
            
			status = Expr(in, line, retVal);
            
            // Check to see if types do no match
            // Errors are int/real not compatible with str
            if(seen == true)
            {
                if(type != retVal.GetType())
                {
                    if(type == VSTR && retVal.GetType() == VREAL )
                    {
                        ParseError(line, "Illegal Assignment Operation");
                        return false;
                    }
                    if(type == VSTR && retVal.GetType() == VINT)
                    {
                        ParseError(line, "Illegal Assignment Operation");
                        return false;
                    }
                    if(type == VINT && retVal.GetType() == VSTR )
                    {
                        ParseError(line, "Illegal Assignment Operation");
                        return false;
                    }
                    if(type == VREAL && retVal.GetType() == VSTR )
                    {
                        ParseError(line, "Illegal Assignment Operation");
                        return false;
                    }
                }
            }
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
           // cout << endl << "retval after assignment statment for " + identstr + ": " << retVal << endl;
        
            symbolTable[identstr] = retVal;
            
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator =");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}

//ExprList:= Expr {,Expr}
inline bool ExprList(istream& in, int& line) {
    Value newvalEL;
	bool status = false;
	//cout << "in ExprList" << endl;
	status = Expr(in, line, newvalEL);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMA) {
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}

//Expr:= Term {(+|-) Term}
inline bool Expr(istream& in, int& line, Value & retVal) {
	
   
    bool t1 = Term(in, line, retVal);
    Value Evalue = retVal;
	LexItem tok;
	//cout << "in Expr" << endl;
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
        if(Evalue.GetType() == VSTR)  // no string can plus/minus
        {
            ParseError(line, "Run-Time Error Illegal String Type Operation");
            return false;
        }
        char s;
        if(tok == PLUS)
        {
            s = '+';
        }
        else
        {
            s = '-';
        }
		t1 = Term(in, line, retVal);
        Value pmvalue = retVal;
        if(pmvalue.GetType() == VSTR) // no string can plus/minus
        {
            ParseError(line, "Run-Time Error Illegal Mixed Type Operation");
            return false;
        }
        // perform plus or minus
        if(s == '+')
        {
            Evalue = pmvalue + Evalue;
        }
        if( s == '-')
        {
            Evalue =  Evalue - pmvalue;
        }
        
		if( !t1 ) 
		{
			ParseError(line, "Missing expression after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		//Evaluate: evaluate the expression for addition or subtraction
	}
    retVal = Evalue;
	Parser::PushBackToken(tok);
    ValQue->push(retVal);
	return true;
}

//Term:= Factor {(*|/) Factor}
inline bool Term(istream& in, int& line, Value & retVal) {
	//cout << "in Term" << endl;
	bool t1 = Factor(in, line, retVal);
    Value Fvalue = retVal;
    //cout << endl << Fvalue << endl;
	LexItem tok;
	//cout << "status of factor1: " << t1<< endl;
	if( !t1 ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
        if(Fvalue.GetType() == VSTR)
        {
            ParseError(line, "Run-Time Error Illegal String Type Operation"); // no string can mult/div
            return false;
        }
        // perform the mult or div
        char s;
        if(tok == MULT)
        {
            s = '*';
        }
        else
        {
            s = '/';
        }
		t1 = Factor(in, line, retVal);
        Value mdvalue = retVal;
        if(mdvalue.GetType() == VSTR)    // no string can mult/div
        {
            ParseError(line, "Run-Time Error Illegal Mixed Type Operation");
            return false;
        }
        if(s == '*')
        {
            Fvalue = mdvalue * Fvalue;
        }
        if( s == '/')
        {
            Fvalue =  Fvalue / mdvalue;
        }
		//cout << "status of factor2: " << t1<< endl;
		if( !t1 ) {
			ParseError(line, "Missing expression after operator");
			return false;
		}
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		//Evaluate: evaluate the expression for multiplication or division
	}
    retVal = Fvalue;
	Parser::PushBackToken(tok);
	return true;
}
//Factor := ident | iconst | rconst | sconst | (Expr)
inline bool Factor(istream& in, int& line, Value & retVal) {
    Value newvalF = retVal;
    //cout << "in Factor" << endl;
	LexItem tok = Parser::GetNextToken(in, line);
	

	if( tok == IDENT ) {
		//check if the var is defined 
		int val;
        string id = tok.GetLexeme();
        if(defVar.find(id) == defVar.end())
        {
            ParseError(line, "Undefined Variable");
            return false;
        }
        else
        {
            retVal = symbolTable.find(id)->second;
            return true;
        }
	}
    else if( tok == ICONST ) {
    //convert the string of digits to an integer number //create a Value object for ICONST
        int val;
        val = stoi(tok.GetLexeme());
        //cout << "integer const: " << val << endl;
        Value newVal(val);
        retVal = newVal;
        return true;
    }
	else if( tok == SCONST ) {
		
		//create a Val object for SICONST and enter into sysmol table
        string val;
        val = tok.GetLexeme();
        //cout << "String Constant: " + val << endl;
        Value newVal(val);
        retVal = newVal;
		return true;
	}
	else if( tok == RCONST ) {
		//convert the string of digits to real number
		//create a Val object for RCONST and enter into sysmol table
        float val = stof(tok.GetLexeme());
        //cout << "real const: " << val << endl;
        Value newVal(val);
        retVal = newVal;
        return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, newvalF);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return false;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	ParseError(line, "Unrecognized input");
	return 0;
}



