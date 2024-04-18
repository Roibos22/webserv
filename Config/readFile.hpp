#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream> 

class readFile
{
	public:
		readFile();
		readFile(std::string &file);
		~readFile();

		int					validName();
		int					error;
		void				errorHandling();
		std::string			fileInOneString;
		void				initialize(std::string &file);
		std::logic_error	def(const std::string& s);

	private:
		std::ifstream	infile;
		std::string		fileName;
};
