#pragma once

#include <array>
#include <optional>
#include <expected>
#include <string_view>
#include <unordered_set>

#include "instructions.hpp"
#include "error.hpp"

namespace analyzer_mod {

    using SemErr = error::SemanticError;

    using Opt = std::optional<instruction_mod::TokenOpt>;
    using Fmt = std::array<Opt, instruction_mod::Inst::INST_SIZE - 1>; // 3 by default

    using InstructionFmt = std::unordered_map<instruction_mod::OpCode, Fmt>;
    extern const InstructionFmt instruction_fmt;    

    class Analyzer {

        private:

            std::unordered_set<std::string> label_table;

        public:

            Analyzer();
            std::expected<void, SemErr> scout_lbl(instruction_mod::Inst& inst); // :D
            std::expected<void, SemErr> validate_token(const instruction_mod::Token& token) const; // :D
            std::expected<void, SemErr> validate_opcode(instruction_mod::Inst& inst, instruction_mod::OpCode opcode) const; // :D
            std::expected<void, SemErr> analyze(instruction_mod::Inst& inst) const; // :D

    };

}