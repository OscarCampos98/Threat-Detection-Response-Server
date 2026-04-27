#include "parser.h"

#include <sstream>
#include <algorithm>
#include <cctype>

/*
    parser.cpp

    Purpose:
    Implements basic message parsing.

    Current parser format:
        COMMAND [PAYLOAD]

    Examples:
        HEARTBEAT
        STATUS OK
        ERROR TEMP_HIGH
        COMMAND INVALID

    Later:
        This can be replaced or extended with JSON parsing.
*/

ParsedMessage Parser::parse(const std::string &raw_message)
{
    ParsedMessage result;

    result.raw = trim(raw_message);
    result.payload = "";
    result.error = "";
    result.valid = false;
    result.type = MessageType::INVALID;

    /*
        Empty messages are invalid.
    */
    if (result.raw.empty())
    {
        result.error = "Empty message";
        return result;
    }

    /*
        Split message into:
        - first word: command/type
        - rest: payload
    */
    std::istringstream stream(result.raw);

    std::string command;
    stream >> command;

    std::string payload;
    std::getline(stream, payload);
    payload = trim(payload);

    result.payload = payload;

    /*
        HEARTBEAT should not require a payload.
    */
    if (command == "HEARTBEAT")
    {
        result.valid = true;
        result.type = MessageType::HEARTBEAT;
        return result;
    }

    /*
        STATUS requires a payload.

        Example:
            STATUS OK
    */
    if (command == "STATUS")
    {
        if (payload.empty())
        {
            result.error = "STATUS message missing payload";
            result.type = MessageType::INVALID;
            return result;
        }

        result.valid = true;
        result.type = MessageType::STATUS;
        return result;
    }

    /*
        ERROR requires a payload.

        Example:
            ERROR TEMP_HIGH
    */
    if (command == "ERROR")
    {
        if (payload.empty())
        {
            result.error = "ERROR message missing payload";
            result.type = MessageType::INVALID;
            return result;
        }

        result.valid = true;
        result.type = MessageType::ERROR;
        return result;
    }

    /*
        COMMAND requires a payload.

        Example:
            COMMAND INVALID
    */
    if (command == "COMMAND")
    {
        if (payload.empty())
        {
            result.error = "COMMAND message missing payload";
            result.type = MessageType::INVALID;
            return result;
        }

        result.valid = true;
        result.type = MessageType::COMMAND;
        return result;
    }

    /*
        If the first word does not match any known message type,
        mark it as UNKNOWN.
    */
    result.type = MessageType::UNKNOWN;
    result.error = "Unknown message type";

    return result;
}

std::string Parser::messageTypeToString(MessageType type)
{
    switch (type)
    {
    case MessageType::HEARTBEAT:
        return "HEARTBEAT";
    case MessageType::STATUS:
        return "STATUS";
    case MessageType::ERROR:
        return "ERROR";
    case MessageType::COMMAND:
        return "COMMAND";
    case MessageType::UNKNOWN:
        return "UNKNOWN";
    case MessageType::INVALID:
        return "INVALID";
    default:
        return "INVALID";
    }
}

std::string Parser::trim(const std::string &input)
{
    /*
        Find first non-whitespace character.
    */
    auto start = std::find_if_not(input.begin(), input.end(), [](unsigned char ch)
                                  { return std::isspace(ch); });

    /*
        Find last non-whitespace character.
    */
    auto end = std::find_if_not(input.rbegin(), input.rend(), [](unsigned char ch)
                                { return std::isspace(ch); })
                   .base();

    /*
        If string is all whitespace, return empty string.
    */
    if (start >= end)
    {
        return "";
    }

    return std::string(start, end);
}