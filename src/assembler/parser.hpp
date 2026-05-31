#pragma once

#include <cctype>
#include <string>
#include <vector>
#include <unordered_map>
#include <charconv>
#include <expected>

#include "instructions.hpp"
#include "error.hpp"

namespace parser_mod { 

    using ParseErr = error::ParsingError;

    enum class State {
        Idn,    //identifier mode, transition state, can be pushed as variable or opcode
        Rgt,    //register transition state (when 'R' is detected)
        Reg,    //register
        Sep,    //seperator (,)
        Imt,    //immediate transition state (when '#' is detected)
        Imm,    //immediate
        Lbt,    //label transition (when '@' is detected)
        Lbl,    //label
        Zer,    //zero prefix, transition state
        Adt,    //address transition ('x' in '0x')
        Adr,    //address (eg. 0xDEADBEEF)
        Cmt,    //comment (;)
        Nil,    //none (whitespace type char)
        Err,    //error/unknown state
    };

    enum class StateType {
        Word,
        RTransition, // special variation of transition since its alphanumeric
        ZTransition, // special variation for 0 prefix as there are two characters required for the address prefix
        Transition,
        None,
    };

    extern const std::unordered_map<State, StateType> state_map; //defined in parser.cpp

    enum class Action {
        Push,   //push character to buffer
        Emit,   //emit token in buffer
        Idle,   //do nothing
    };

    class Parser {
        
        private:
            State cur_state;
            State prev_state;
            Action cur_action;
            std::string buffer;
            size_t inst_count;
            instruction_mod::Inst cur_inst;
            unsigned char cur_ch;
            size_t line_count;
            size_t col_count;
            size_t tkn_start_char_col;
            
        public:
            
            Parser();
            std::expected<void, ParseErr> parse(instruction_mod::Pipeline& pipeline, char cur_char); //:D
            std::expected<void, ParseErr> set_state(); //:D
            std::expected<void, ParseErr> set_action(); //:D
            std::expected<void, ParseErr> execute(instruction_mod::Pipeline& pipeline); //:D

    };

}

