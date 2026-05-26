#include "analyzer.hpp"
#include "../overload.hpp"
#include "../panic.hpp"

using instruction_mod::OpCode;
using TT = instruction_mod::TokenType;

namespace analyzer_mod {

    inline Opt OPT(TT t) { return instruction_mod::TokenOpt{t}; }
    inline Opt OPT(std::initializer_list<TT> ts) { return instruction_mod::TokenOpt{ts}; }

    inline Fmt DefineFmt(Opt a, Opt b, Opt c) {
        return Fmt{ a, b, c };
    }

    //destination is (almost) always first
    inline Fmt Null() { return DefineFmt( std::nullopt, std::nullopt, std::nullopt ); }                                         // N
    inline Fmt Reg() { return DefineFmt( OPT(TT::Register), std::nullopt, std::nullopt ); }                                     // S
    inline Fmt RegAdr() { return DefineFmt( OPT(TT::Register), OPT({TT::Address, TT::Variable}), std::nullopt ); }              // M
    inline Fmt RegRgI() { return DefineFmt( OPT(TT::Register), OPT({TT::Register, TT::Immediate}), std::nullopt); }             // R/I
    inline Fmt RegRegRgI() { return DefineFmt( OPT(TT::Register), OPT(TT::Register), OPT({TT::Register, TT::Immediate}) ); }    // R/I
    inline Fmt Lbl() { return DefineFmt( OPT(TT::Label), std::nullopt, std::nullopt); }                                         // J

    inline const InstructionFmt instruction_fmt {

        //  memory and registers
        { OpCode::LOAD, RegAdr() },
        { OpCode::SEND, RegAdr() },
        { OpCode::SET,  RegRgI() },

        //  bitwise
        { OpCode::NOT,  RegRgI() },
        { OpCode::AND,  RegRegRgI() },
        { OpCode::OR,   RegRegRgI() },
        { OpCode::XOR,  RegRegRgI() },

        //  shift and rotation
        { OpCode::STL,  RegRegRgI() },
        { OpCode::STR,  RegRegRgI() },
        { OpCode::RTL,  RegRegRgI() },
        { OpCode::RTR,  RegRegRgI() },

        //  arithmetic
        { OpCode::NEG,  RegRgI() },
        { OpCode::ADD,  RegRegRgI() },
        { OpCode::SUB,  RegRegRgI() },
        { OpCode::MUL,  RegRegRgI() },
        { OpCode::DIV,  RegRegRgI() },
        { OpCode::MOD,  RegRegRgI() },

        // comparison and branching
        { OpCode::CMP,  RegRgI() },
        { OpCode::GOTO, Lbl() },
        { OpCode::WEQ,  Lbl() },
        { OpCode::WNE,  Lbl() },
        { OpCode::WLT,  Lbl() },
        { OpCode::WGT,  Lbl() },
        { OpCode::WCY,  Lbl() },
        { OpCode::WOV,  Lbl() },
        { OpCode::WDZ,  Lbl() },

        //  stacks and sub-routines
        { OpCode::CAL,  Lbl() },
        { OpCode::RET,  Null() },
        { OpCode::PUSH, Reg() },
        { OpCode::POP,  Reg() },

        //  misc. and debugging
        { OpCode::END,  Null() },
        { OpCode::CLR,  Null() },

        // i/o (VM only)
        { OpCode::PRINT,  Reg() },
        { OpCode::PRINTC, Reg() },

    };

    Analyzer::Analyzer() :
        priv_inst_count{0}
    { }

    //scouting labels as first pass
    std::expected<void, SemErr> Analyzer::scout_lbl(instruction_mod::Inst& inst, std::unordered_map<std::string, size_t>& label_table) { 
        //std::cout << priv_inst_count << "\n";       
        auto& first_token{inst.token_arr[0]};
        bool label_too_long {false};
        if (inst.used_size > 1) label_too_long = true;
        if (!first_token.has_value()) { //check if first token exists, should not happen ideally but still
            return std::unexpected(SemErr::MissingOpCodeError);
        } else {
            switch (first_token->token_type) {
                case TT::OpCode:
                    inst.inst_no = priv_inst_count++;
                    break;
                case TT::Label:
                    if (label_too_long) {
                        return std::unexpected(SemErr::LabelTooLongError); //if extra operands apart from just the label
                    } else {
                        return std::visit(overload{
                            [this, &inst, &label_table](std::string val) -> std::expected<void, SemErr> {
                                auto [_, result] = label_table.insert(std::make_pair(val, priv_inst_count * 4));
                                if (!result) {
                                    return std::unexpected(SemErr::LabelAlreadyExists);
                                }
                                inst.inst_type = instruction_mod::InstType::LBL;
                                return {};
                            },
                            [this](auto) -> std::expected<void, SemErr> {
                                panic::panic();
                                return std::unexpected(SemErr::UnknownSemanticError);
                            }
                        }, first_token->value);
                    }
                    break;
                    //add label to symbol table
                default: //ignore otherwise (for now)
                    return {};
            }
        }
        return {};
    }

