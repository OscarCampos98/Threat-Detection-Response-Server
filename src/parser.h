#ifndef PARSER_H
#define PARSER_H

#include <string>

/*
    parser.h

    Purpose:
    Declares the message parsing layer.

    The parser is responsible for converting raw client input into
    a structured format that the threat engine can evaluate later.

    Current supported message examples:
    - HEARTBEAT
    - STATUS OK
    - ERROR TEMP_HIGH
    - COMMAND INVALID

    Later, this parser can be upgraded to handle JSON messages.
*/

enum class MessageType
{
    HEARTBEAT,
    STATUS,
    ERROR,
    COMMAND,
    UNKNOWN,
    INVALID
};

/*
    ParsedMessage stores the result of parsing one client message.

    valid:
        true  -> message follows a known format
        false -> malformed or unknown message

    type:
        detected message type

    raw:
        original message after basic trimming

    payload:
        remaining content after the command/type

    error:
        explanation when parsing fails
*/
struct ParsedMessage
{
    bool valid;
    MessageType type;
    std::string raw;
    std::string payload;
    std::string error;
};

class Parser
{
public:
    /*
        Parses a raw message received from a client.

        Input:
            raw_message -> text received from socket

        Output:
            ParsedMessage -> structured result
    */
    ParsedMessage parse(const std::string &raw_message);

    /*
        Converts a MessageType enum into readable text.
        Useful for logging and terminal output.
    */
    static std::string messageTypeToString(MessageType type);

private:
    /*
        Removes leading and trailing whitespace/newline characters.
    */
    std::string trim(const std::string &input);
};

#endif