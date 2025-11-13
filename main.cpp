#include <iostream>
#include <string>
#include <stdexcept>
#include <cctype>
using namespace std;

enum class token_typ {
    number, plus, minus, multi, divison, mod,
    assign, iden, print,
    lparen, rparen, lbrace, rbrace,
    less, greater,
    If, Else, For,
    semi, end
};

struct token {
    token_typ type;
    string val;
};

class lexer {
public:
    string source;
    int posi;

    lexer(string s) {
        source = s;
        posi = 0;
    }

    token gettoken() {
        while (posi < source.size()) {
            char c = source[posi];

            if (isspace(c)) {
                posi++;
                continue;
            }

            if (isalpha(c)) {
                string id = "";
                while (posi < source.size() && isalnum(source[posi])) {
                    id += source[posi++];
                }
                if (id == "if") return {token_typ::If, id};
                if (id == "else") return {token_typ::Else, id};
                if (id == "for") return {token_typ::For, id};
                if (id == "print") return {token_typ::print, id};
                return {token_typ::iden, id};
            }

            if (isdigit(c) || c == '.') {
                string num = "";
                bool dot = false;
                while (posi < source.size() &&
                       (isdigit(source[posi]) || source[posi] == '.')) {
                    if (source[posi] == '.') {
                        if (dot) break;
                        dot = true;
                    }
                    num += source[posi++];
                }
                return {token_typ::number, num};
            }

            posi++;

            if (c == '+') return {token_typ::plus, "+"};
            if (c == '-') return {token_typ::minus, "-"};
            if (c == '*') return {token_typ::multi, "*"};
            if (c == '/') return {token_typ::divison, "/"};
            if (c == '%') return {token_typ::mod, "%"};
            if (c == '(') return {token_typ::lparen, "("};
            if (c == ')') return {token_typ::rparen, ")"};
            if (c == '{') return {token_typ::lbrace, "{"};
            if (c == '}') return {token_typ::rbrace, "}"};
            if (c == '=') return {token_typ::assign, "="};
            if (c == ';') return {token_typ::semi, ";"};
            if (c == '<') return {token_typ::less, "<"};
            if (c == '>') return {token_typ::greater, ">"};

            cout << "Unknown character: " << c << endl;
        }
        return {token_typ::end, ""};
    }
};

enum class nodetype {
    numbr, varibl, binop, assign,
    ifstm, forstm, blck, print
};

class node {
public:
    nodetype type;
    node(nodetype t) { type = t; }
    virtual ~node() {}
};

class numbnode : public node {
public:
    double n;
    numbnode(double x) : node(nodetype::numbr) { n = x; }
};

class varinode : public node {
public:
    string name;
    varinode(string n) : node(nodetype::varibl) { name = n; }
};

class binopnode : public node {
public:
    node *l;
    node *r;
    token op;

    binopnode(node *a, token o, node *b)
        : node(nodetype::binop), l(a), op(o), r(b) {}

    ~binopnode() { delete l, r; }
};

class assignnode : public node {
public:
    string name;
    node *expr;

    assignnode(string n, node *e)
        : node(nodetype::assign), name(n), expr(e) {}

    ~assignnode() { delete expr; }
};

class ifnode : public node {
public:
    node *cond;
    node *thenstm;
    node *elsstm;

    ifnode(node *c, node *t, node *e)
        : node(nodetype::ifstm), cond(c), thenstm(t), elsstm(e) {}

    ~ifnode() { delete cond, thenstm; if (elsstm) delete elsstm; }
};

class fornode : public node {
public:
    node *init;
    node *cond;
    node *upd;
    node *bdy;

    fornode(node *i, node *c, node *u, node *b)
        : node(nodetype::forstm), init(i), cond(c), upd(u), bdy(b) {}

    ~fornode() { delete init, cond, upd, bdy; }
};

class blocknode : public node {
public:
    node *arr[100];
    int cnt;

    blocknode() : node(nodetype::blck), cnt(0) {}

    void add(node *n) { arr[cnt++] = n; }

    ~blocknode() {
        for (int i = 0; i < cnt; i++) delete arr[i];
    }
};

class printnode : public node {
public:
    node *expr;
    printnode(node *e) : node(nodetype::print), expr(e) {}
    ~printnode() { delete expr; }
};

class Parser
{
private:
    Lexer &lexer;
    Token token;

    void eat(TokenType expected)
    {
        if (token.type == expected)
            token = lexer.getNextToken();
        else
            throw runtime_error("Unexpected token");
    }

