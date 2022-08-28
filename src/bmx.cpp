/**
*                    ___           ___
*     _____         /__/\         /__/|
*    /  /::\       |  |::\       |  |:|
*   /  /:/\:\      |  |:|:\      |  |:|
*  /  /:/~/::\   __|__|:|\:\   __|__|:|
* /__/:/ /:/\:| /__/::::| \:\ /__/::::\____
* \  \:\/:/~/:/ \  \:\~~\__\/    ~\~~\::::/
*  \  \::/ /:/   \  \:\           |~~|:|~~
*   \  \:\/:/     \  \:\          |  |:|
*    \  \::/       \  \:\         |  |:|
*     \__\/         \__\/         |__|/
*
* Block Mixture Language for Modern C++
* - File: Source File
* - Project: https://github.com/AnpyDX/Bmx
* - Version: 1.0 (for C++)
*
* @copyright Copyright (c) 2022 AnpyD
* Distributed under MIT license, or public domain if desired and recognized in your jurisdiction.
*/


#include "include/bmx.h"
#include <iostream>
#include <map>



/* Internal Members */
namespace
{
    // BMX Language Keys
    enum KeyWordEnum {
        LEFT_BRACKET_KEY,
        RIGHT_BRACKET_KEY
    };

    std::map<KeyWordEnum, std::string> KeyWords = {
        {LEFT_BRACKET_KEY, "["},
        {RIGHT_BRACKET_KEY, "]"}
    };

    // Wrong Point Info
    const std::string ErrorPoint(const std::string& row, uint32_t rowCount, uint32_t lineCount)
    {
        std::string codeLine;
        std::string underLine;

        codeLine = "|" + std::to_string(rowCount) + " |" + row + "\n";

        for (uint32_t i = 0; i < (3 + (int)(rowCount) / 10) + lineCount; i++)
        {
            underLine.append(" ");
        }
        underLine.append("^");

        return codeLine.append(underLine);
    }

}


/* Public Members */
namespace Bmx
{
    // BMX Exception Type
    Exception::Exception(const std::string& info)
    {
        this->mExceptionInfo = info;
    }

    Exception::~Exception() {}

    const char* Exception::what() const
    {
        return this->mExceptionInfo.c_str();
    }


    /// Bmx Type
    std::string& Type::operator[](const std::string& key)
    {
        if (!has_block(key))
        {
            // Error, key is not exisited
            throw Bmx::Exception("Block \"" + key + "\" is not exisited!");
        }

        return _values[std::find(_keys.begin(), _keys.end(), key) - _keys.begin()];
    }

    void Type::update(const std::string& key, const std::string& value)
    {
        if (!has_block(key))
        {
            _keys.push_back(key);
            _values.push_back(value);
        }
        else
        {
            __int64 index = std::find(_keys.begin(), _keys.end(), key) - _keys.begin();
            _values[index] = value;
        }
    }

    void Type::remove(const std::string& key)
    {
        if (has_block(key))
        {
            __int64 index = std::find(_keys.begin(), _keys.end(), key) - _keys.begin();
            _keys.erase(_keys.begin() + index);
            _values.erase(_values.begin() + index);
        }
        else
        {
            // Error, key is not exisited
            throw Bmx::Exception("Block \"" + key + "\" is not exisited!");
        }
    }

    bool Type::has_block(const std::string& key) const
    {
        return (std::find(_keys.begin(), _keys.end(), key) != _keys.end());
    }

    std::vector<std::string> Type::get_keys() const
    {
        return _keys;
    }


