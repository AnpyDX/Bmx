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

    void Message(MessageType type, std::string file, std::string info)
    {
        if (type == Warning)
        {
            std::cerr << "[ Warning ] File " << file << std::endl << info << std::endl;
        }
        else if (type == Error)
        {
            std::cerr << "[ Error ] File " << file << std::endl << info << std::endl;
            throw std::runtime_error(info);
        }
        else
        {
            // UNDIFINED TYPE, PASS
        }
    }
    
    void Message(MessageType type, std::string info)
    {
        if (type == Warning)
        {
            std::cerr << "[ Warning ] " << info << std::endl;
        }
        else if (type == Error)
        {
            std::cerr << "[ Error ] " << info << std::endl;
            throw std::runtime_error(info);
        }
        else
        {
            // UNDEFINED TYPE, PASS
        }
    }

    const std::string ErrorPoint(const std::string& row, int rowCount, int lineCount)
    {
        std::string codeLine;
        std::string underLine;

        codeLine = "|" + std::to_string(rowCount) + " |" + row + "\n";

        for (int i = 0; i < (3 + (int)(rowCount)/10) + lineCount; i++)
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
    class Type
    {
        public:
            const std::string& operator[](const std::string& key) const
            {
                // check key is exisit
                if (has_block(key))
                {
                    return _values[std::find(_keys.begin(), _keys.end(), key) - _keys.begin()];
                }
                else
                {
                    // Error, key is not exisited
                    Message(Error, "Block \"" + key + "\" is not exisited!");
                }
            }

            /**
            * Usage: add/change block
            * Note: In order to avoid modifing original data accidently, calling this function will give you a warning.
            *       If you do know what you are doing, set parameter "noWarning" to true to disable warning.
            */
            void update(const std::string& key, const std::string& value, const bool noWarning = false)
            {
                if (!has_block(key))
                {
                    _keys.push_back(key);
                    _values.push_back(value);

                    // Warning
                    if (!noWarning)
                    {
                        Message(Warning, "You have add block \"" + key + "\" , this will cause the data to differ from the original file!");
                    }
                }
                else
                {
                    __int64 index = std::find(_keys.begin(), _keys.end(), key) - _keys.begin();
                    _values[index] = value;

                    // Warning
                    if (!noWarning)
                    {
                        Message(Warning, "You have change \"" + key + "\"'s data , this will cause the data to differ from the original file!");
                    }
                }

                
            }

            /**
            * Usage: remove block
            * Note: In order to avoid modifing original data accidently, calling this function will give you a warning.
            *       If you do know what you are doing, set parameter "noWarning" to true to disable warning.
            */
            void remove(const std::string& key, const bool noWarning = false)
            {
                if (has_block(key))
                {
                    __int64 index = std::find(_keys.begin(), _keys.end(), key) - _keys.begin();
                    _keys.erase(_keys.begin() + index);
                    _values.erase(_values.begin() + index);

                    // Warning
                    if (!noWarning)
                    {
                        Message(Warning, "You have deleted block \"" + key + "\" , this will cause the data to differ from the original file!");
                    }

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

    /// Read Bmx file
    Type read(const std::string& file)
    {
        Type _data;

        // Open file
        std::ifstream _file(file);
        if (!_file.is_open()) {
            Message(Error, file, ">> Failed to open Bmx file!");
        }

        // Read file
        uint32_t _rowNum = 0;
        std::string _row;
        bool _inBlock = false;
        std::string _blockName = "";
        std::string _blockContain = "";
        
        while (std::getline(_file, _row))
        {
            // -- Read block head. 
            // Note: "[[" will be record as "["
            if (std::string(1, _row[0]) == KeyWords[LEFT_BRACKET_KEY] &&
                std::string(1, _row[1]) != KeyWords[LEFT_BRACKET_KEY])
            {
                // End recording block contain
                if (_inBlock)
                {
                    _data.update(_blockName, _blockContain, true);
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
                    Message(Error, file + "(" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                        ErrorPoint(_row, _rowNum + 1, _row.length()), ">> Error! Block head's bracket was never closed!");
                }
                // Error, the block head is empty
                else if (_nameEnd == _row.rend().base())
                {
                    
                    Message(Error, file + "(" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                        ErrorPoint(_row, _rowNum + 1, _row.length()), ">> Error! Block head is empty!");
                }
                // Right block head
                else
                {
                    _blockName = _row.substr(_nameBegin - _row.begin(), _nameEnd - _row.begin() - (_nameBegin - _row.begin()) + 1);

                    // Check block name has exisited
                    if (_data.has_block(_blockName))
                    {
                        Message(Error, file + "(" + std::to_string(_rowNum + 1) + ", " + std::to_string(_row.length()) + ")\n" +
                            ErrorPoint(_row, _rowNum + 1, _nameBegin - _row.begin() + 1), ">> Error! Block name \"" + _blockName + "\" has exisited!");
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
            _data.update(_blockName, _blockContain, true);
        }

        return _data;
    }
}
