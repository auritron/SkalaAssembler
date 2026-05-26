#include "codegen.hpp"
#include "../overload.hpp"
#include "../panic.hpp"

namespace codegen_mod {

    using instruction_mod::OpCode;
    using instruction_mod::TokenType;
    using instruction_mod::InstType;

    const std::unordered_map<OpCode, uint8_t> opcode_mapping {
        // R/I-types
        { OpCode::NOT,    0b00000 },
        { OpCode::AND,    0b00001 },
        { OpCode::OR,     0b00010 },
        { OpCode::XOR,    0b00011 },
        { OpCode::STL,    0b00100 },
        { OpCode::STR,    0b00101 },
        { OpCode::RTL,    0b00110 },
        { OpCode::RTR,    0b00111 },
        { OpCode::ADD,    0b01000 },
        { OpCode::SUB,    0b01001 },
        { OpCode::NEG,    0b01011 },
        { OpCode::MUL,    0b01100 },
        { OpCode::DIV,    0b01110 },
        { OpCode::MOD,    0b01111 },
        { OpCode::SET,    0b10000 },
        { OpCode::CMP,    0b10001 },

        // M-types
        { OpCode::LOAD,   0b00000 },
        { OpCode::SEND,   0b00001 },

        // S-types
        { OpCode::PUSH,   0b00000 },
        { OpCode::POP,    0b00001 },
        { OpCode::PRINT,  0b10000 },
        { OpCode::PRINTC, 0b10001 },

        // J-types
        { OpCode::WEQ,    0b00000 },
        { OpCode::WNE,    0b00001 },
        { OpCode::WGT,    0b00010 },
        { OpCode::WLT,    0b00011 },
        { OpCode::WCY,    0b00100 },
        { OpCode::WOV,    0b00101 },
        { OpCode::WDZ,    0b00110 },
        { OpCode::GOTO,   0b01000 },
        { OpCode::CAL,    0b01001 },

        // N-types
        { OpCode::RET,    0b00000 },
        { OpCode::END,    0b11111 },
        { OpCode::CLR,    0b10000 },
    };

    inline constexpr uint8_t FMT_R = 0b000;
    inline constexpr uint8_t FMT_I = 0b001;
    inline constexpr uint8_t FMT_M = 0b010;
    inline constexpr uint8_t FMT_J = 0b011;
    inline constexpr uint8_t FMT_S = 0b100;
    inline constexpr uint8_t FMT_N = 0b101;

    CodeGen::CodeGen() :
        next_unused_adr{instruction_mod::Inst::ADR_OFFSET},
        var_table{}
    { }

    uint8_t CodeGen::encode_tkn_u8(const instruction_mod::Token& token) {
        switch (token.token_type) {
            case TokenType::OpCode:
                return std::visit(overload{
                    [](OpCode val) {
                        auto map_res = opcode_mapping.find(val);
                        if (map_res == opcode_mapping.end()) { 
                            panic::panic();
                            return static_cast<std::uint8_t>(0);
                        } else {
                            return map_res->second;
                        } 
                    },
                    [](auto) { 
                        panic::panic();
                        return static_cast<std::uint8_t>(0);
                    }
                }, token.value);
            case TokenType::Register:
                return std::visit(overload{
                    [](int val) { return static_cast<std::uint8_t>(val); },
                    [](auto) {
                        panic::panic(); 
                        return static_cast<std::uint8_t>(0);
                    }
                }, token.value);
            default:
                panic::panic();
                return 0;
        }
        return 0;
    }
    
    uint16_t CodeGen::encode_tkn_u16(const instruction_mod::Token& token, const std::unordered_map<std::string, size_t>& lbl_table) {
        switch (token.token_type) {
            case TokenType::Address:
                return std::visit(overload{
                    [](int val) { return static_cast<std::uint16_t>(val); },
                    [](auto) {
                        panic::panic();
                        return static_cast<std::uint16_t>(0);
                    }
                }, token.value);
            case TokenType::Variable:
                return std::visit(overload{
                    [this](std::string val) {
                        auto var_res = var_table.find(val);
                        if (var_res == var_table.end()) { 
                            var_table.insert({val, next_unused_adr});
                            next_unused_adr += 2;
                            return static_cast<uint16_t>(next_unused_adr - 2);
                        } else {
                            return static_cast<uint16_t>(var_res->second);
                        }
                    },
                    [](auto) {
                        panic::panic();
                        return static_cast<std::uint16_t>(0);
                    }
                }, token.value);
            case TokenType::Label:
                return std::visit(overload{
                    [this, &lbl_table](std::string val) {
                        auto lbl_res = lbl_table.find(val);
                        if (lbl_res == lbl_table.end()) { 
                            panic::panic();
                            return static_cast<std::uint16_t>(0);
                        } else {
                            return static_cast<uint16_t>(lbl_res->second);
                        }
                    },
                    [](auto) {
                        panic::panic();
                        return static_cast<std::uint16_t>(0);
                    }
                }, token.value);
            case TokenType::Immediate:
                return std::visit(overload{
                    [](int val) { return static_cast<std::uint16_t>(val); },
                    [](auto) {
                        panic::panic();
                        return static_cast<std::uint16_t>(0);
                    }
                }, token.value);
        }
        return 0;
    }