    /// Load Bmx string
    Bmx::Type load(const std::string& str)
    {
        Bmx::Type _data;

        auto _lastI = str.begin();
        std::string _currentLine = "";
        uint32_t _lineNum = 0;
        bool _inBlock = false;
        std::string _blockName = "";
        std::string _blockContain = "";

        for (auto i = str.begin(); i != str.end(); i++)
        {
            if (*i == '\n' || i == str.end() - 1)
            {
                // GET LINE
                _currentLine = "";
                if (i == str.end() - 1)
                {
                    for (auto _ri = _lastI; _ri != str.end(); _ri++)
                    {
                        _currentLine.push_back(*_ri);
                    }
                }
                else
                {
                    for (auto _ri = _lastI; _ri != i; _ri++)
                    {
                        _currentLine.push_back(*_ri);
                    }
                }

                _lastI = i + 1;

                // READ LINE

                // -- Read block head. 
                // Note: "[[" will be record as "["
                if (std::string(1, _currentLine[0]) == KeyWords[LEFT_BRACKET_KEY] &&
                    std::string(1, _currentLine[1]) != KeyWords[LEFT_BRACKET_KEY])
                {
                    // End recording block contain
                    if (_inBlock)
                    {
                        _data.update(_blockName, _blockContain);
                        _blockName = "";
                        _blockContain = "";
                        _inBlock = false;
                    }

                    // Read block head
                    std::string::iterator _nameBegin;
                    std::string::iterator _nameEnd;
                    bool _foundRightBracket = false;
                    bool _foundRightBegin = false;

                    // Left [
                    for (auto _column = _currentLine.begin() + 1; _column != _currentLine.end(); _column++)
                    {
                        if (std::string(1, *_column) != " ")
                        {
                            _nameBegin = _column;
                            break;
                        }
                    }

                    // Right ]
                    for (auto _column = _currentLine.rbegin(); _column != _currentLine.rend(); _column++)
                    {
                        if (std::string(1, *_column) == KeyWords[RIGHT_BRACKET_KEY])
                        {
                            _foundRightBracket = true;
                            for (auto _inl = _column + 1; _inl != _currentLine.rend(); _inl++)
                            {
                                if (std::string(1, *_inl) != " ")
                                {
                                    _nameEnd = _inl.base() - 1;
                                    _foundRightBegin = true;
                                    break;
                                }
                            }
                        }

                        if (_foundRightBegin)
                        {
                            break;
                        }
                    }

                    // Error, the bracket was not closed
                    if (!_foundRightBracket)
                    {
                        throw Bmx::Exception("At (" + std::to_string(_lineNum + 1) + ", " + std::to_string(_currentLine.length()) + ")\n" +
                            ErrorPoint(_currentLine, _lineNum + 1, static_cast<uint32_t>(_currentLine.length())) + "\n>> Block head's bracket was never closed!");
                    }
                    // Error, the block head is empty
                    else if (_nameEnd == _currentLine.rend().base())
                    {

                        throw Bmx::Exception("At (" + std::to_string(_lineNum + 1) + ", " + std::to_string(_currentLine.length()) + ")\n" +
                            ErrorPoint(_currentLine, _lineNum + 1, static_cast<uint32_t>(_currentLine.length())) + "\n>> Block head is empty!");
                    }
                    // Right block head
                    else
                    {
                        _blockName = _currentLine.substr(_nameBegin - _currentLine.begin(), _nameEnd - _currentLine.begin() - (_nameBegin - _currentLine.begin()) + 1);

                        // Check block name has exisited
                        if (_data.has_block(_blockName))
                        {
                            throw Bmx::Exception("At (" + std::to_string(_lineNum + 1) + ", " + std::to_string(_nameBegin - _currentLine.begin() + 1) + ")\n" +
                                ErrorPoint(_currentLine, _lineNum + 1, static_cast<uint32_t>(_nameBegin - _currentLine.begin()) + 1) + "\n>> Block name \"" + _blockName + "\" has exisited!");
                        }

                        _inBlock = true;
                    }

                }

                // -- Read block contain
                else
                {
                    // "[[" => "["
                    if (_currentLine != "" && std::string(1, _currentLine[1]) == KeyWords[LEFT_BRACKET_KEY])
                    {
                        _blockContain.append(_currentLine.substr(1, _currentLine.length()) + "\n");
                    }
                    // If in block, record contain
                    else
                    {
                        if (_inBlock)
                        {
                            _blockContain.append(_currentLine + "\n");
                        }
                    }
                }

                _lineNum++;
            }
        }

        if (_inBlock)
        {
            // Add the last block
            _data.update(_blockName, _blockContain);
        }

        return _data;
    }


