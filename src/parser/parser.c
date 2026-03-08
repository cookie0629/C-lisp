#include "parser.h"
#include "../runtime/gc.h"
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
    #define strcasecmp _stricmp
    #define strdup _strdup
#endif

/* --- Lexer Definitions --- */

typedef enum {
    TOK_LPAREN,     // (
    TOK_RPAREN,     // )
    TOK_INT,        // 123
    TOK_FLOAT,      // 3.14
    TOK_SYMBOL,     // abc, +, setq
    TOK_STRING,     // "hello"
    TOK_QUOTE,      // '
    TOK_EOF,
    TOK_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char* text;     // Dynamic buffer for symbols/strings
    long long int_val;
    double float_val;
} Token;

// Global parser state (Not thread-safe, okay for Stage 1)
static const char* src;
static int pos;

/* --- Lexer Implementation --- */

static char peek() {
    return src[pos];
}

static char advance() {
    return src[pos++];
}

static bool is_symbol_char(char c) {
    // Allow alphanumeric and special Lisp characters
    return isalnum(c) || strchr("+-*/<>=!@._?", c) != NULL;
}

static void skip_whitespace() {
    while (peek() != '\0') {
        char c = peek();
        if (isspace(c)) {
            advance();
        } else if (c == ';') {
            // Comment until end of line
            while (peek() != '\0' && peek() != '\n') advance();
        } else {
            break;
        }
    }
}

static Token get_token() {
    skip_whitespace();
    char c = peek();
    Token token;
    token.text = NULL;

    if (c == '\0') {
        token.type = TOK_EOF;
        return token;
    }
    if (c == '(') {
        advance();
        token.type = TOK_LPAREN;
        return token;
    }
    if (c == ')') {
        advance();
        token.type = TOK_RPAREN;
        return token;
    }
    if (c == '\'') {
        advance();
        token.type = TOK_QUOTE;
        return token;
    }

    // String Literal
    if (c == '"') {
        advance(); // Skip opening quote
        int capacity = 128;
        token.text = malloc(capacity);
        int len = 0;
        while (peek() != '\0') {
                    if (peek() == '\\' && src[pos+1] == '"') {
                        advance();
                        token.text[len++] = advance();
                    } else if (peek() == '"') {
                        break; 
                    } else {
                        token.text[len++] = advance();
                    }
                    
                    if (len >= capacity - 2) {
                        capacity *= 2;
                        token.text = realloc(token.text, capacity);
                    }
                }
                
        token.text[len] = '\0';
                
        if (peek() == '"') advance(); // Skip closing quote
        token.type = TOK_STRING;
        return token;
    }

    // Number (Integer or Float)
    if (isdigit(c) || (c == '-' && isdigit(src[pos+1]))) {
        int start = pos;
        advance();
        bool is_float = false;
        while (isdigit(peek()) || peek() == '.') {
            if (peek() == '.') is_float = true;
            advance();
        }
        int len = pos - start;
        char* buf = malloc(len + 1);
        strncpy(buf, src + start, len);
        buf[len] = '\0';

        if (is_float) {
            token.type = TOK_FLOAT;
            token.float_val = atof(buf);
        } else {
            token.type = TOK_INT;
            token.int_val = atoll(buf);
        }
        free(buf);
        return token;
    }

    // Symbol
    if (is_symbol_char(c)) {
        int start = pos;
        while (is_symbol_char(peek())) {
            advance();
        }
        int len = pos - start;
        token.text = malloc(len + 1);
        strncpy(token.text, src + start, len);
        token.text[len] = '\0';
        token.type = TOK_SYMBOL;
        return token;
    }

    token.type = TOK_ERROR;
    return token;
}

/* --- Parser Implementation --- */

static LispVal* parse_expression();

// Parse a list recursively: (expr ...)
static LispVal* parse_list() {
    // Check for empty list ')'
    int saved_pos = pos;
    skip_whitespace();

    if (peek() == ')') {
        advance();
        return lisp_alloc(LISP_NIL);
    }

    pos = saved_pos; // Restore position

    LispVal* head = parse_expression();
    GC_PUSH(head);

    LispVal* tail = parse_list(); 
    GC_PUSH(tail);

    LispVal* result = lisp_cons(head, tail);

    GC_POP(); //tail
    GC_POP(); //head

    return result;
}

static LispVal* parse_quote() {
    LispVal* expr = parse_expression();
    GC_PUSH(expr);

    LispVal* quote_sym = lisp_symbol("quote");
    GC_PUSH(quote_sym);

    LispVal* nil = lisp_alloc(LISP_NIL);
    GC_PUSH(nil);

    LispVal* inner = lisp_cons(expr, nil);
    GC_PUSH(inner);

    LispVal* result = lisp_cons(quote_sym, inner);

    GC_POP(); // inner
    GC_POP(); // nil
    GC_POP(); // quote_sym
    GC_POP(); // expr

    return result;
}

static LispVal* parse_expression() {
    Token token = get_token();

    switch (token.type) {
        case TOK_INT:
            return lisp_int(token.int_val);
        case TOK_FLOAT:
            return lisp_float(token.float_val);
        case TOK_STRING: {
            LispVal* val = lisp_string(token.text);
            free(token.text); // lisp_string made a copy
            return val;
        }
        case TOK_SYMBOL: {
            // Handle 'nil' and 't' specially
            if (strcasecmp(token.text, "nil") == 0) {
                free(token.text);
                return lisp_alloc(LISP_NIL);
            }
            if (strcasecmp(token.text, "t") == 0) {
                free(token.text);
                return lisp_alloc(LISP_T);
            }
            LispVal* val = lisp_symbol(token.text);
            free(token.text);
            return val;
        }
        case TOK_LPAREN:
            return parse_list();
        case TOK_QUOTE:
            return parse_quote();
        case TOK_EOF:
            return NULL;
        default:
            fprintf(stderr, "Syntax Error: Unexpected token type %d at pos %d\n", token.type, pos);
            exit(1);
    }
}

/* --- Public API --- */

LispVal* lisp_parse(const char* input) {
    src = input;
    pos = 0;

    LispVal* root = lisp_alloc(LISP_NIL);
    GC_PUSH(root);

    LispVal* tail = NULL;

    while (true) {
        skip_whitespace();
        if (peek() == '\0') break;

        LispVal* expr = parse_expression();
        if (!expr) break;

        GC_PUSH(expr);

        LispVal* nil = lisp_alloc(LISP_NIL);
        GC_PUSH(nil);

        LispVal* new_node = lisp_cons(expr, nil);
        GC_PUSH(new_node);

        if (root->type == LISP_NIL) {
            root = new_node;
            tail = root;
        } else {
            tail->data.cons.cdr = new_node;
            tail = new_node;
        }

        GC_POP(); // new_node
        GC_POP(); // nil
        GC_POP(); // expr
    }

    GC_POP(); // root

    return root;
}

LispVal* lisp_parse_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(length + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }
    
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);

    LispVal* result = lisp_parse(buffer);
    free(buffer);
    return result;
}