    BaseNode *factor()
    {
        if (token.type == TokenType::number)
        {
            double val = stod(token.value);
            eat(TokenType::number);
            return new NumNode(val);
        }
        else if (token.type == TokenType::identifier)
        {
            string name = token.value;
            eat(TokenType::identifier);
            return new VarNode(name);
        }
        else if (token.type == TokenType::lparen)
        {
            eat(TokenType::lparen);
            BaseNode *node = expr();
            eat(TokenType::rparen);
            return node;
        }
        throw runtime_error("Invalid factor");
    }

    BaseNode *term()
    {
        BaseNode *node = factor();
        while (token.type == TokenType::mul || token.type == TokenType::div || token.type == TokenType::mod)
        {
            Token oper = token;
            if (oper.type == TokenType::mul)
                eat(TokenType::mul);
            else if (oper.type == TokenType::div)
                eat(TokenType::div);
            else
                eat(TokenType::mod);
            node = new BinOpNode(node, oper, factor());
        }
        return node;
    }

    BaseNode *expr()
    {
        BaseNode *node = term();
        while (token.type == TokenType::plus || token.type == TokenType::minus)
        {
            Token oper = token;
            if (oper.type == TokenType::plus)
                eat(TokenType::plus);
            else
                eat(TokenType::minus);
            node = new BinOpNode(node, oper, term());
        }
        return node;
    }

public:
    Parser(Lexer &lex) : lexer(lex) { token = lexer.getNextToken(); }

    BaseNode *statement()
    {
        if (token.type == TokenType::identifier)
        {
            string varName = token.value;
            eat(TokenType::identifier);
            if (token.type == TokenType::assign)
            {
                eat(TokenType::assign);
                BaseNode *rhs = expr();
                return new AssignNode(varName, rhs);
            }
            throw runtime_error("Expected '=' after variable");
        }
        return expr();
    }

    BaseNode *parse() { return statement(); }
};
class Value
{
public:
    double val;
    Value(double v = 0) : val(v) {}

    Value operator+(const Value &other) const { return Value(val + other.val); }
    Value operator-(const Value &other) const { return Value(val - other.val); }
    Value operator*(const Value &other) const { return Value(val * other.val); }
    Value operator/(const Value &other) const
    {
        if (other.val == 0){
            throw runtime_error("Division by zero");
        }
        return Value(val / other.val);
    }

    Value mod(const Value &other) const
    {
        if (other.val == 0){
            throw runtime_error("Modulo by zero");
        }
        if (val == (int)val && other.val == (int)other.val){
            return Value((int)val % (int)other.val);
        }
        else{
            throw runtime_error("Modulus only supported for integers");
        }
    }

    void display() const
    {
        if (val == (int)val){
            cout << (int)val;
        }
        else{
            cout << val;
        }
    }
};
class Interpreter
{
    map<string, Value> variables;

public:
    Value visit(BaseNode *node)
    {
        if (node->getType() == NodeType::number)
        {
            NumNode *n = static_cast<NumNode *>(node);
            return Value(n->getValue());
        }
        else if (node->getType() == NodeType::variable)
        {
            VarNode *v = static_cast<VarNode *>(node);
            if (variables.find(v->getName()) == variables.end())
                throw runtime_error("Undefined variable: " + v->getName());
            return variables[v->getName()];
        }
        else if (node->getType() == NodeType::assign)
        {
            AssignNode *a = static_cast<AssignNode *>(node);
            Value val = visit(a->getExpr());
            variables[a->getVarName()] = val;
            return val;
        }
        else if (node->getType() == NodeType::binop)
        {
            BinOpNode *b = static_cast<BinOpNode *>(node);
            Value left = visit(b->getLeft());
            Value right = visit(b->getRight());
            switch (b->getOp().type)
            {
            case TokenType::plus:
                return left + right;
            case TokenType::minus:
                return left - right;
            case TokenType::mul:
                return left * right;
            case TokenType::div:
                return left / right;
            case TokenType::mod:
                return left.mod(right);
            default:
                return Value(0);
            }
        }
        return Value(0);
    }
};

int main()
{
    Interpreter interp;
    while (true)
    {
        cout << ">>> ";
        string s;
        if (!getline(cin, s) || s.empty())
            break;

        Lexer lexer(s);
        Parser parser(lexer);
        BaseNode *tree = parser.parse();

        try
        {
            Value result = interp.visit(tree);
            cout << "Result = ";
            result.display();
            cout << "\n";
        }
        catch (exception &e)
        {
            cout << "Error: " << e.what() << "\n";
        }

        delete tree;
    }

    return 0;
}
