#include "assembler.hpp"
#include "../overload.hpp"

using namespace assembler_mod;

Assembler::Assembler() :
    instruction_pipeline{},
    error_detected{false},
    error_log{}
{ }

void Assembler::assemble_prog(instruction_mod::Pipeline& pipeline, const std::string& file) {
    auto parser = parser_mod::Parser();
    for (int pos{0}; pos < file.size(); ++pos) {
        auto parse_success = parser.parse(pipeline, file[pos]);
        if (!parse_success) { if (!error_detected) error_detected = true; log_error(parse_success.error()); };
        //debugging purpose only
        /*std::cout << magic_enum::enum_name(parser.cur_state) 
        << ", Ln: " << parser.line_count 
        << ", Cm: " << parser.col_count << std::endl;*/
    }
    { auto parse_success = parser.parse(pipeline, '\n'); //add \n sentinel
        if (!parse_success) {
            if (!error_detected) error_detected = true; 
            log_error(parse_success.error()); 
        }
    } 

    if (!pipeline.empty()) {
        auto analyzer = analyzer_mod::Analyzer();
        for (int inst_no{0}; inst_no < pipeline.size(); ++inst_no) {
            auto scout_success = analyzer.scout_lbl(pipeline[inst_no]);
            if (!scout_success) { if (!error_detected) error_detected = true; log_error(scout_success.error()); };
        } 
        for (int inst_no{0}; inst_no < pipeline.size(); ++inst_no) {
            auto analyze_success = analyzer.analyze(pipeline[inst_no]);
            //std::cout << inst_no + 1 << ": ";
            if (!analyze_success) { //debug
                if (!error_detected) error_detected = true; 
                log_error(analyze_success.error()); 
                //std::cout << "INVALID\n";
            } else {
                //std::cout << "VALID\n";
            };
            analyzer.RI_inst_classify(pipeline[inst_no]);
        }
    }

}

void Assembler::dbg_display_tokens(const instruction_mod::Pipeline& pipeline) const {

    for (int i{0}; i < pipeline.size(); ++i) {
        const auto& cur_inst{pipeline[i]};
        std::cout << "Instruction No. " << i + 1 << "(" << magic_enum::enum_name(cur_inst.inst_type) << ")" << ": ";
        for (int j{0}; j < cur_inst.INST_SIZE; ++j) {
            const auto& cur_token{cur_inst.token_arr[j]};
            if (cur_token.has_value()) {
                std::cout << magic_enum::enum_name(cur_token->token_type) << " : ";
                std::visit(overload {
                    [](instruction_mod::OpCode oc) { std::cout << magic_enum::enum_name(oc); }, //for opcode
                    [](auto a) { std::cout << a ;}, //for int and double
                }, cur_token->value);
            } else {
                std::cout << "No Token";
            }
            std::cout << " , ";
        }
        std::cout << '\n';
    }

}

void Assembler::log_error(error::Err auto error) {
    error_log.emplace_back(error);
}