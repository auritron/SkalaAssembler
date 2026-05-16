#include "parser.hpp"
#include "../overload.hpp"

namespace parser_mod {

    const std::unordered_map<State, StateType> state_map {
        { State::Idn, StateType::Word },
        { State::Reg, StateType::Word },
        { State::Imm, StateType::Word },
        { State::Lbl, StateType::Word },
        { State::Adr, StateType::Word },
        { State::Rgt, StateType::RTransition },
        { State::Imt, StateType::Transition },
        { State::Lbt, StateType::Transition },
        { State::Adt, StateType::Transition },
        { State::Zer, StateType::ZTransition },
        { State::Cmt, StateType::None },
        { State::Nil, StateType::None },
        { State::Err, StateType::None },
    };

    Parser::Parser() :
        cur_state{State::Nil},
        prev_state{State::Nil},
        cur_action{Action::Idle},
        buffer{""},
        cur_inst(),
        cur_ch{0},
        ch_count{0},
        line_count{1},
        col_count{0}
    { }

    std::expected<void, ParseErr> Parser::set_state() {


        if (cur_ch == ';') {

            cur_state = State::Cmt;
            
        } else if (cur_state == State::Zer && cur_ch == 'x') { //0x detection

            cur_state = State::Adt;

        } else if (std::isspace(cur_ch)) { //simple state machine lmao

            switch (cur_state) {
                case State::Idn:
                case State::Reg:
                case State::Rgt: // despite being a transition state, this is allowed since R is alphanumeric
                case State::Imm:
                case State::Lbl:
                case State::Adr:
                case State::Nil:
                    cur_state = State::Nil;
                    break;
                case State::Imt:
                case State::Zer:
                    return std::unexpected(ParseErr::ImmediateValueError);
                    break;
                case State::Lbt:
                    return std::unexpected(ParseErr::LabelNamingError);
                    break;
                case State::Adt:
                    return std::unexpected(ParseErr::AddressNamingError);
                    break;
                case State::Err:
                    break;
                case State::Cmt:
                    std::abort();

            }

        } else if (cur_ch == 'R') {

            switch (cur_state) {
                case State::Nil:
                    cur_state = State::Rgt;
                    break;
                case State::Rgt:
                case State::Reg:
                    cur_state = State::Idn;
                    break;
                case State::Adt:
                case State::Adr:
                    return std::unexpected(ParseErr::AddressNamingError);
                    break;
                case State::Imt:
                case State::Imm:
                    return std::unexpected(ParseErr::ImmediateValueError);
                    break;
                case State::Lbt:
                case State::Lbl:
                    cur_state = State::Lbl;
                    break;
                case State::Zer:
                    return std::unexpected(ParseErr::IdentifierNamingError);
                    break;
                case State::Err:
                    break;
                case State::Cmt:
                    std::abort();
                default:
                    break;
            }

        } else if (cur_ch == '@') {
            
            switch (cur_state) {
                case State::Idn:
                case State::Rgt:
                case State::Reg:
                case State::Zer:
                    return std::unexpected(ParseErr::IdentifierNamingError);
                    break;
                case State::Imt:
                case State::Imm:
                    return std::unexpected(ParseErr::ImmediateValueError);
                    break;
                case State::Lbl:
                case State::Lbt:
                    return std::unexpected(ParseErr::LabelNamingError);
                    break;
                case State::Adt:
                case State::Adr:
                    return std::unexpected(ParseErr::AddressNamingError);
                    break;
                case State::Nil:
                    cur_state = State::Lbt;
                    break;
                case State::Err:
                    break;
                case State::Cmt:
                    std::abort();
            }

        } else if (cur_ch == '#') {

            switch (cur_state) {
                case State::Idn:
                case State::Rgt:
                case State::Reg:
                case State::Zer:
                    return std::unexpected(ParseErr::IdentifierNamingError);
                    break;
                case State::Imt:
                case State::Imm:
                    return std::unexpected(ParseErr::ImmediateValueError);
                    break;
                case State::Lbl:
                case State::Lbt:
                    return std::unexpected(ParseErr::LabelNamingError);
                    break;
                case State::Adt:
                case State::Adr:
                    return std::unexpected(ParseErr::AddressNamingError);
                    break;
                case State::Nil:
                    cur_state = State::Imt;
                    break;
                case State::Err:
                    break;
                case State::Cmt:
                    std::abort();
            }

        } else if (cur_ch == '_') {

            switch (cur_state) {
                case State::Idn:
                case State::Rgt:
                case State::Reg:
                case State::Nil:
                    cur_state = State::Idn;
                    break;
                case State::Zer:
                    return std::unexpected(ParseErr::IdentifierNamingError);
                    break;
                case State::Imt:
                case State::Imm:
                    return std::unexpected(ParseErr::ImmediateValueError);
                    break;
                case State::Lbl:
                case State::Lbt:
                    cur_state = State::Lbl;
                    break;
                case State::Adt:
                case State::Adr:
                    return std::unexpected(ParseErr::AddressNamingError);
                    break;
                case State::Err:
                    break;
                case State::Cmt:
                    std::abort();
            }
            
        } else if (std::isdigit(cur_ch)) {

            switch (cur_state) {
                case State::Idn:
                    cur_state = State::Idn;
                    break;
                case State::Rgt:
                case State::Reg:
                    cur_state = State::Reg;
                    break;
                case State::Nil:
                    if (cur_ch == '0') { //if its zero, set to zer to transition to address, else raise error
                        cur_state = State::Zer;
                    } else {
                        return std::unexpected(ParseErr::IdentifierNamingError);
                    }
                    break;
                case State::Imt:
                case State::Imm:
                    cur_state = State::Imm;
                    break;
                case State::Lbt:
                case State::Lbl:
                    cur_state = State::Lbl;
                    break;
                case State::Zer:
                    return std::unexpected(ParseErr::ImmediateValueError);
                    break;
                case State::Adt:
                case State::Adr:
                    cur_state = State::Adr;
                    break;
                case State::Err:
                    break;
                case State::Cmt:
                    std::abort();
            }

        } else if (std::isalpha(cur_ch)) {

            switch (cur_state) {
                case State::Idn:
                case State::Rgt:
                case State::Reg:
                case State::Nil:
                    cur_state = State::Idn;
                    break;
                case State::Imm:
                case State::Imt:
                    return std::unexpected(ParseErr::ImmediateValueError);
                    break;
                case State::Lbt:
                case State::Lbl:
                    cur_state = State::Lbl;
                    break;
                case State::Zer:
                    return std::unexpected(ParseErr::IdentifierNamingError);
                    break;
                case State::Adt:
                case State::Adr:
                    if ((cur_ch >= 'A' && cur_ch <= 'F') || (cur_ch >= 'a' && cur_ch <= 'f')) { //if between A-F or a-f
                        cur_state = State::Adr;
                    } else {
                        return std::unexpected(ParseErr::AddressNamingError);
                    }
                    break;
                case State::Err:
                    break;
                case State::Cmt:
                    std::abort();
                
            }

        } else {

            cur_state = State::Err;
            return std::unexpected(ParseErr::UnknownCharError);

        }

        return {};

    }

