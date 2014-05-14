//this also includes all necessary libraries
//don't double include files
#include "ast.cpp";
#include "lexer.cpp";

//token in use at this point 
static int currToken;

//precedence for binary operators
static std::map<char, int> precedence;
static int getNextToken();

//Get the precedence of an operator
static int tokenPrecedence() {
	if (!isascii(currToken)) return -1;

	// Make sure it's declared
	int p = precedence[currToken];
	if (p <= 0 || p == std::string::npos) return -1;
	return p;
}

//error handling
ExprAST *error(const char* message) { fprintf(stderr, "error: %s\n", message); return 0; }
PrototypeAST *errorP(const char* message) { error(message); return 0; }

static ExprAST *parseExpression();

static ExprAST *parseIdentifierExpr() {
	std::string id = identifier;
	getNextToken();

	if (currToken != '(') return new VariableExprAST(id);
	//eat the '('
	getNextToken();
	std::vector<ExprAST*> args;

	//get the parenthesized expression
	if (currToken != ')') {
		while (1) {
			ExprAST *arg = parseExpression();
			if (!arg) return 0;
			args.push_back(arg);

			if (currToken == ')') break;
			if (currToken != ',') return error("Expected ')' or ',' in argument list");
			getNextToken();
		}
	}

	// Eat the ')'.
	getNextToken();
	return new CallExprAST(id, args);
}

//'(' expression ')'
static ExprAST *parseParenExpr() {
	getNextToken();  // eat (
	ExprAST *v = parseExpression();
	if (!v) return 0;

	if (currToken != ')') return error("expected ')'");
	getNextToken();  // eat )
	return v;
}

//simple number
static ExprAST *parseNumberExpr() {
	ExprAST *r = new NumberExprAST(number);
	getNextToken();
	return r;
}

//identifier, number, or parentheses
static ExprAST *parsePrimary() {
	switch (currToken) {
	case tok_identifier: return parseIdentifierExpr();
	case tok_number:     return parseNumberExpr();
	case '(':            return parseParenExpr();
	default: return error("unknown token");
	}
}

static ExprAST *ParseBinOpRHS(int expressionPrecedence, ExprAST *LHS) {
	while (1) {
		int precedence = tokenPrecedence();
		if (precedence < expressionPrecedence) return LHS; //yay, this is easy!
		int op = currToken;
		getNextToken();  // eat the operator

		// Parse the primary expression after the binary operator.
		ExprAST *RHS = parsePrimary();
		if (!RHS) return 0;

		//if it goes with RHS, bind it with RHS
		int next = tokenPrecedence();
		if (precedence < next) {
			RHS = ParseBinOpRHS(++precedence, RHS);
			if (RHS == 0) return 0;
		}

		// Merge LHS/RHS.
		LHS = new BinaryExprAST(op, LHS, RHS);
	}
}


static ExprAST *parseExpression() {
	ExprAST *LHS = parsePrimary();
	if (!LHS) return 0;

	return ParseBinOpRHS(0, LHS);
}

//name (var var);
static PrototypeAST *parsePrototype() {
	//a prototype needs a name first
	if (currToken != tok_identifier) return errorP("Expected function name in prototype");

	std::string name = identifier;
	getNextToken();

	//a prototype then needs a '('
	if (currToken != '(') return errorP("Expected '(' in prototype beginning");

	//get the arguments in the parentheses
	std::vector<std::string> arguments;
	while (getNextToken() == tok_identifier)
		arguments.push_back(identifier);
	if (currToken != ')') return errorP("Expected ')' in prototype end");

	getNextToken();  // eat ')'.

	//finished
	return new PrototypeAST(name, arguments);
}

//def prototype name
static FunctionAST *parseDefinition() {
	getNextToken();  //get the definition
	PrototypeAST *p = parsePrototype();
	if (p == 0) return 0;
	if (ExprAST *exp = parseExpression()) return new FunctionAST(p, exp);
	return 0;
}

//name expression
static FunctionAST *parseTopLevelExpr() {
	if (ExprAST *E = parseExpression()) {
		PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
		return new FunctionAST(Proto, E);
	}
	return 0;
}

//extern name
static PrototypeAST *parseExtern() {
	getNextToken();
	return parsePrototype();
}

static void evaluateDefinition() {
	if (parseDefinition()) fprintf(stdout, "Parsed a function definition.\n");
	else getNextToken();
}

static void evaluateExtern() {
	if (parseExtern()) fprintf(stdout, "Parsed an extern\n");
	else getNextToken();
}

static void evaluateTopLevelExpression() {
	if (parseTopLevelExpr()) fprintf(stdout, "Parsed a top-level expr\n");
	else getNextToken();
}

//definition | external | expression | ';'
static void run() {
	while (1) {
		switch (currToken) {
		case tok_eof:
			return;
		case ';':
			getNextToken();
			break;
		case tok_def:
			evaluateDefinition(); break;
		case tok_extern:
			evaluateExtern(); break;
		default:
			evaluateTopLevelExpression(); break;
		}
		fprintf(stdout, "oxygen> ");
	}
}

//skip token
static int getNextToken() {
	return currToken = gettoken();
}