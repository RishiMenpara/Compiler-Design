#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

enum class TokenType
{
    NUMBER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LPAREN,
    RPAREN,
    END_OF_FILE
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
                return {TokenType::NUMBER, num};
            }

            pos++;
            switch (current)
            {
            case '+':
                return {TokenType::PLUS, "+"};
            case '-':
                return {TokenType::MINUS, "-"};
            case '*':
                return {TokenType::MUL, "*"};
            case '/':
                return {TokenType::DIV, "/"};
            case '(':
                return {TokenType::LPAREN, "("};
            case ')':
                return {TokenType::RPAREN, ")"};
            default:
                cerr << "Unexpected character: " << current << endl;
            }
        }
        return {TokenType::END_OF_FILE, ""};
    }
};

enum class NodeType
{
    NUMBER,
    BINOP
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
    NumNode(int v) : BaseNode(NodeType::NUMBER), value(v) {}
    inline int getValue() const { return value; }
    friend class Interpreter;
};

class BinOpNode : public BaseNode
{
    BaseNode *left;
    Token op;
    BaseNode *right;

public:
    BinOpNode(BaseNode *l, Token o, BaseNode *r) : BaseNode(NodeType::BINOP), left(l), op(o), right(r) {}
    inline BaseNode *getLeft() const { return left; }
    inline BaseNode *getRight() const { return right; }
    inline Token getOp() const { return op; }
    ~BinOpNode()
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
        if (token.type == TokenType::NUMBER)
        {
            int val = stoi(token.value);
            eat(TokenType::NUMBER);
            return new NumNode(val);
        }
        else if (token.type == TokenType::LPAREN)
        {
            eat(TokenType::LPAREN);
            BaseNode *node = expr();
            eat(TokenType::RPAREN);
            return node;
        }
        return nullptr;
    }

    BaseNode *term()
    {
        BaseNode *node = factor();
        while (token.type == TokenType::MUL || token.type == TokenType::DIV)
        {
            Token oper = token;
            if (oper.type == TokenType::MUL)
                eat(TokenType::MUL);
            else
                eat(TokenType::DIV);
            node = new BinOpNode(node, oper, factor());
        }
        return node;
    }

    BaseNode *expr()
    {
        BaseNode *node = term();
        while (token.type == TokenType::PLUS || token.type == TokenType::MINUS)
        {
            Token oper = token;
            if (oper.type == TokenType::PLUS)
                eat(TokenType::PLUS);
            else
                eat(TokenType::MINUS);
            node = new BinOpNode(node, oper, term());
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
            throw runtime_error("Division by zero");
        return Value(val / other.val);
    }
    friend class Interpreter;
};

class Interpreter
{
public:
    Value visit(BaseNode *node)
    {
        if (node->getType() == NodeType::NUMBER)
        {
            NumNode *n = (NumNode *)node;
            return Value(n->getValue());
        }
        else if (node->getType() == NodeType::BINOP)
        {
            BinOpNode *b = (BinOpNode *)node;
            Value left = visit(b->getLeft());
            Value right = visit(b->getRight());
            switch (b->getOp().type)
            {
            case TokenType::PLUS:
                return left + right;
            case TokenType::MINUS:
                return left - right;
            case TokenType::MUL:
                return left * right;
            case TokenType::DIV:
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