    /// Load Bmx file
    Bmx::Type loads(std::fstream& file)
    {
        Bmx::Type _data;

        // Check file is opened
        if (!file.is_open()) {
            throw Bmx::Exception(">> Failed to open Bmx file!");
        }

        // Read file
        uint32_t _lineNum = 0;
        std::string _currentLine = "";
        bool _inBlock = false;
        std::string _blockName = "";
        std::string _blockContain = "";

        while (std::getline(file, _currentLine))
        {
            // -- Read block head. 
            // Note: "[[" will be record as "["
            if (std::string(1, _currentLine[0]) == KeyWords[LEFT_BRACKET_KEY] &&
                std::string(1, _currentLine[1]) != KeyWords[LEFT_BRACKET_KEY])
            {
                // End recording block contain
                if (_inBlock)
                {
                    _data.update(_blockName, _blockContain);
                    _blockName = "";
                    _blockContain = "";
                    _inBlock = false;
                }


                // Read block head
                std::string::iterator _nameBegin;
                std::string::iterator _nameEnd;
                bool _foundRightBracket = false;
                bool _foundRightBegin = false;

                // Left [
                for (auto _column = _currentLine.begin() + 1; _column != _currentLine.end(); _column++)
                {
                    if (std::string(1, *_column) != " ")
                    {
                        _nameBegin = _column;
                        break;
                    }
                }

                // Right ]
                for (auto _column = _currentLine.rbegin(); _column != _currentLine.rend(); _column++)
                {
                    if (std::string(1, *_column) == KeyWords[RIGHT_BRACKET_KEY])
                    {
                        _foundRightBracket = true;
                        for (auto _inl = _column + 1; _inl != _currentLine.rend(); _inl++)
                        {
                            if (std::string(1, *_inl) != " ")
                            {
                                _nameEnd = _inl.base() - 1;
                                _foundRightBegin = true;
                                break;
                            }
                        }
                    }

                    if (_foundRightBegin)
                    {
                        break;
                    }
                }

                // Error, the bracket was not closed
                if (!_foundRightBracket)
                {
                    throw Bmx::Exception("At file (" + std::to_string(_lineNum + 1) + ", " + std::to_string(_currentLine.length()) + ")\n" +
                        ErrorPoint(_currentLine, _lineNum + 1, static_cast<uint32_t>(_currentLine.length())) + "\n>> Block head's bracket was never closed!");
                }
                // Error, the block head is empty
                else if (_nameEnd == _currentLine.rend().base())
                {
                    throw Bmx::Exception("At file (" + std::to_string(_lineNum + 1) + ", " + std::to_string(_currentLine.length()) + ")\n" +
                        ErrorPoint(_currentLine, _lineNum + 1, static_cast<uint32_t>(_currentLine.length())) + "\n>> Block head is empty!");
                }
                // Right block head
                else
                {
                    _blockName = _currentLine.substr(_nameBegin - _currentLine.begin(), _nameEnd - _currentLine.begin() - (_nameBegin - _currentLine.begin()) + 1);

                    // Check block name has exisited
                    if (_data.has_block(_blockName))
                    {
                        throw Bmx::Exception("At file (" + std::to_string(_lineNum + 1) + ", " + std::to_string(_nameBegin - _currentLine.begin() + 1) + ")\n" +
                            ErrorPoint(_currentLine, _lineNum + 1, static_cast<uint32_t>(_nameBegin - _currentLine.begin()) + 1) + "\n>> Block name \"" + _blockName + "\" has exisited!");
                    }

                    _inBlock = true;
                }

            }

            // -- Read block contain
            else
            {
                // "[[" => "["
                if (_currentLine != "" && std::string(1, _currentLine[1]) == KeyWords[LEFT_BRACKET_KEY])
                {
                    _blockContain.append(_currentLine.substr(1, _currentLine.length()) + "\n");
                }
                // If in block, record contain
                else
                {
                    if (_inBlock)
                    {

                        _blockContain.append(_currentLine + "\n");
                    }
                }
            }

            _lineNum++;
        }

        if (_inBlock)
        {
            // Add the last block
            _data.update(_blockName, _blockContain);
        }

        return _data;
    }


    /// Convert the Bmx type to string for writing to a file
    std::string dumps(Bmx::Type& data)
    {
        std::string _str;

        if (data.get_keys().size() != 0)
        {
            auto _keys = data.get_keys();
            for (size_t i = 0; i < _keys.size(); i++)
            {
                _str.append("[ " + data._keys[i] + " ]\n" + data._values[i] + "\n");
            }
        }
        else
        {
            // Error, there are no blocks in data
            throw Bmx::Exception("No block in Bmx type!");
        }

        return _str;
    }
}