    //validate specific tokens to see if they are within constraints, only checked for positions 2 to INSTSIZE
    std::expected<void, SemErr> Analyzer::validate_token(const instruction_mod::Token& token, const std::unordered_map<std::string, size_t>& label_table) const {
        return std::visit(overload{
            [&token](OpCode) -> std::expected<void, SemErr> { //shouldn't happen ideally
                return std::unexpected(SemErr::InvalidOpCodePosition);
            },
            [&token](int tkn_i) -> std::expected<void, SemErr> { 
                switch (token.token_type) {
                    case TT::Register:
                        if (tkn_i < 0 || tkn_i >= instruction_mod::Inst::MAX_REG_COUNT) {
                            return std::unexpected(SemErr::RegisterOutOfRange);
                        } else {
                            return {};
                        }
                    case TT::Immediate:
                        if (tkn_i < 0 || tkn_i > instruction_mod::Inst::MAX_IMM_VAL) {
                            return std::unexpected(SemErr::ImmediateOutOfRange);
                        } else {
                            return {};
                        }
                    case TT::Address:
                        if (tkn_i < 0 || tkn_i > instruction_mod::Inst::MAX_ADDR_VAL) {
                            return std::unexpected(SemErr::AddressOutOfRange);
                        } else {
                            return {};
                        }
                    default:
                        panic::panic(); //unreachable
                        return std::unexpected(SemErr::UnknownSemanticError);
                }
            },
            [this, &token, &label_table](std::string tkn_s) -> std::expected<void, SemErr> { 
                switch (token.token_type) {
                    case TT::Label:
                        if (label_table.contains(tkn_s)) {
                            return {};
                        }
                        return std::unexpected(SemErr::UndefinedLabelError);
                    case TT::Variable:
                        return {}; //no further verification required
                    default:
                        panic::panic(); //unreachable
                        return std::unexpected(SemErr::UnknownSemanticError);
                }    
            },
        }, token.value);
    }

    //validates opcodes and assigns them respective types accordingly
    std::expected<void, SemErr> Analyzer::validate_opcode(instruction_mod::Inst& inst, instruction_mod::OpCode opcode, const std::unordered_map<std::string, size_t>& label_table) const {

        const auto opc_pattern = instruction_fmt.find(opcode);
        if (opc_pattern == instruction_fmt.end()) {
            panic::panic(); //change this to a panic function with error message.
        } else { 
            for (int tkn{1}; tkn < instruction_mod::Inst::INST_SIZE; ++tkn) {

                const auto& cur_token = inst.token_arr[tkn];
                const auto& cur_target = (opc_pattern->second)[tkn-1];

                if (cur_token.has_value() != cur_target.has_value()) { // one is null and the other is not
                    return std::unexpected(SemErr::IncorrectOperandFmt);
                } else if (cur_token.has_value()) {
                    if (!cur_target->token_type_exists(cur_token->token_type)) {
                        return std::unexpected(SemErr::IncorrectOperandFmt);
                    } else {
                        const auto& tkn_val_res = validate_token(cur_token.value(), label_table);
                        if (!tkn_val_res) return tkn_val_res;
                    }
                }

            }
        }
        const auto cur_inst_type = instruction_mod::instruction_types.find(opcode);
        if (cur_inst_type == instruction_mod::instruction_types.end()) {
            return std::unexpected(SemErr::UnknownSemanticError);
        } else {
            inst.inst_type = cur_inst_type->second;
        }
        return {};
        
    }

    //analyzer
    std::expected<void, SemErr> Analyzer::analyze(instruction_mod::Inst& inst, const std::unordered_map<std::string, size_t>& label_table) const {
        const auto& first_token = inst.token_arr[0];
        if (!first_token.has_value()) { //check if first token exists, should not happen ideally but still
            return std::unexpected(SemErr::MissingOpCodeError);
        } else {
            switch(first_token->token_type) {
                case TT::OpCode: {
                    auto is_matched = std::visit(overload{
                        [this, &inst, &label_table](instruction_mod::OpCode oc) -> std::expected<void, SemErr> {
                            return validate_opcode(inst, oc, label_table);
                        },
                        [](auto a) -> std::expected<void, SemErr> { 
                            panic::panic(); 
                            return std::unexpected(SemErr::UnknownSemanticError); 
                        }, //shouldn't happen
                    }, first_token->value);
                    if (!is_matched) return is_matched;
                    //remaining validation
                    break;
                }
                case TT::Label: //intentionally empty, covered in scout
                    break;
                default:
                    return std::unexpected(SemErr::IncorrectFirstToken);
                    break;
            }
        }
        return {};
    };

    //classifying RI instructions as R and I
    void Analyzer::RI_inst_classify(instruction_mod::Inst& inst) const {
        if (inst.inst_type == instruction_mod::InstType::RI) {
            if (inst.used_size == 3) {
                const auto& last_token_type = inst.token_arr[2].value().token_type;
                if (last_token_type == instruction_mod::TokenType::Register) inst.inst_type = instruction_mod::InstType::R;
                else if (last_token_type == instruction_mod::TokenType::Immediate) inst.inst_type = instruction_mod::InstType::I;
                else panic::panic();
            } else if (inst.used_size == 4) {
                const auto& last_token_type = inst.token_arr[3].value().token_type;
                if (last_token_type == instruction_mod::TokenType::Register) inst.inst_type = instruction_mod::InstType::R;
                else if (last_token_type == instruction_mod::TokenType::Immediate) inst.inst_type = instruction_mod::InstType::I;
                else panic::panic();
            } else { panic::panic(); /*shouldn't happen*/ }
        }
    }

}