#ifndef UTILS_HPP
# define UTILS_HPP

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <cstring>


class Utils
{
	public:
		Utils();
		~Utils();
		void						storeFileContent(std::ifstream &file, std::string& fileContent);

		// CONVERSIONS
		std::string 				intToString(int number);

		template<typename T>
		std::vector<T>	convListToVector(std::list<T> list)
		{
			std::vector<T>	ret;
			for (typename std::list<T>::iterator it = list.begin(); it != list.end(); it++)
			{
				ret.push_back(*it);
			}
			return(ret);
		};

		char**	convertStringVectorToCharArray(std::vector<std::string>& stringVector);
		std::vector<std::string>	convertStringArrayToStringVector(const char* stringArray[]);

		// COUNT
		int							countChar(const std::string s, char c);
		int							count_words(const std::string& input);
		int							countDotDotSlash(const std::string& str, size_t& pos);

		// LENGTH
		template<typename T>
		int	vectorLength(std::vector<T> vec)
		{
			int length = 0;
			for (typename std::vector<T>::iterator it = vec.begin(); it != vec.end(); it++)
				length++;
			return (length);
		}
		template<typename Container>
		int containerLength(const Container& cont)
		{
			int length = 0;
			for(typename Container::const_iterator it = cont.begin(); it != cont.end(); ++it)
				++length;
			return length;
		}

		// ERROR CHECK
		int							checkDoublesVectorString(std::vector<std::string> og);
		bool						doubleQuoted(const std::string s);
		bool						strNeedsNoSemiColon(std::string& input);
		bool						checkFileType(std::string s, std::string end);
		bool						validHttpStatusCode(int	num);

		// FINDING
		bool						findSubstring(const std::string& inputString, const std::string& toFind);

		// FREEING
		void						freeCharArray(char** charArray, size_t size);

		// SPLITTING
		std::vector<std::string>	splitWordsTrimSpaces(std::string s);

		// TRIMMING
		void						trimDoubleQuotes(std::string& s);
		void						trimSpaces(std::string& s);
		void						trimFrontSpace(std::string& s);
		void						trimBackSpace(std::string& s);
		void						trimSpacesAndFinalSemiColon(std::string& s);
		void						trimSpacesAndFinalBracket(std::string& s);
		std::list<std::string>		listSplitStrings(const std::string& s, const char delim);

		// OUTPUT
		template<typename T>
		void						outputVector(std::vector<T> vec)
		{
			for (typename std::vector<T>::iterator it = vec.begin(); it < vec.end(); it++)
				//std::cout << "\"" << *it << "\"" << " | ";
				NULL;
		}
		template<typename Key, typename Value>
		void						outputMap(std::map<Key, Value> Map)
		{
			for (typename std::map<Key, Value>::iterator it = Map.begin(); it != Map.end(); it++)
				//std::cout << "first: " << it->first << " " << "second: " << it->second << std::endl;
				NULL;
		}
};

#endif