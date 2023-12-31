#pragma once
#include <string>
#include <cstdarg>
#include <vector>

typedef struct SyComplexString xstring;
struct SyComplexString
{
	enum ArgType
	{
		INT,
		STRING,
		DOUBLE,
		UNKNOWN
	};
	SyComplexString()
	{
		*this = SyComplexString("");
	}
	template<typename... Args>
	SyComplexString(std::string mainString_, Args... args_)
	{
		mainString = mainString_;
		length = mainString_.length();
		std::vector <ArgType> argTypes;
		argTypes.reserve(6);
		int argNum = 0;
		for (auto c = mainString_.c_str(); *c != '\0'; c++)
		{
			if (*c == '%' && *(c + 1) != '\0')
			{
				switch (*(c + 1))
				{
				case 'd':
					argTypes.push_back(INT);
					argNum++;
					break;
				case 's':
					argTypes.push_back(STRING);
					argNum++;
					break;
				case 'f':
					argTypes.push_back(DOUBLE);
					argNum++;
					break;
				case '%':
					c++;
					break;
				default:
					argTypes.push_back(UNKNOWN);
					argNum++;
					break;
				}
				c++;
			}
		}
		ParseArguments(argTypes, argNum, args_...);
	}
	template<typename... Args>
	SyComplexString(const char* mainString_ = "", Args... args_)
	{
		*this = SyComplexString(std::string(mainString_), args_...);
	}
	std::string& ToString()
	{
		if (asString.empty())
		{
			asString.reserve(length);
			size_t insCtr = 0;
			for (auto c = mainString.c_str(); *c != '\0'; c++)
				if (*c == '%' && *(c + 1) != '\0')
				{
					if (*(c + 1) == '%')
						asString += '%';
					else
					{
						for (int i = 0; i < inserts[insCtr].length(); i++)
						{
							asString += inserts[insCtr][i];
						}
						insCtr++;
					}
					c++;
				}
				else
					asString += *c;
		}
		return asString;
	}
	size_t GetLength() { return length; };
	std::string& GetInsertion(int num_) { return inserts[num_]; }
private:
	void ParseArguments(const std::vector <ArgType>& argTypes_, int count_, ...)
	{
		std::va_list args;
		va_start(args, count_);
		for (auto& argType : argTypes_)
		{
			std::string argStr;
			switch (argType)
			{
			case INT:
				argStr = std::to_string(va_arg(args, int));
				break;
			case STRING:
				argStr = std::string(va_arg(args, char*));
				break;
			case DOUBLE:
				argStr = std::to_string(va_arg(args, double));
				break;
			case UNKNOWN:
				argStr = "";
				break;
			}
			inserts.push_back(argStr);
			length += argStr.length();
		}
		va_end(args);
		length -= 2 * count_;
	}

	std::string mainString;
	std::vector<std::string> inserts;
	size_t length = 0;
	std::string asString;
};
