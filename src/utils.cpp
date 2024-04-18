/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helauren <helauren@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 14:42:35 by helauren          #+#    #+#             */
/*   Updated: 2024/04/09 17:27:50 by helauren         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

using std::endl;
using std::cout;

Utils ut1;

Utils::Utils()
{
}

Utils::~Utils()
{
}

// CONVERSIONS

std::string	Utils::intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

char**	Utils::convertStringVectorToCharArray(std::vector<std::string>& stringVector)
{
	char** charArray = new char*[stringVector.size()];
	std::string	indivString;
	
	for (size_t i = 0; i < stringVector.size(); ++i)
	{
		charArray[i] = new char[stringVector[i].length() + 1];
		for (size_t j = 0; j < stringVector[i].size(); j++)
		{
			charArray[i][j] = stringVector[i][j];
		}
		charArray[i][stringVector[i].length()] = 0;
	}
	return charArray;
}

std::vector<std::string> Utils::convertStringArrayToStringVector(const char* stringArray[])
{
	std::vector<std::string> stringVector;
	for (int i = 0; stringArray[i] != NULL; ++i)
	{
		stringVector.push_back(stringArray[i]);
	}
	return stringVector;
}

// FINDING

bool Utils::findSubstring(const std::string& inputString, const std::string& toFind)
{
	for (size_t i = 0; i <= inputString.length() - 4; ++i)
	{
		if(i + toFind.length() > inputString.length())
			return (false);
		if (inputString.substr(i, toFind.length()) == toFind) {
			return (true);
		}
	}
	return (false);
}

// FREEING

void	Utils::freeCharArray(char** charArray, size_t size)
{
	for (size_t i = 0; i < size; ++i)
		delete[] charArray[i];
	delete[] charArray;
}

// COUNT

int	Utils::countChar(const std::string s, char c)
{
	size_t	it = 0;
	int	count = 0;

	while(1)
	{
		it = s.find_first_of(c, it);
		if(it == std::string::npos)
			break;
		it++;
		count++;
	}
	return (count);
}

int Utils::count_words(const std::string& input)
{
	std::stringstream ss(input);
	std::string word;
	bool		inQuotes = false;

	int count = 0;
	while (1)
	{
		ss >> word;
		if(word.empty() == true)
			break;
		if(*word.begin() == '"')
			inQuotes = true;
		if(inQuotes == true)
			if(word.length() > 1)
				if (word[word.length() -1] == '"')
					inQuotes = false;
		word.clear();
		if(inQuotes == false)
		{
			count++;
		}
	}
	return (count);
}

int	Utils::countDotDotSlash(const std::string& str, size_t& pos)
{
	int count = 0;
	size_t	found = 0;
	while ((found = str.find("../", found)) != std::string::npos)
	{
		++count;
		found += 3;
		pos = found;
	}
	return	count;
}

bool Utils::strNeedsNoSemiColon(std::string& input)
{
	ut1.trimSpaces(input);
	if(input.length() == 1 && (input == "{" || input == "}"))
		return (true);
	if(Utils::findSubstring(input, "http") == true || Utils::findSubstring(input, "server") == true || Utils::findSubstring(input,"location") == true || Utils::findSubstring(input,"limit_except") == true)
		return (true);
	if(input[input.length() -1] == '{' || input[input.length() -1] == '}')
		return (true);
	return (false);
}

bool	Utils::doubleQuoted(const std::string s)
{
	std::string	copy(s);
	ut1.trimSpaces(copy);
	if(s.length() < 2)
		return (false);
	if(s[0] != '\"' && s[s.length() -1] != '\"')
		return (false);
	return(true);
}

std::vector<std::string>	Utils::splitWordsTrimSpaces(std::string s)
{
	ut1.trimSpaces(s);
	std::vector<std::string> ret;
	std::stringstream		ss(s);
	std::string				word;

	ss >> word;
	while(word.empty() == false)
	{
		ret.push_back(word);
		word.clear();
		ss >> word;
		ut1.trimSpaces(word);
	}
	return (ret);
}

void	Utils::trimSpaces(std::string& s)
{
	while(s.length() && std::isspace(s[0]))
		s.erase(s.begin());
	while(s.length() && std::isspace(s[s.length() -1]))
		s = s.substr(0, s.length() -1);
}

void	Utils::trimDoubleQuotes(std::string& s)
{
	ut1.trimSpaces(s);
	if(s.length() > 0 && (s[0] == '"'))
		s.erase(s.begin());
	if(s.length() > 0 && (s[s.length() -1] == '"'))
		s = s.substr(0, s.length() -1);
	ut1.trimSpaces(s);
}

void	Utils::trimSpacesAndFinalSemiColon(std::string& s)
{
	ut1.trimSpaces(s);
	if(s.length() > 0 && s[s.length() -1] == ';')
		s = s.substr(0, s.length() -1);
	ut1.trimSpaces(s);
}

void	Utils::trimSpacesAndFinalBracket(std::string& s)
{
	ut1.trimSpaces(s);
	if(s.length() > 0 && (s[s.length() -1] == '{' || s[s.length() -1] == '}'))
		s = s.substr(0, s.length() -1);
	ut1.trimSpaces(s);
	if(s.length() > 0 && s[s.length() -1] == '}')
		s = s.substr(0, s.length() -1);
	ut1.trimSpaces(s);
}

void	Utils::trimFrontSpace(std::string& s)
{
	while(s.length() && std::isspace(s[0]))
		s.erase(s.begin());
}

void	Utils::trimBackSpace(std::string& s)
{
	while(s.length() && std::isspace(s[s.length() -1]))
		s = s.substr(0, s.length() -1);
}

std::list<std::string> Utils::listSplitStrings(const std::string& s, const char delim)
{
	std::stringstream ss;
	std::string	test;
	std::list<std::string>	list;

	ss << s;
	test = ss.str();
	while(1)
	{
		std::getline(ss, test, delim);
		ut1.trimSpaces(test);
		list.push_back(test);
		if(ss.eof())
			break ;
	}
	return (list);
}

void	Utils::storeFileContent(std::ifstream &file, std::string& fileContent)
{
	std::string	buffer;
	std::string	prev;
	std::string	s1;
	std::string	s2;

	prev = fileContent;
	std::getline(file, buffer, '\0');
	fileContent.append(buffer);
}

bool	Utils::checkFileType(std::string s, std::string end)
{
	if(s.length() < end.length() +1)
		return (false);
	std::string	strEnd = s.substr(s.length() - end.length(), end.length());
	if(strEnd == end)
		return (true);
	return (false);
}
	
bool	Utils::validHttpStatusCode(int	num)
{
	int	validCodes[] =
	{
		100, 101, 102, 103, 122, 200, 201, 202, 203, 204,
		205, 206, 207, 208, 226, 300, 301, 302, 303, 304,
		305, 306, 307, 308, 400, 401, 402, 403, 404, 405,
		406, 407, 408, 409, 410, 411, 412, 413, 414, 415,
		416, 417, 418, 419, 420, 421, 422, 423, 424, 425,
		426, 428, 429, 431, 444, 449, 450, 451, 494, 495,
		496, 497, 498, 499, 500, 501, 502, 503, 504, 505,
		506, 507, 508, 509, 510, 511, 598, 599
	};
	size_t	maxValue = sizeof(validCodes) / sizeof(int);
	for (size_t it = 0; it != maxValue; it++)
		if(validCodes[it] == num)
			return (true);
	return (false);
}

int	Utils::checkDoublesVectorString(std::vector<std::string> og)
{
	std::map<int, std::string> copy;
	int i = 0;
	for(std::vector<std::string>::iterator it = og.begin(); it != og.end(); it++, i++)
	{
		for(std::map<int, std::string>::iterator it_copy = copy.begin(); it_copy != copy.end(); it_copy++)
			if(it_copy->second == *it)
				return (1);
		copy[i] = *it;
	}
	return (0);
}
