#pragma once

#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <initializer_list>
#include <cstdint>

namespace instruction_mod {

    enum class TokenType {
        Invalid, //debugging purposes, should not ideally be pushed
        OpCode,
        Variable,
        Register,
        Immediate,
        Label,
        Address,
    };

    enum class OpCode {

        //  memory and registers
        LOAD,
        SEND,
        SET,

        //  bitwise
        NOT,
        AND,
        OR,
        XOR,

        //  shift and rotation
        STL,
        STR,
        RTL,
        RTR,

        //  arithmetic
        NEG,
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,

        //  comparison and branching
        CMP,
        GOTO,
        WEQ,
        WNE,
        WGT,
        WLT,
        WCY,
        WOV,
        WDZ,

        //  stacks and sub-routines
        CAL,
        RET,
        PUSH,
        POP,

        //  i/o
        PRINT,
        PRINTC,

        //  misc. and debugging
        END,
        CLR,

    };

    enum class InstType {
        R,
        I,
        RI, //either R or I, to be determined by analyzer
        M,
        S,
        J,
        N,
        LBL, //labels skipped by codegen
        INV, //invalid
    };

    extern const std::unordered_map<std::string_view, OpCode> instruction_map; //defined in instruction.cpp
    
    // Opcode : Instruction Type
    extern const std::unordered_map<OpCode, InstType> instruction_types; //defined in instruction.cpp

    class Token {

        public:

            TokenType token_type;

            using TokenDataType = std::variant<OpCode, int, std::string>;
            TokenDataType value;

            Token();
            Token(TokenType type, OpCode val);
            Token(TokenType type, int val);
            Token(TokenType type, std::string val);
            
    };

    class TokenOpt {

        private:
            
            std::variant<TokenType, std::unordered_set<TokenType>> token_opt_list;

        public:

            TokenOpt(TokenType token);
            TokenOpt(std::initializer_list<TokenType> tokens);
            bool token_type_exists(TokenType target) const;

    };

    class Inst {

        public:

            static constexpr size_t INST_SIZE = 4;
            static constexpr size_t MAX_REG_COUNT = 16;
            static constexpr int MAX_ADDR_VAL = 65535;
            static constexpr int MAX_IMM_VAL = 65535;
            size_t used_size;
            InstType inst_type;
            std::array<std::optional<Token>, INST_SIZE> token_arr;

            Inst();
            bool push_token(Token&& token);

    };

    using Pipeline = std::vector<Inst>;

}
