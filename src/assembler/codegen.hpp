#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

#include "instructions.hpp"
#include "error.hpp"

namespace codegen_mod {

    extern const std::unordered_map<instruction_mod::OpCode, uint8_t> opcode_mapping;

    class CodeGen {

        private: 

            size_t next_unused_adr;
            std::unordered_map<std::string, size_t>var_table;

        public:

            CodeGen();
            uint8_t encode_tkn_u8(const instruction_mod::Token& token); //FINISH
            uint16_t encode_tkn_u16(const instruction_mod::Token& token, const std::unordered_map<std::string, size_t>& lbl_table); //FINISH
            uint32_t encode_R(uint8_t opcode, uint8_t dest, uint8_t src1, uint8_t src2);
            uint32_t encode_I(uint8_t opcode, uint8_t dest, uint8_t src, uint16_t imm);
            uint32_t encode_M(uint8_t opcode, uint8_t reg, uint16_t adr);
            uint32_t encode_J(uint8_t opcode, uint16_t adr);
            uint32_t encode_S(uint8_t opcode, uint8_t reg);
            uint32_t encode_N(uint8_t opcode);
            void write_magic(std::ofstream& output, uint32_t MAGIC_NUM);
            void write_inst(std::ofstream& output, uint32_t encoded_bytes);
            void generate(std::ofstream& output, const instruction_mod::Inst& inst, const std::unordered_map<std::string, size_t>& lbl_table);

    };

}