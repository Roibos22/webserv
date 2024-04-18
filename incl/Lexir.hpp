#ifndef LEXIR_HPP
#define LEXIR_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include "readFile.hpp"
#include <vector>
#include <exception>
#include <stdexcept>
#include <list>
#include <string>

#ifndef DEBUG_PARSE
#define DEBUG_PARSE 0
#endif

class Lexir : public readFile
{
	public:
		Lexir();
		Lexir(std::string &file);
		~Lexir();
		void	cutComments(std::string &s);
		int		lineNeededToParse(std::string line);
		void	listedFileContent();
		void	cleanUpFileContent();

	// getters
		std::string	getFileContentSpace();

	protected:
		readFile				rf;
		sockaddr				addr;
		std::list<std::string>	fileContent;
		std::logic_error			LineNotValid(const std::string& s);
};

#endif