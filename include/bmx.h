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
* - File: Header File
* - Project: https://github.com/AnpyDX/Bmx
* - Version: 1.0 (for C++)
* 
* @copyright Copyright (c) 2022 AnpyD
* Distributed under MIT license, or public domain if desired and recognized in your jurisdiction.
*/

#pragma once
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>



/* Public Members */
namespace Bmx
{
    /// BMX Exception Type
    class Exception : public std::exception
    {
    public:
        Exception(const std::string& info);

        virtual ~Exception();

        virtual const char* what() const;

    private:
        std::string mExceptionInfo;
    };

    /// Bmx Type
    class Type
    {
        friend std::string dumps(Type& type);

        public:
            std::string& operator[](const std::string& key);

            /**
            * Usage: add/change block
            */
            void update(const std::string& key, const std::string& value);

            /**
            * Usage: remove block
            */
            void remove(const std::string& key);

            /**
            * Usage: check block is exisited
            */
            bool has_block(const std::string& key) const;

            /**
            * Usage: get all blocks' key
            */
            std::vector<std::string> get_keys() const;

            
        private:
            std::vector<std::string> _keys;
            std::vector<std::string> _values;
    };


    /// Load Bmx string
    Bmx::Type load(const std::string& str);

    /// Load Bmx file
    Bmx::Type loads(std::fstream& file);


    /// Convert the Bmx type to string for writing to a file
    std::string dumps(Bmx::Type& data);
    
}
