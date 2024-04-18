/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helauren <helauren@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 14:06:25 by helauren          #+#    #+#             */
/*   Updated: 2024/04/05 13:56:36 by helauren         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "readFile.hpp"
#include "Utils.hpp"

using std::cerr;
using std::cout;
using std::endl;

void	readFile::initialize(std::string &file)
{
	this->error = 0;
	this->fileName = file;
	if(this->validName())
		return ;
	this->infile.open((char *)this->fileName.c_str());
	if(this->infile.is_open())
	{
		Utils().storeFileContent(this->infile, this->fileInOneString);
		this->infile.close();
		if(this->infile.fail())
			this->error = 2;
	}
	else
		this->error = 1;
}

readFile::readFile()
: error(1)
{
}

readFile::readFile(std::string &file)
{
	this->initialize(file);
	if(this->error)
		throw this->def("Something wrong with your file:\n- Double check the path\n- Make sure it ends in .cfg or .conf");
}

readFile::~readFile()
{
}

std::logic_error	readFile::def(const std::string& s)
{
	std::logic_error	ret(s);
	return (ret);
}

int	readFile::validName()
{
	int len = this->fileName.length();
	if(len < 5)
		return (this->error = 4, 1);
	int	dot = this->fileName.find_last_of('.');
	if(this->fileName.compare(dot, this->fileName.length(), ".cfg") == 0)
		return (this->error = 0, 0);
	if(this->fileName.compare(dot, this->fileName.length(), ".conf") == 0)
		return (this->error = 0, 0);
	return (this->error = 3, 1);
}

void	readFile::errorHandling()
{
	switch (this->error)
	{
		case 1:
			std::cerr << "Could not open file\n";
			break ;
		case 2:
			std::cerr << "Could not close file\n";
			break ;
		case 3:
			std::cerr << "File type is not valid: requires .cfg or .conf\n";
			break ;
		case 4:
			std::cerr << "File name is not valid, it's definitely too short, come one make an effort !\n";
			break ;
	}
}
