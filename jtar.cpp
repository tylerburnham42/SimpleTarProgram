//File: jtar.cpp
//Author: Tyler Burnham 
//Date: 07-29-2015
//Description: Takes in command line arguments and preforms the following actions: 
//  tar -cf archive.tar foo bar  # Create archive.tar from files.
//  tar -tf archive.tar          # List all files in archive.tar verbosely.
//  tar -xf archive.tar          # Extract all files from archive.tar.

#include "file.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>  
#include <dirent.h>
#include <vector>

typedef char String[80];

void printHelp();
void runCF(int argc, char* argv[]);
void runTF(string tarPath);
void runXF(string tarPath);
void listAll(string s, vector<string>& listOfAddresses);
string convertTime(time_t timeIn);
bool isDir(string file);
bool fileExists(const std::string& file);

template <class T>
string to_string (const T& t);

using namespace std;

//Takes command line arguments and runs the commands.
// -cf -- runCF
// -tf -- runTF
// -xf -- runXF
// --help -- Prints help file
int main(int argc, char* argv[])
{
    if(argc == 2)
    {
    	if(strcmp(argv[1], "--help") == 0)
    	{
			printHelp();
			return 0;
    	}
    	cout << "Incorrect Paremeters" << endl;
    	return 0;
    }
    else
    {
    	if(argc < 3)
        {
            cout << "Incorrect Paremeters" << endl;
    		return 1;
        }

		if(strcmp(argv[1], "-cf") == 0)
			runCF(argc, argv);
    	if(strcmp(argv[1], "-tf") == 0)
			runTF(argv[2]);
    	if(strcmp(argv[1], "-xf") == 0)
    		runXF(argv[2]);
    }

    return 0;
}

//Print the help file 
void printHelp()
{
	ifstream t;
	t.open("help");
	string line;
	while(t)
	{
		getline(t, line);
		cout << line << endl;
	}
	t.close();
}

//
void runCF(int argc, char* argv[])
{
    string tarPath = argv[2];
    vector<string> listOfAddresses;
    vector<File*> listOfFiles;

	//Takes all the remaining arguments adds them to list of addresses
    for(int x=1; x < argc-2; x++)
        listAll(argv[x+2],listOfAddresses);

	//loop through the list of addresses and create files
    for(int x =0; x<listOfAddresses.size(); x++)
    {
        struct stat st;
        stat(listOfAddresses[x].c_str(), &st);
        listOfFiles.push_back(new File(listOfAddresses[x].c_str(), to_string(st.st_mode).c_str(), 
                                      to_string(st.st_size).c_str(), 
                                      convertTime(st.st_mtime).c_str()));
    }

    fstream outfile (tarPath.c_str(), ios::out | ios::binary);
    File buffer;

	//loop through all the files, check if its a directory if it is not add the file contents after the file info
    for(int x =0; x<listOfFiles.size(); x++)
    {
        buffer = *listOfFiles[x];
        if(isDir(buffer.getName()))
            buffer.flagAsDir();
    
        outfile.write((char *) &buffer, sizeof(buffer));

        if(!buffer.isADir())
        {
            fstream  file(buffer.getName().c_str(), ios::in);
            outfile <<  file.rdbuf();
        }
    }
    outfile.close();
}

//Gets all the addresses and returns them in a vector. 
void listAll(string s, vector<string>& listOfAddresses)
{
    if(!fileExists(s))
    {
        cout << s << " Does not exist." << endl;
        return;
    }
    struct stat st;
    stat(s.c_str(), &st);
    listOfAddresses.push_back(s);
    
    if(isDir(s))
    {
        DIR* d = opendir(s.c_str());
        for(struct dirent *de = NULL; (de = readdir(d)) != NULL; )
        {
            if (strcmp (de->d_name, "..") != 0 &&
                strcmp (de->d_name, ".") != 0) 
            {
                listAll (s + '/' + de->d_name, listOfAddresses);
            }
        }
    }
}

//Converts Unix time to the time format returned
string convertTime(time_t timeIn)
{
    time_t t = timeIn;
    struct tm lt;
    localtime_r(&t, &lt);
    char timbuf[80];
    strftime(timbuf, sizeof(timbuf), "%Y%m%d%H%M.%S", &lt);
    string timeOut(timbuf, 80);
    return timbuf;
}


//Converts c_string to string
template <class T>
inline string to_string (const T& t)
{
    stringstream ss;
    ss << t;
    return ss.str();
}

//checks if the file is a directory 
bool isDir(string file)
{
    struct stat st;
    stat(file.c_str(), &st);
    return S_ISDIR(st.st_mode);

}

//checks if the file exists 
bool fileExists(const std::string& file) 
{
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

//Checks the contents of the file and prints it
void runTF(string tarPath)
{
    File buff;
    vector<File> listOfFiles;
    fstream  file(tarPath.c_str(), ios::in | ios::out | ios::binary);
	
	//Retrieves the files from the archive
	while(file.read ((char *) &buff, sizeof(buff)))
    {
        
        listOfFiles.push_back(buff);
        if(!buff.isADir())
        {
            char largeBuffer[atoi(buff.getSize().c_str())];
            file.read((char *) &largeBuffer, sizeof(largeBuffer));
        }
    }
    file.clear();
	
	//Loops through the files and prints them
    for(int x = 0; x<listOfFiles.size(); x++)
    {
        cout << listOfFiles[x].getName() << endl;
    }

}

//Extracts the tar archive and resets the file timestamps
void runXF(string tarPath)
{
    File buff;
    vector<File> listOfFiles;
    fstream  infile(tarPath.c_str(), ios::in | ios::out | ios::binary);
	
	//Loops through the archive
    while(infile.read ((char *) &buff, sizeof(buff)))
    {
		//Adds a file to buffer
        listOfFiles.push_back(buff);
		
		//if the file is not a directory then read the remaining file contents
		//Create the file and add the information
        if(!buff.isADir())
        {
            fstream  outfile(buff.getName().c_str(), ios::out);
            char largeBuffer[atoi(buff.getSize().c_str())];
            infile.read((char *) &largeBuffer, sizeof(largeBuffer));
            outfile  << largeBuffer << endl;
            system(("touch -t " + buff.getStamp() + " " + buff.getName()).c_str());
        }
		//If the file is a directory make the directory and add the information.
        else
        {
            if(!fileExists(buff.getName()))
                system(("mkdir " + to_string(buff.getName())).c_str());
        }
    }
    infile.clear();

}