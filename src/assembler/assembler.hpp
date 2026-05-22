#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <magic_enum/magic_enum.hpp> //debugging

#include "instructions.hpp"
#include "error.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "codegen.hpp"

namespace assembler_mod {
    
    inline constexpr std::string_view prog_name = "demo";
    inline constexpr uint32_t MAGIC_NUM = 0x3cDEC0DE;

    class Assembler {

        private:
            std::vector<error::Error> error_log;
            bool error_detected;

        public:
            instruction_mod::Pipeline instruction_pipeline;
            std::unordered_map<std::string, size_t> label_table;

            Assembler();
            void assemble_prog(instruction_mod::Pipeline& pipeline, const std::string& file);
            void dbg_display_tokens(const instruction_mod::Pipeline& pipeline) const;
            void dbg_display_labels() const;
            void log_error(error::Err auto error);

    };

}
