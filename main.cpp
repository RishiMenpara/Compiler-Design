#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

enum class TokenType
{
    number,
    plus,
    minus,
    mul,
    div,
    lparen,
    rparen,
    end_of_file
};

struct Token
{
    TokenType type;
    string value;
};

class Lexer
{
private:
    string src;
    int pos;

public:
    Lexer(const string &source) : src(source), pos(0) {}

    Token getNextToken()
    {
        while (pos < (int)src.size())
        {
            char current = src[pos];
            if (isspace(current))
            {
                pos++;
                continue;
            }

            if (isdigit(current))
            {
                string num;
                while (pos < (int)src.size() && isdigit(src[pos]))
                    num.push_back(src[pos++]);
                return {TokenType::number, num};
            }

            pos++;
            switch (current)
            {
            case '+':
                return {TokenType::plus, "+"};
            case '-':
                return {TokenType::minus, "-"};
            case '*':
                return {TokenType::mul, "*"};
            case '/':
                return {TokenType::div, "/"};
            case '(':
                return {TokenType::lparen, "("};
            case ')':
                return {TokenType::rparen, ")"};
            default:
                cerr << "Unexpected character: " << current << endl;
            }
        }
        return {TokenType::end_of_file, ""};
    }
};

enum class NodeType
{
    number,
    binop
};

class BaseNode
{
protected:
    NodeType nodetype;

public:
    BaseNode(NodeType type) : nodetype(type) {}
    inline NodeType getType() const { return nodetype; }
    friend class Interpreter;
};

class NumNode : public BaseNode
{
    int value;

public:
    NumNode(int v) : BaseNode(NodeType::number), value(v) {}
    inline int getValue() const { return value; }
    friend class Interpreter;
};

class binopNode : public BaseNode
{
    BaseNode *left;
    Token op;
    BaseNode *right;

public:
    binopNode(BaseNode *l, Token o, BaseNode *r) : BaseNode(NodeType::binop), left(l), op(o), right(r) {}
    inline BaseNode *getLeft() const { return left; }
    inline BaseNode *getRight() const { return right; }
    inline Token getOp() const { return op; }
    ~binopNode()
    {
        delete left;
        delete right;
    }
    friend class Interpreter;
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
