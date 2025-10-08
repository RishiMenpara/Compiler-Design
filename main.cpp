#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>
using namespace std;

enum class tokentype
{
    number,
    identifier,
    plus,
    minus,
    mul,
    divi,
    assign,
    lparen,
    rparen,
    eof
};

struct token
{
    tokentype type;
    string value;
};

class lexer
{
    string src;
    int pos;

public:
    lexer(const string &s) : src(s), pos(0) {}

    token getnext()
    {
        while (pos < (int)src.size())
        {
            char c = src[pos];
            if (isspace(c))
            {
                pos++;
                continue;
            }
            if (isdigit(c))
            {
                string num;
                while (pos < (int)src.size() && isdigit(src[pos]))
                    num.push_back(src[pos++]);
                return {tokentype::number, num};
            }
            if (isalpha(c))
            {
                string id;
                while (pos < (int)src.size() && isalnum(src[pos]))
                    id.push_back(src[pos++]);
                return {tokentype::identifier, id};
            }
            pos++;
            switch (c)
            {
            case '+':
                return {tokentype::plus, "+"};
            case '-':
                return {tokentype::minus, "-"};
            case '*':
                return {tokentype::mul, "*"};
            case '/':
                return {tokentype::divi, "/"};
            case '(':
                return {tokentype::lparen, "("};
            case ')':
                return {tokentype::rparen, ")"};
            case '=':
                return {tokentype::assign, "="};
            default:
                throw runtime_error("invalid char");
            }
        }
        return {tokentype::eof, ""};
    }
};

enum class nodetype
{
    number,
    binop,
    var,
    assign
};

class basenode
{
protected:
    nodetype type;

public:
    basenode(nodetype t) : type(t) {}
    nodetype gettype() const { return type; }
    virtual ~basenode() = default;
};

class numnode : public basenode
{
    int val;

public:
    numnode(int v) : basenode(nodetype::number), val(v) {}
    int getval() const { return val; }
};

class varnode : public basenode
{
    string name;

public:
    varnode(const string &n) : basenode(nodetype::var), name(n) {}
    string getname() const { return name; }
};

class binopnode : public basenode
{
    basenode *left;
    token op;
    basenode *right;

public:
    binopnode(basenode *l, token o, basenode *r)
        : basenode(nodetype::binop), left(l), op(o), right(r) {}
    basenode *getleft() const { return left; }
    basenode *getright() const { return right; }
    token getop() const { return op; }
    ~binopnode()
    {
        delete left;
        delete right;
    }
};

class assignnode : public basenode
{
    string name;
    basenode *expr;

public:
    assignnode(const string &n, basenode *e)
        : basenode(nodetype::assign), name(n), expr(e) {}
    string getname() const { return name; }
    basenode *getexpr() const { return expr; }
    ~assignnode() { delete expr; }
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
    }

    BaseNode *factor()
    {
        if (token.type == TokenType::number)
        {
            int val = stoi(token.value);
            eat(TokenType::number);
            return new NumNode(val);
        }
        else if (token.type == TokenType::lparen)
        {
            eat(TokenType::lparen);
            BaseNode *node = expr();
            eat(TokenType::rparen);
            return node;
        }
        return nullptr;
    }

    BaseNode *term()
    {
        BaseNode *node = factor();
        while (token.type == TokenType::mul || token.type == TokenType::div)
        {
            Token oper = token;
            if (oper.type == TokenType::mul)
                eat(TokenType::mul);
            else
                eat(TokenType::div);
            node = new binopNode(node, oper, factor());
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
            node = new binopNode(node, oper, term());
        }
        return node;
    }

public:
    Parser(Lexer &lex) : lexer(lex) { token = lexer.getNextToken(); }
    BaseNode *parse() { return expr(); }
};

class Value
{
public:
    int val;
    Value(int v = 0) : val(v) {}
    inline Value operator+(const Value &other) const { return Value(val + other.val); }
    inline Value operator-(const Value &other) const { return Value(val - other.val); }
    inline Value operator*(const Value &other) const { return Value(val * other.val); }
    inline Value operator/(const Value &other) const
    {
        if (other.val == 0)
            throw runtime_error("division by zero");
        return Value(val / other.val);
    }
    friend class Interpreter;
};

class Interpreter
{
public:
    Value visit(BaseNode *node)
    {
        if (node->getType() == NodeType::number)
        {
            NumNode *n = (NumNode *)node;
            return Value(n->getValue());
        }
        else if (node->getType() == NodeType::binop)
        {
            binopNode *b = (binopNode *)node;
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
            default:
                return Value(0);
            }
        }
        return Value(0);
    }
};

int main()
{
    string s;
    cout << "Enter expression: ";
    getline(cin, s);

    Lexer lexer(s);
    Parser parser(lexer);
    BaseNode *tree = parser.parse();

    Interpreter interp;
    try
    {
        Value result = interp.visit(tree);
        cout << "Result = " << result.val << endl;
    }
    catch (exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    delete tree;
    return 0;
}
