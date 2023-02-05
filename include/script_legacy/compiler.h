#pragma once

#include <script_legacy/chunk.h>
#include <script_legacy/object.h>
#include <script_legacy/scanner.h>
#include <string_view>

namespace calamus::script {

void mark_compiler_roots();

enum class FunctionType {
    Function,
    Method,
    Initializer,
    Script
};

class Compiler final {
public:
    Compiler() = default;
    ~Compiler() = default;

    ObjFunction* compile(std::string_view source);

private:
    /*
    void statement();
    void expression_statement();
    void for_statement();
    void if_statement();
    void print_statement();
    void return_statement();
    void while_statement();

    void declaration();
    void class_declaration();
    void fun_declaration();
    void var_declaration();

    void and_();
    void block();
    void expression();
    void function(FunctionType);
    void method();

    u8 argument_list();
    u8 parse_variable(std::string_view error_message);

    void add_local(Token);

    void declare_variable();
    void define_variable(u8 global);
    void mark_initialized();
    void synchronize();
    */
};

}
