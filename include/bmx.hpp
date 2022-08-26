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
* - Project: https://github.com/AnpyDX/Bmx
* - Version: 1.0 (for C++)
* 
* @copyright Copyright (c) 2022 AnpyD
* Distributed under MIT license, or public domain if desired and recognized in your jurisdiction.
*/

#pragma once
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>
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

    // BMX Reader Message
    enum MessageType {
        Warning, Error
    };
    
    void Message(MessageType type, std::string info)
    {
        if (type == Warning)
        {
            throw std::runtime_error("[ Bmx ][ Warning ] " + info);
        }
        else if (type == Error)
        {
            throw std::runtime_error("[ Bmx ][ Error ] " + info);
        }
        else
        {
            // UNDEFINED TYPE, PASS
        }
    }

    const std::string ErrorPoint(const std::string& row, uint32_t rowCount, uint32_t lineCount)
    {
        std::string codeLine;
        std::string underLine;

        codeLine = "|" + std::to_string(rowCount) + " |" + row + "\n";

        for (uint32_t i = 0; i < (3 + (int)(rowCount)/10) + lineCount; i++)
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
    /// Bmx Type
    class Type
    {
        friend std::string dumps(Type& type);

        public:
            std::string& operator[](const std::string& key)
            {
                if (!has_block(key))
                {
                    // Error, key is not exisited
                    Message(Error, "Block \"" + key + "\" is not exisited!");
                }

                return _values[std::find(_keys.begin(), _keys.end(), key) - _keys.begin()];
            }

            /**
            * Usage: add/change block
            */
            void update(const std::string& key, const std::string& value)
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

            /**
            * Usage: remove block
            */
            void remove(const std::string& key)
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
                    Message(Error, "Block \"" + key + "\" is not exisited!");
                }
            }

            /**
            * Usage: check block is exisited
            */
            bool has_block(const std::string& key) const
            {
                return (std::find(_keys.begin(), _keys.end(), key) != _keys.end());
            }

            /**
            * Usage: get all blocks' key
            */
            std::vector<std::string> get_keys() const
            {
                return _keys;
            }

            
        private:
            std::vector<std::string> _keys;
            std::vector<std::string> _values;
    };

    /// Load Bmx string
    Type load(const std::string& str)
    {
        Type _data;

        auto _lastI = str.begin();
        std::string _row = "";
        uint32_t _rowNum = 0;
        bool _inBlock = false;
        std::string _blockName = "";
        std::string _blockContain = "";

        for (auto i = str.begin(); i != str.end(); i++)
        {
            if (*i == '\n' || i == str.end() - 1)
            {   
                // GET ROW
                _row = "";
                if (i == str.end() - 1)
                {
                    for (auto _ri = _lastI; _ri != str.end(); _ri++)
                    {
                        _row.push_back(*_ri);
                    }
                }
                else
                {
                    for (auto _ri = _lastI; _ri != i; _ri++)
                    {
                        _row.push_back(*_ri);
                    }
                }

                _lastI = i + 1;

                // READ ROW
                
                // -- Read block head. 
                // Note: "[[" will be record as "["
                if (std::string(1, _row[0]) == KeyWords[LEFT_BRACKET_KEY] &&
                    std::string(1, _row[1]) != KeyWords[LEFT_BRACKET_KEY])
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
                    for (auto _line = _row.begin() + 1; _line != _row.end(); _line++)
                    {
                        if (std::string(1, *_line) != " ")
                        {
                            _nameBegin = _line;
                            break;
                        }
                    }

                    // Right ]
                    for (auto _line = _row.rbegin(); _line != _row.rend(); _line++)
                    {
                        if (std::string(1, *_line) == KeyWords[RIGHT_BRACKET_KEY])
                        {
                            _foundRightBracket = true;
                            for (auto _inl = _line + 1; _inl != _row.rend(); _inl++)
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
                        Message(Error,  "At (" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                            ErrorPoint(_row, _rowNum + 1, static_cast<uint32_t>(_row.length())) + "\n>> Block head's bracket was never closed!");
                    }
                    // Error, the block head is empty
                    else if (_nameEnd == _row.rend().base())
                    {

                        Message(Error,  "At (" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                            ErrorPoint(_row, _rowNum + 1, static_cast<uint32_t>(_row.length())) + "\n>> Block head is empty!");
                    }
                    // Right block head
                    else
                    {
                        _blockName = _row.substr(_nameBegin - _row.begin(), _nameEnd - _row.begin() - (_nameBegin - _row.begin()) + 1);

                        // Check block name has exisited
                        if (_data.has_block(_blockName))
                        {
                            Message(Error,"At (" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                                ErrorPoint(_row, _rowNum + 1, static_cast<uint32_t>(_nameBegin - _row.begin()) + 1) + "\n>> Block name \"" + _blockName + "\" has exisited!");
                        }

                        _inBlock = true;
                    }

                }

                // -- Read block contain
                else
                {
                    // "[[" => "["
                    if (_row != "" && std::string(1, _row[1]) == KeyWords[LEFT_BRACKET_KEY])
                    {
                        _blockContain.append(_row.substr(1, _row.length()) + "\n");
                    }
                    // If in block, record contain
                    else
                    {
                        if (_inBlock)
                        {
                            _blockContain.append(_row + "\n");
                        }
                    }
                }

                _rowNum++;
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
    Type loads(std::fstream& file)
    {
        Type _data;

        // Check file is opened
        if (!file.is_open()) {
            Message(Error, ">> Failed to open Bmx file!");
        }

        // Read file
        uint32_t _rowNum = 0;
        std::string _row;
        bool _inBlock = false;
        std::string _blockName = "";
        std::string _blockContain = "";
        
        while (std::getline(file, _row))
        {
            // -- Read block head. 
            // Note: "[[" will be record as "["
            if (std::string(1, _row[0]) == KeyWords[LEFT_BRACKET_KEY] &&
                std::string(1, _row[1]) != KeyWords[LEFT_BRACKET_KEY])
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
                for (auto _line = _row.begin() + 1; _line != _row.end(); _line++)
                {
                    if (std::string(1, *_line) != " ")
                    {
                        _nameBegin = _line;
                        break;
                    }
                }

                // Right ]
                for (auto _line = _row.rbegin(); _line != _row.rend(); _line++)
                {
                    if (std::string(1, *_line) == KeyWords[RIGHT_BRACKET_KEY])
                    {
                        _foundRightBracket = true;
                        for (auto _inl = _line + 1; _inl != _row.rend(); _inl++)
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
                    Message(Error, "At file (" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                        ErrorPoint(_row, _rowNum + 1, static_cast<uint32_t>(_row.length())) + "\n>> Block head's bracket was never closed!");
                }
                // Error, the block head is empty
                else if (_nameEnd == _row.rend().base())
                { 
                    Message(Error, "At file (" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                        ErrorPoint(_row, _rowNum + 1, static_cast<uint32_t>(_row.length())) + "\n>> Block head is empty!");
                }
                // Right block head
                else
                {
                    _blockName = _row.substr(_nameBegin - _row.begin(), _nameEnd - _row.begin() - (_nameBegin - _row.begin()) + 1);

                    // Check block name has exisited
                    if (_data.has_block(_blockName))
                    {
                        Message(Error, "At file (" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                            ErrorPoint(_row, _rowNum + 1,static_cast<uint32_t>( _nameBegin - _row.begin()) + 1) + "\n>> Block name \"" + _blockName + "\" has exisited!");
                    }

                    _inBlock = true;
                }

            }

            // -- Read block contain
            else
            {
                // "[[" => "["
                if (_row != "" && std::string(1, _row[1]) == KeyWords[LEFT_BRACKET_KEY])
                {
                    _blockContain.append(_row.substr(1, _row.length()) + "\n");
                }
                // If in block, record contain
                else
                {
                    if (_inBlock)
                    {
                        
                        _blockContain.append(_row + "\n");
                    }
                }
            }
            
            _rowNum++;
        }

        if (_inBlock)
        {
            // Add the last block
            _data.update(_blockName, _blockContain);
        }

        return _data;
    }


    /// Convert the Bmx type to string for writing to a file
    std::string dumps(Type& data)
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
            // Error, there no block
            Message(Error, "No block in Bmx type!");
        }

        return _str;
    }
    
}