    uint32_t CodeGen::encode_R(uint8_t instr, uint8_t dest, uint8_t src1, uint8_t src2) {
        uint8_t opcode = (FMT_R << 5) | instr;
        return (opcode << 24) | (dest << 20) | (src1 << 16) | (src2 << 12);
    }

    uint32_t CodeGen::encode_I(uint8_t instr, uint8_t dest, uint8_t src, uint16_t imm) {
        uint8_t opcode = (FMT_I << 5) | instr;
        return (opcode << 24) | (dest << 20) | (src << 16) | (imm);
    }

    uint32_t CodeGen::encode_M(uint8_t instr, uint8_t reg, uint16_t adr) {
        uint8_t opcode = (FMT_M << 5) | instr;
        return (opcode << 24) | (reg << 20) | (adr);
    }

    uint32_t CodeGen::encode_J(uint8_t instr, uint16_t adr) {
        uint8_t opcode = (FMT_J << 5) | instr;
        return (opcode << 24) | (adr);
    }

    uint32_t CodeGen::encode_S(uint8_t instr, uint8_t reg) {
        uint8_t opcode = (FMT_S << 5) | instr;
        return (opcode << 24) | (reg << 20);
    }

    uint32_t CodeGen::encode_N(uint8_t instr) {
        uint8_t opcode = (FMT_N << 5) | instr;
        return (opcode << 24);
    }

    void CodeGen::write_magic(std::ofstream& output, uint32_t MAGIC_NUM) {
        output.put((MAGIC_NUM >> 24) & 0xFF);
        output.put((MAGIC_NUM >> 16) & 0xFF);
        output.put((MAGIC_NUM >> 8)  & 0xFF);
        output.put((MAGIC_NUM)       & 0xFF);
    }

    void CodeGen::write_inst(std::ofstream& output, uint32_t encoded_bytes) {
        output.put((encoded_bytes >> 24) & 0xFF);
        output.put((encoded_bytes >> 16) & 0xFF);
        output.put((encoded_bytes >> 8)  & 0xFF);
        output.put((encoded_bytes)       & 0xFF);
    }

    void CodeGen::generate(std::ofstream& output, const instruction_mod::Inst& inst, const std::unordered_map<std::string, size_t>& lbl_table) {
        uint32_t encoded_inst{0};
        uint8_t enc_opcode{0};
        uint8_t enc_reg{0};
        uint16_t enc_adr{0};
        uint16_t enc_lbl{0};
        switch (inst.inst_type) { //encode instruction based on type
            case InstType::R:
                enc_opcode = encode_tkn_u8(inst.token_arr[0].value());
                if (inst.used_size == 4) { 
                    uint8_t enc_dest = encode_tkn_u8(inst.token_arr[1].value());
                    uint8_t enc_src1 = encode_tkn_u8(inst.token_arr[2].value());
                    uint8_t enc_src2 = encode_tkn_u8(inst.token_arr[3].value());
                    encoded_inst = encode_R(enc_opcode, enc_dest, enc_src1, enc_src2);
                } else if (inst.used_size == 3) {
                    uint8_t enc_dest = encode_tkn_u8(inst.token_arr[1].value());
                    uint8_t enc_src = encode_tkn_u8(inst.token_arr[2].value());
                    encoded_inst = encode_R(enc_opcode, enc_dest, static_cast<uint8_t>(0), enc_src);
                } else {
                    panic::panic();
                }
                break;
            case InstType::I:
                enc_opcode = encode_tkn_u8(inst.token_arr[0].value());
                if (inst.used_size == 4) { 
                    uint8_t enc_dest = encode_tkn_u8(inst.token_arr[1].value());
                    uint8_t enc_src = encode_tkn_u8(inst.token_arr[2].value());
                    uint16_t enc_imm = encode_tkn_u16(inst.token_arr[3].value(), lbl_table);
                    encoded_inst = encode_I(enc_opcode, enc_dest, enc_src, enc_imm);
                } else if (inst.used_size == 3) {
                    uint8_t enc_dest = encode_tkn_u8(inst.token_arr[1].value());
                    uint16_t enc_imm = encode_tkn_u16(inst.token_arr[2].value(), lbl_table);
                    encoded_inst = encode_I(enc_opcode, enc_dest, static_cast<uint8_t>(0), enc_imm);
                } else {
                    panic::panic();
                }
                break;
            case InstType::M:
                enc_opcode = encode_tkn_u8(inst.token_arr[0].value());
                enc_reg = encode_tkn_u8(inst.token_arr[1].value());
                enc_adr = encode_tkn_u16(inst.token_arr[2].value(), lbl_table);
                encoded_inst = encode_M(enc_opcode, enc_reg, enc_adr);
                break;
            case InstType::J:
                enc_opcode = encode_tkn_u8(inst.token_arr[0].value());
                enc_lbl = encode_tkn_u16(inst.token_arr[1].value(), lbl_table);
                encoded_inst = encode_J(enc_opcode, enc_lbl);
                break;
            case InstType::S:
                enc_opcode = encode_tkn_u8(inst.token_arr[0].value());
                enc_reg = encode_tkn_u8(inst.token_arr[1].value());
                encoded_inst = encode_S(enc_opcode, enc_reg);
                break;
            case InstType::N:
                enc_opcode = encode_tkn_u8(inst.token_arr[0].value());
                encoded_inst = encode_N(enc_opcode);
                break;
            case InstType::LBL:
                break;
            default:
                panic::panic();
                break;
        }
        write_inst(output, encoded_inst);
    }

}