    std::expected<void, ParseErr> Parser::set_action() {

        switch ( state_map.at(prev_state) ) {

            case StateType::Word:
            case StateType::RTransition:
                switch ( state_map.at(cur_state) ) {
                    case StateType::Word:
                        cur_action = Action::Push;
                        break;
                    case StateType::None:
                        cur_action = Action::Emit;
                        break;
                    default: // default, anyways handled by error
                        cur_action = Action::Idle;
                        break;
                }
                break;

            case StateType::ZTransition:
                switch ( state_map.at(cur_state) ) { 
                    case StateType::Transition:
                        cur_action = Action::Push;
                        break;
                    default:
                        cur_action = Action::Idle;
                        break;
                }
                break;

            case StateType::Transition:
                switch ( state_map.at(cur_state) ) {
                    case StateType::Word:
                        cur_action = Action::Push;
                        break;
                    default:
                        cur_action = Action::Idle; // anyways handled by error
                        break;
                }
                break;
            case StateType::None:
                switch ( state_map.at(cur_state) ) {
                    case StateType::None:
                        cur_action = Action::Idle;
                        break;
                    default:
                        cur_action = Action::Push;
                        break;
                }
                break;
        }

        return {};

    }

    std::expected<void, ParseErr> Parser::execute(instruction_mod::Pipeline& pipeline) {
            
        instruction_mod::Token token;
        switch (cur_action) {

        case Action::Push:
            buffer.push_back(cur_ch);
            break;
        
        case Action::Emit:
            
            switch (prev_state) {
                case State::Idn:
                    try {
                        token = instruction_mod::Token(instruction_mod::TokenType::OpCode, instruction_mod::instruction_map.at(buffer)); //tt
                    } catch (const std::out_of_range& e) {
                        token = instruction_mod::Token(instruction_mod::TokenType::Variable, buffer); //sv
                    }
                    break;
                case State::Reg:
                case State::Imm:
                    try {
                        size_t pos;
                        int val = std::stoi(buffer.substr(1), &pos);
                        if (pos != (buffer.size() - 1)) throw std::invalid_argument("Not completely int!");
                        switch (prev_state) {
                            case State::Reg: token = instruction_mod::Token(instruction_mod::TokenType::Register, val); break; //account for error handling
                            case State::Imm: token = instruction_mod::Token(instruction_mod::TokenType::Immediate, val); break;
                        }
                    } catch (const std::invalid_argument& e) { //shouldnt happen ideally
                        token = instruction_mod::Token(instruction_mod::TokenType::Invalid, 0);
                    } catch (const std::out_of_range& e) {
                        switch (prev_state) {
                            case State::Reg: return std::unexpected(ParseErr::RegisterOutOfRange); break;
                            case State::Imm: return std::unexpected(ParseErr::ImmediateValueTooBig); break;
                        }
                    }
                    break;
                case State::Adr:
                    try {
                        size_t pos;
                        int val = std::stoi(buffer.substr(2), &pos, 16); //hex code
                        if (pos != (buffer.size() - 2)) throw std::invalid_argument("Not completely hex!");
                        token = instruction_mod::Token(instruction_mod::TokenType::Address, val); //account for error handling
                    } catch (const std::invalid_argument& e) { //shouldnt happen ideally
                        token = instruction_mod::Token(instruction_mod::TokenType::Invalid, 0);
                    } catch (const std::out_of_range& e) {
                        return std::unexpected(ParseErr::AddressOutOfRange);
                    }
                    break;
                case State::Lbl:
                    token = instruction_mod::Token(instruction_mod::TokenType::Label, buffer.substr(1));
                    break;
                default:
                    std::abort();
            }
            
            if (!cur_inst.push_token(std::move(token))) return std::unexpected(ParseErr::InstructionTooLong); //move happens here 
            buffer.clear();
            break;

        case Action::Idle: 
            break;

        }

        return {};

    }

    std::expected<void, ParseErr> Parser::parse(instruction_mod::Pipeline& pipeline, char current_char) {

        cur_ch = current_char;

        if (cur_state != State::Err && cur_state != State::Cmt) {
            auto result = set_state()
                .and_then([this]() { return set_action(); })
                .and_then([this, &pipeline]() { return execute(pipeline); });

            if (!result) {
                cur_state = State::Err;
                buffer.clear();
                return std::unexpected<ParseErr>(result.error());
            }
        }

        if (cur_ch == '\n') {
            if (cur_state != State::Err && cur_inst.used_size != 0) {
                pipeline.push_back(std::move(cur_inst));
            }
            cur_inst = instruction_mod::Inst(); //reset current instruction
            ++line_count;
            col_count = 0;
            cur_state = State::Nil;
        } else {
            ++col_count;
        }

        prev_state = cur_state;

        return {};
    }

    //helpers
    bool Parser::inst_type_already_opcode() {
        switch (cur_inst.inst_type) {
            case instruction_mod::InstType::LBL:
            case instruction_mod::InstType::INV:
                return false;
            default:
                return true;
        }
    }

}