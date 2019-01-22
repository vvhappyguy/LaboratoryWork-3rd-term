#ifndef _CMD_
#define _CMD_

#include <iostream>
#include <cstddef>	
#include <cstring>
#include <fstream>
#include <vector>
#include <unistd.h>
#include "rec.hpp"


#define FName "addbook.bin"
#define BYTE char *

using namespace std;

class DB
{
  public:
	size_t _size;
	size_t count;
	static DB &Instance()
	{
		static DB theSingleInstance;
		return theSingleInstance;
	}

	void connect()
	{

		if (!fstream(FName).good())
		{
			cout << "Do you want to create new DB by name: " << FName << " (yes/no)" << std::endl;
			char *ans = new char[3];
			cin >> ans;
			if (strcmp(ans, "yes"))
			{
				delete ans;
				throw runtime_error("Bad filename of DB");
			}

			else
			{
				delete ans;
				ofstream ofs(FName);
				ofs.close();
			}
		}
		fstream fileDB(FName);
		fileDB.seekg(0, std::ios::end);
		_size = fileDB.tellg();

		this->count = _size / sizeof(Rec);
		fileDB.close();
		if (_size % sizeof(Rec) != 0)
		{
			throw runtime_error("Bad structure of DB file");
		}
	}

	ofstream *getOF()
	{
		if (!_ofs)
			_ofs = new ofstream(FName);
		return _ofs;
	}

	ifstream *getIF()
	{
		if (_ifs)
			_ifs = new ifstream(FName);
		return _ifs;
	}

	size_t getAddrN(size_t n)
	{
		return sizeof(Rec) * n;
	}

	void closeOF()
	{
		delete _ofs;
	}

	void closeIF()
	{
		delete _ifs;
	}

	const size_t getSize()
	{
		return this->Instance()._size;
	}

	void sizeP()
	{
		this->_size++;
	}

	void sizeM()
	{
		this->_size--;
	}

  private:
	ofstream *_ofs;
	ifstream *_ifs;

	DB(){};
	DB(const DB &root) = delete;
	DB &operator=(const DB &) = delete;
};

int add(const char *cmd)
{
	ushort words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 8)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	std::cout << "DBG1" << std::endl;
	delete pars_cmd;
	if (words.size() != 8)
	{
		cout << "Words.size() = " << words.size() << "\n";
		std::cout << "[ERR]: Bad command" << std::endl;
		return 1;
	}

	std::cout << "Creating new Rec" << std::endl;
	Rec *tmp_rec = new Rec;
	strncpy(tmp_rec->LastName, words[1].c_str(), 50);
	strncpy(tmp_rec->FirstName, words[2].c_str(), 50);
	strncpy(tmp_rec->FatherName, words[3].c_str(), 50);
	strncpy(tmp_rec->HB, words[4].c_str(), 10);
	strncpy(tmp_rec->Address, words[5].c_str(), 100);
	strncpy(tmp_rec->PhoneNumber, words[6].c_str(), 20);
	strncpy(tmp_rec->Note, words[7].c_str(), 4096);
	ofstream fo(FName, ios::binary | std::ios::app);
	fo.seekp((DB::Instance().count) * sizeof(Rec), std::ifstream::cur);
	fo.write((BYTE)tmp_rec, sizeof(Rec));
	fo.close();
	DB::Instance().count++;
	delete tmp_rec;
	return 0;
}

int list(const char *cmd)
{
	if (DB::Instance().count == 0)
		return 1;
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 3)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	std::cout << "DBG1dasd" << std::endl;
	delete pars_cmd;
	if (words.size() > 3)
	{
		cout << "Words.size() = " << words.size() << "\n";
		std::cout << "[ERR]: Bad command" << std::endl;
		return 1;
	}

	size_t i1, i2; // Two counters for limits

	if (words.size() == 1)
	{
		i1 = 0;
		i2 = DB::Instance().count;
	}
	else if (words.size() == 2)
	{
		i1 = stoul(words[1]);
		i2 = DB::Instance().count;
	}
	else if (words.size() == 3)
	{
		i1 = stoul(words[1]);
		i2 = stoul(words[2]);
	}
	Rec cout_rec;
	ifstream f2(FName, ios::binary | ios::in);
	std::cout << i1 << " " << i2 << std::endl;
	for (size_t i = 1; i <= DB::Instance().count; i++)
	{
		f2.read((BYTE)&cout_rec, sizeof(Rec));
		if ((i >= i1) && (i <= i2))
			std::cout << "Number [" << i << "]: "
					  << cout_rec.LastName << " " << cout_rec.FirstName << " "
					  << cout_rec.FatherName << " " << cout_rec.HB << " "
					  << cout_rec.Address << " " << cout_rec.PhoneNumber << " "
					  << cout_rec.Note << std::endl;
	}
	return 0;
}

