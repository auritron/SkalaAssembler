#include "error.hpp"
#include "../overload.hpp"

using namespace error;

std::string_view Error::fmt_error_as_str() {

    return std::visit(overload{
        [](ParsingError PErr){
            switch (PErr) {
                case ParsingError::ImmediateValueError:
                    return "Operand is not a correct immediate value";
                case ParsingError::IdentifierNamingError:
                    return "Identifier is not named correctly";
                case ParsingError::LabelNamingError:
                    return "Incorrect syntax for naming label";
                case ParsingError::AddressNamingError:
                    return "Incorrect syntax for naming address label";
                case ParsingError::UnknownCharError:
                    return "Unknown character, cannot be parsed";
                case ParsingError::RegisterOutOfRange:
                    return "Requested register number is too large";
                case ParsingError::ImmediateValueTooBig:
                    return "Immediate value is too large to be parsed";
                case ParsingError::AddressOutOfRange:
                    return "Address value is too large to be parsed";
                case ParsingError::InstructionTooLong:
                    return "Input instruction has too many operands";
                default:
                    return "Error message for this type has not been implemented";
            }
        },
        [](SemanticError SErr){
            switch(SErr) {
                case SemanticError::MissingOpCodeError:
                    return "Missing OpCode in Instruction";
                case SemanticError::IncorrectFirstToken:
                    return "Instruction must either start with an OpCode or a Label";
                case SemanticError::IncorrectOperandFmt:
                    return "Operands not in the correct format as the given instructions";
                case SemanticError::InvalidOpCodePosition:
                    return "Opcode position is invalid";
                case SemanticError::RegisterOutOfRange:
                    return "Requested register number is out of range";
                case SemanticError::ImmediateOutOfRange:
                    return "Immediate value in instruction is out of range";
                case SemanticError::AddressOutOfRange:
                    return "Memory address in instruction is out of range";
                case SemanticError::LabelAlreadyExists:
                    return "Specified Label has already been defined";
                case SemanticError::LabelTooLongError:
                    return "Specified Label should not have extra operards";
                case SemanticError::UndefinedLabelError:
                    return "Specified Label has not yet been defined, but is used";
                case SemanticError::UnknownSemanticError:
                    return "Unknown semantic error";
                default:
                    return "Error message for this type has not been implemented";
            }
        }
    }, this->error);

}