#include "Lexir.hpp"
#include "Utils.hpp"

using std::endl;
using std::cout;

Utils	ut;

Lexir::Lexir()
{
}

Lexir::Lexir(std::string &file)
: readFile(file)
{
	this->listedFileContent();
}

Lexir::~Lexir()
{
}

int	Lexir::lineNeededToParse(std::string line)
{
	int	i = 0;

	while(line[i] && std::isspace(line[i]))
		i++;
	if(line[i] == '#')
		return (1);
	return (0);
}

void	Lexir::cutComments(std::string &s)
{
	for (int i = 0; s[i]; i++)
	{
		if(s.find('#') != std::string::npos)
		{
			s[i] = 0;
			return ;
		}
	}
}

std::logic_error	Lexir::LineNotValid(const std::string& s)
{
	std::string	output = "The following line is not valid :\n\"";
	output += s;
	output += "\"";
	std::logic_error	ret(output);
	return (ret);
}

void	Lexir::cleanUpFileContent()
{
	size_t	comm;

	for (std::list<std::string>::iterator i = this->fileContent.begin(); i != this->fileContent.end(); i++)
	{
		if((*i)[0] == '#' || (*i)[0] == '\n' || (*i)[0] == 0)
		{
			i = this->fileContent.erase(i);
			i--;
			continue;
		}
		comm = i->find('#');
		if(comm != std::string::npos)
			*i = i->substr(0, comm);
		ut.trimSpaces(*i);
		if((*i)[0] == '{')
		{
			std::list<std::string>::iterator previous = i;
			if(previous != this->fileContent.begin())
			{
				previous--;
				*previous += " {";
				if(i->length() == 1)
				{
					i = this->fileContent.erase(i);
					i--;
				}
				else
					*i = i->substr(1, i->length() -1);
			}
		}
		ut.trimSpaces(*i);
		char	c = (*i)[i->length() -1];
		bool	needsno = ut.strNeedsNoSemiColon(*i);
		if(needsno == false && c != ';')
			throw (this->LineNotValid(*i));
	}
}

void	Lexir::listedFileContent()
{
	this->fileContent = ut.listSplitStrings(this->fileInOneString, '\n');
	this->cleanUpFileContent();
	if(ut.countChar(this->fileInOneString, '{') != ut.countChar(this->fileInOneString, '}'))
		throw (this->def("Verify your brackets: unequal amout of open and closed brackets"));
	//cout << "FILE CONTENT:\n\n";
	for(std::list<std::string>::iterator it = this->fileContent.begin(); it != this->fileContent.end(); it++)
	{
		//std::cout << *it << std::endl;
	}
	//cout << std::endl;
}

std::string	Lexir::getFileContentSpace()
{
	std::string s;
	int			count = 0;

	for (std::list<std::string>::iterator i = this->fileContent.begin(); i != this->fileContent.end(); i++, count++)
	{
		s += *i;
		s += '\n';
	}
	return (s);
}
