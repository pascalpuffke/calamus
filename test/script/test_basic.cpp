#include <gtest/gtest.h>
#include <script_legacy/vm.h>

using namespace calamus::script;

// These tests are *extremely* bare bones and do NOT test the full capabilities of the scripting engine.
// They are just meant to verify that it isn't *totally* broken. Testing scripts should be done externally.

TEST(Script, InterpreterReturnsOk) {
    VM::init_vm();
    const auto source = std::string_view { "" };
    const auto result = VM::interpret(source);
    VM::free_vm();

    ASSERT_EQ(result, VM::InterpretResult::Ok);
}

TEST(Script, CompileError) {
    VM::init_vm();
    const auto source = std::string_view { R"(
        this should not compile
    )"};
    const auto result = VM::interpret(source);
    VM::free_vm();

    ASSERT_EQ(result, VM::InterpretResult::CompileError);
}

TEST(Script, RuntimeError) {
    VM::init_vm();
    const auto source = std::string_view { R"(
        var some_string = "";
        var some_number = 42;
        some_string + some_number;
        // This compiles fine, but adding a number to a string is illegal.
        // The compiler, as dumb as it is, doesn't know that and happily emits one addition op.
    )"};
    const auto result = VM::interpret(source);
    VM::free_vm();

    ASSERT_EQ(result, VM::InterpretResult::RuntimeError);
}