int edit(const char *cmd)
{
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 4)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	delete pars_cmd;
	delete pch;

	size_t n = stoul(words[1].c_str());
	Rec *tmp_rec = new Rec();
	fstream f2(FName, ios::in | ios::out | ios::binary);
	f2.seekg((n - 1) * sizeof(Rec), std::ifstream::cur);
	f2.read((BYTE)tmp_rec, sizeof(Rec));

	char *tmp = getFieldbyName(words[2], tmp_rec);
	sprintf(tmp, "%s", words[3].c_str());
	f2.seekp((n - 1) * sizeof(Rec), ios::beg);
	f2.write((BYTE)tmp_rec, sizeof(Rec));
	f2.close();
	delete tmp_rec;
	return 0;
}

int del(const char *cmd)
{
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 2)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	delete pars_cmd;
	delete pch;

	size_t n = stoul(words[1].c_str());
	Rec *tmp_rec_del = new Rec();
	Rec *tmp_rec_last = new Rec();
	fstream f2(FName, ios::in | ios::out | ios::binary);
	f2.seekg((n - 1) * sizeof(Rec), ios::beg);
	f2.read((BYTE)tmp_rec_del, sizeof(Rec));
	f2.seekg((DB::Instance().count - 1) * sizeof(Rec), ios::beg);
	f2.read((BYTE)tmp_rec_last, sizeof(Rec));
	f2.seekp((n - 1) * sizeof(Rec), ios::beg);
	f2.write((BYTE)tmp_rec_last, sizeof(Rec));
	f2.seekg((DB::Instance().count - 1) * sizeof(Rec), ios::beg);
	truncate(FName, sizeof(Rec) * (DB::Instance().count - 1));
	DB::Instance().count--;
	f2.close();
	delete tmp_rec_del;
	delete tmp_rec_last;
	return 0;
}

int find(const char *cmd)
{
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{

		std::cout << "pch = " << pch
				  << "\twords.size() = " << words.size()
				  << "\tcounter: " << words_counter << std::endl;
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 3)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	delete pars_cmd;
	delete pch;

	Rec cout_rec;
	ushort num = 0;
	ifstream f2(FName, ios::binary | ios::in);
	for (size_t i = 1; i <= DB::Instance().count; i++)
	{
		f2.read((BYTE)&cout_rec, sizeof(Rec));
		char *tmp = getFieldbyName(words[1], &cout_rec);
		std::cout << tmp << " " << words[2].c_str() << std::endl;
		if (strncmp(tmp, words[2].c_str(), words[2].size()) == 0)
		{
			num = i;
			break;
		}
		if (i == DB::Instance().count)
			num = 0;
	}
	f2.close();
	if (num != 0)
		std::cout << "Find your field[" << num << "]" << std::endl;
	else
		std::cout << "No such field!" << std::endl;

	return 0;
}

int help()
{
	cout << "Help Inforamtion ..." << std::endl
		 << "\tadd <...> \n\t- for adding new Rec to DB. \n\t\t(example: add Dyakin Ivan Pavlovich 09.09.1999 Moscow @riokin www.github.com/vvhappyguy)\n"
		 << "\tedit <n> <fieldname> <val> \n\t- for editting <fieldName> field of <n> Rec from db by your value <val> \n\t\t(example: edit 1 Address SergievPosad)\n"
		 << "\tdel <n> \n\t- for deletting <n> Rec from DB \n\t\t(example: del 2)\n";
	;
	return 0;
}

#endif