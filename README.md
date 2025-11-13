# Mini Compiler – Team Contributions & Architecture

This project implements a simple **Mini Compiler** consisting of a Lexer, Parser, AST Builder, Interpreter, and Runtime Environment.  
Below is a clear breakdown of each team member’s contributions and the system architecture.

---

## 🧩 Project Overview

The Mini Compiler supports:

- Arithmetic expressions  
- Variable assignments  
- If–Else statements  
- For-loops  
- Blocks `{ }`  
- Print statements  
- Symbol table with runtime evaluation  
- AST-based interpretation  

---

# 👥 Team Contributions

---

## **Rishi Menpara (202411062) – Lexer Class**

### **Key Contributions**
- Developed the complete **Lexer** responsible for converting raw source code into tokens.
- Implemented token recognition for:
  - Numbers  
  - Identifiers  
  - Operators `+ - * / %`  
  - Comparison operators `< >`  
  - Keywords: `if`, `else`, `for`, `print`
  - Parentheses `()`, Braces `{}`, Semicolons `;`
- Implemented identifier & keyword detection using character-by-character scanning.
- Implemented number parsing including support for floating-point values.
- Added whitespace skipping and error reporting for invalid characters.
- Ensured proper token sequencing to integrate correctly with the Parser.
- Debugged and stabilized the lexical scanning logic.

---

## **Harpil Patel (202411070) – Parser Class**

### **Key Contributions**
- Implemented a complete **Recursive-Descent Parser**.
- Developed parsing functions:
  - `factor()`
  - `term()`
  - `expr()`
  - `statement()`
  - `parse()`
- Added grammar support for:
  - Arithmetic expressions  
  - Assignments  
  - If–Else  
  - For-loops  
  - Blocks `{ }`  
  - Print statements  
- Built AST node creation for all constructs.
- Implemented exception-based syntax error handling.
- Ensured tight integration between Parser and Lexer.

---

## **Tirth Manavadariya (202411075) – Value Class, Variable Table, Main Program**

### **Key Contributions**
- Implemented the **Value** class supporting runtime arithmetic:
  - Addition, subtraction  
  - Multiplication, division (with zero-division check)  
  - Modulo  
- Built the **Variable Table (VarTable)** with:
  - Symbol storage  
  - Lookup  
  - Assignment and updating  
  - Undefined-variable error handling  
- Wrote the `main()` program:
  - Input loop  
  - Code buffering  
  - Lexer/Parser initialization  
  - AST execution  
  - Error reporting  
- Connected all modules together for a smooth interactive execution flow.

---

## **Jinit Ponkiya (202411075) – Interpreter Class**

### **Key Contributions**
- Implemented the **Interpreter** using AST traversal.
- Added evaluation for:
  - Numeric nodes  
  - Variable nodes  
  - Arithmetic operations  
  - Comparison operations `< >`  
  - Assignments  
  - If–Else execution  
  - For-loop execution  
  - Block execution  
  - Print statements  
- Developed `visit()`-based recursive evaluation.
- Added runtime error handling.
- Ensured correct execution order even in deeply nested structures.

---

# 🏗️ System Architecture

### **1. Lexer**
Converts raw source code into stream of tokens.

### **2. Parser**
Consumes tokens and builds an Abstract Syntax Tree (AST).

### **3. AST Nodes**
Represent statements and expressions.

### **4. Interpreter**
Traverses AST and executes nodes.

### **5. Runtime**
Manages variables, values, and error handling.

---
