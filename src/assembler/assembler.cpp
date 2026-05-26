#include "assembler.hpp"
#include "../overload.hpp"

using namespace assembler_mod;

Assembler::Assembler() :
    instruction_pipeline{},
    error_detected{false},
    error_log{},
    label_table{}
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
            auto scout_success = analyzer.scout_lbl(pipeline[inst_no], label_table);
            if (!scout_success) { if (!error_detected) error_detected = true; log_error(scout_success.error()); };
        } 
        for (int inst_no{0}; inst_no < pipeline.size(); ++inst_no) {
            auto analyze_success = analyzer.analyze(pipeline[inst_no], label_table);
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

    if (!error_detected) { //only do codegen if no errors are detected
        std::cout << "Errors not detected. Code generation started.\n";
        auto codegen = codegen_mod::CodeGen();
        fs::path source_dir = fs::path(__FILE__).parent_path().parent_path().parent_path();
        std::string filename = std::string(assembler_mod::prog_name) + ".bin";
        fs::path filepath = source_dir / "progs" / filename;
        std::cout << filepath << std::endl;
        std::ofstream output(filepath, std::ios::binary);
        codegen.write_magic(output, MAGIC_NUM);
        for (const auto& inst : pipeline) {
            codegen.generate(output, inst, label_table);
        }
        std::cout << "Code generation successfully completed.\n";
    } else {
        std::cout << "Errors detected. Code generation did not start.\n";
    }

}

void Assembler::dbg_display_tokens(const instruction_mod::Pipeline& pipeline) const {

    for (int i{0}; i < pipeline.size(); ++i) {
        const auto& cur_inst{pipeline[i]};
        std::cout << "Instruction: " << i << "(" << magic_enum::enum_name(cur_inst.inst_type) << ")" << ": ";
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

void Assembler::dbg_display_labels() const {
    
    for (const auto& [key, value] : label_table) {
        std::cout << key << ": " << value << "\n";
    }

}

void Assembler::log_error(error::Err auto error) {
    error_log.emplace_back(error);
}