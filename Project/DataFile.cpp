/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#include "DataFile.h"
#include <fstream>
#include "AutoArr.h"
#include <string>
#include "util.h"

using namespace std;

DataFile::DataFile(const char *pFilename, bool writing)
{
	// Open file
	_writing = writing;
	ios_base::open_mode mode = fstream::binary;
	mode |= (_writing) ? (fstream::out) : (fstream::in);
	_file.open(pFilename, mode);
	if (!_file)
		throw Error("Opening file failed.");
}

DataFile::~DataFile(void)
{
}

void DataFile::exchange(float *pF)
{
	checkedExchange(pF, sizeof(float));
}

void DataFile::exchange(int *pI)
{
	checkedExchange(pI, sizeof(int));
}

void DataFile::exchange(bool *pBool)
{
	if (_writing)
	{
		int i = (*pBool) ? 1 : 0;
		checkedWrite(&i, sizeof(int));
	}
	else
	{
		int i;
		checkedRead(&i, sizeof(int));
		*pBool = (i != 0);
	}
}

void DataFile::exchange(unsigned int *pI)
{
	checkedExchange(pI, sizeof(unsigned int));
}

void DataFile::exchange(char *pCa, size_t len)
{
	checkedExchange(pCa, len);
}

void DataFile::exchange(int *pIa, size_t len)
{
	checkedExchange(pIa, sizeof(int) * len);
}

void DataFile::exchange(float *pFa, size_t len)
{
	checkedExchange(pFa, sizeof(float) * len);
}


void DataFile::exchange(std::string *pStr, int maxLen)
{
	// Read or write a string
	if (_writing)
	{
		int len = minVal(static_cast<int>(pStr->size()), maxLen);
		checkedWrite(&len, sizeof(int));
		checkedWrite(pStr->c_str(), len);
	}
	else
	{
		// First read an AutoArr (safe in case of exceptions)
		int len;
		checkedRead(&len, sizeof(int));
		len = minVal(len, maxLen);
		AutoArr<char> arr(len + 1);
		checkedRead(arr.get(), len);
		arr[len] = 0;
		// Convert AutoArr to string
		*pStr = std::string(arr.get());
	}
}

void DataFile::checkedExchange(void *pData, size_t len)
{
	if (_writing)
	{
		checkedWrite(pData, len);
	}
	else
	{
		checkedRead(pData, len);
	}
}

void DataFile::checkedRead(void *pData, size_t len)
{
	// Read and check for errors
	std::streampos startPos;

	startPos = _file.tellg();
	_file.read(reinterpret_cast<char*>(pData), static_cast<std::streamsize>(len));
	if (!_file)
		throw Error("Error or end of file while reading data.");
	if (_file.tellg() - startPos != len)
		throw Error("End of file reached too soon.");
}

void DataFile::checkedWrite(const void *pData, size_t len)
{
	// Write and check for errors
	_file.write(reinterpret_cast<const char*>(pData), static_cast<std::streamsize>(len));
	if (!_file)
		throw Error("Error while writing data.");
}

bool DataFile::writing()
{
	return _writing;
}