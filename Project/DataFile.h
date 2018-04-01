/*
(C) 2004 - 2008 Thomas Bleeker
MadWizard.org

Released under the BSD license with the exception of parts of the code
not written by Thomas Bleeker (www.madwizard.org), which are marked in
the source code. See license.txt for details.
*/

#ifndef _CG_DATAFILE_H_
#define _CG_DATAFILE_H_

#include <fstream>
#include <string>


// Serialization class
class DataFile
{
public:
	// Create a new file for reading (writing=false) or writing (writing=true)
	// Throws Error if something goes wrong
	DataFile(const char *pFilename, bool writing);
	virtual ~DataFile(void);

	// Returns true if the data file is writing, false if reading
	bool writing();

	/* Exchange overloads for simple variables and arrays. When reading, the variable
	pointed to is filled with the data read. When writing, the data pointed to is
	written to file. They can all throw Error on errors. */
	void exchange(float *pF);
	void exchange(int *pI);
	void exchange(unsigned int *pI);
	void exchange(bool *pBool);
	void exchange(char *pCa, size_t len);
	void exchange(int *pIa, size_t len);
	void exchange(float *pFa, size_t len);

	// Reads/writes a string. maxLen is the maximum charachter count to read or write
	void exchange(std::string *pStr, int maxLen);

	// Error exception class
	class Error {
	public:
		Error(const char *pMessage)
		{
			_msg = pMessage;
		}
		std::string what()
		{
			return _msg;
		}
	private:
		std::string _msg;
	};

private:
	// Internal functions to read/write and check for errors
	void checkedExchange(void *pData, size_t len);
	void checkedRead(void *pData, size_t len);
	void checkedWrite(const void *pData, size_t len);

	std::fstream	_file;		// The file reading from/writing to
	bool			_writing;	// Reading or writing?
};


#endif