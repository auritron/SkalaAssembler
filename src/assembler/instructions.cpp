#include "instructions.hpp"
#include "../overload.hpp"

namespace instruction_mod {

    const std::unordered_map<std::string_view, OpCode> instruction_map {
        { "LOAD",   OpCode::LOAD },
        { "SEND",   OpCode::SEND },
        { "SET",    OpCode::SET },
        { "NOT",    OpCode::NOT },
        { "AND",    OpCode::AND },
        { "OR",     OpCode::OR },
        { "XOR",    OpCode::XOR },
        { "STL",    OpCode::STL },
        { "STR",    OpCode::STR },
        { "RTL",    OpCode::RTL },
        { "RTR",    OpCode::RTR },
        { "ADD",    OpCode::ADD },
        { "SUB",    OpCode::SUB },
        { "MUL",    OpCode::MUL },
        { "DIV",    OpCode::DIV },
        { "MOD",    OpCode::MOD },
        { "NEG",    OpCode::NEG },
        { "CMP",    OpCode::CMP },
        { "GOTO",   OpCode::GOTO },
        { "WEQ",    OpCode::WEQ },
        { "WNE",    OpCode::WNE },
        { "WGT",    OpCode::WGT },
        { "WLT",    OpCode::WLT },
        { "WCY",    OpCode::WCY },
        { "WOV",    OpCode::WOV },
        { "WDZ",    OpCode::WDZ },
        { "CAL",    OpCode::CAL },
        { "RET",    OpCode::RET },
        { "PUSH",   OpCode::PUSH },
        { "POP",    OpCode::POP },
        { "PRINT",  OpCode::PRINT },
        { "PRINTC", OpCode::PRINTC },
        { "END",    OpCode::END },
        { "CLR",    OpCode::CLR },
    };

    const std::unordered_map<OpCode, InstType> instruction_types {
        // memory
        { OpCode::LOAD,   InstType::M  },
        { OpCode::SEND,   InstType::M  },

        // RI (either R or I, determined by analyzer)
        { OpCode::SET,    InstType::RI },
        { OpCode::NOT,    InstType::RI },
        { OpCode::AND,    InstType::RI },
        { OpCode::OR,     InstType::RI },
        { OpCode::XOR,    InstType::RI },
        { OpCode::STL,    InstType::RI },
        { OpCode::STR,    InstType::RI },
        { OpCode::RTL,    InstType::RI },
        { OpCode::RTR,    InstType::RI },
        { OpCode::NEG,    InstType::RI },
        { OpCode::ADD,    InstType::RI },
        { OpCode::SUB,    InstType::RI },
        { OpCode::MUL,    InstType::RI },
        { OpCode::DIV,    InstType::RI },
        { OpCode::MOD,    InstType::RI },
        { OpCode::CMP,    InstType::RI },

        // jump/branch
        { OpCode::GOTO,   InstType::J  },
        { OpCode::WEQ,    InstType::J  },
        { OpCode::WNE,    InstType::J  },
        { OpCode::WGT,    InstType::J  },
        { OpCode::WLT,    InstType::J  },
        { OpCode::WCY,    InstType::J  },
        { OpCode::WOV,    InstType::J  },
        { OpCode::WDZ,    InstType::J  },
        { OpCode::CAL,    InstType::J  },

        // single register
        { OpCode::PUSH,   InstType::S  },
        { OpCode::POP,    InstType::S  },
        { OpCode::PRINT,  InstType::S  },
        { OpCode::PRINTC, InstType::S  },

        // no operands
        { OpCode::RET,    InstType::N  },
        { OpCode::END,    InstType::N  },
        { OpCode::CLR,    InstType::N  },
    };

    Token::Token() : 
        token_type{TokenType::Invalid}, value{0}, file_line{0}, file_col{0} { }
    Token::Token(TokenType type, OpCode val, size_t f_line, size_t f_col) :
        token_type{type}, value{val}, file_line{f_line}, file_col{f_col} { }
    Token::Token(TokenType type, int val, size_t f_line, size_t f_col) :
        token_type{type}, value{val}, file_line{f_line}, file_col{f_col} { }
    Token::Token(TokenType type, std::string val, size_t f_line, size_t f_col) :
        token_type{type}, value{val}, file_line{f_line}, file_col{f_col} { }

    TokenOpt::TokenOpt(TokenType token) :
        token_opt_list(std::move(token))
    { }

    TokenOpt::TokenOpt(std::initializer_list<TokenType> tokens) :
        token_opt_list(std::unordered_set<TokenType>(tokens))
    { }

    bool TokenOpt::token_type_exists(TokenType target) const {
        return std::visit(overload {
            [target](TokenType token) { return token == target; },
            [target](const std::unordered_set<TokenType>& tokens) { return tokens.contains(target); },
        }, token_opt_list);
    }

    Inst::Inst() :
        token_arr{std::nullopt},
        used_size{0},
        inst_no{std::nullopt},
        inst_type{InstType::INV}
    { }

    bool Inst::push_token(Token&& token) {
        if (used_size < INST_SIZE) {
            token_arr[used_size] = std::move(token);
            ++used_size;
            return true;
        }
        return false;
    }

}