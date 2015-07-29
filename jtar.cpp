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
//typedef char largeBuffer[2056];

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

void runCF(int argc, char* argv[])
{
    string tarPath = argv[2];
    vector<string> listOfAddresses;
    vector<File*> listOfFiles;
    //cout << "CF" << endl;
    for(int x=1; x < argc-2; x++)
        listAll(argv[x+2],listOfAddresses);



    for(int x =0; x<listOfAddresses.size(); x++)
    {
        //cout << listOfAddresses[x] << endl;
        struct stat st;
        stat(listOfAddresses[x].c_str(), &st);
        listOfFiles.push_back(new File(listOfAddresses[x].c_str(), to_string(st.st_mode).c_str(), 
                                      to_string(st.st_size).c_str(), 
                                      convertTime(st.st_mtime).c_str()));
    }
    //cout << "-----------" << endl;


    fstream outfile (tarPath.c_str(), ios::out | ios::binary);
    File buffer;

    for(int x =0; x<listOfFiles.size(); x++)
    {
        buffer = *listOfFiles[x];
        if(isDir(buffer.getName()))
            buffer.flagAsDir();
    
        //cout << buffer.getName() << endl;
        outfile.write((char *) &buffer, sizeof(buffer));
        //outfile.seekp(sizeof(buffer), ios::cur);
        if(!buffer.isADir())
        {
            fstream  file(buffer.getName().c_str(), ios::in);
            outfile <<  file.rdbuf();

        }

    }
    outfile.close();

}


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
    

    //cout << s << endl;
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
        //string allFiles = to_string(system("ls"));
        //cout << allFiles << endl;
    }
}


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



template <class T>
inline string to_string (const T& t)
{
    stringstream ss;
    ss << t;
    return ss.str();
}

bool isDir(string file)
{
    struct stat st;
    stat(file.c_str(), &st);
    return S_ISDIR(st.st_mode);

}

bool fileExists(const std::string& file) 
{
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

void runTF(string tarPath)
{
	//cout << "TF" << endl;
    File buff;
    vector<File> listOfFiles;
    fstream  file(tarPath.c_str(), ios::in | ios::out | ios::binary);
    while(file.read ((char *) &buff, sizeof(buff)))
    {
        
        listOfFiles.push_back(buff);
        if(!buff.isADir())
        {
            char largeBuffer[atoi(buff.getSize().c_str())];
            file.read((char *) &largeBuffer, sizeof(largeBuffer));
            //cout << buff.getName() << endl << largeBuffer << endl;
        }


        //cout << buff.getName() <<endl;
        //file.seekp(sizeof(buff), ios::cur);
    }
    file.clear();

    //cout  << "output" << endl;
    for(int x = 0; x<listOfFiles.size(); x++)
    {
        cout << listOfFiles[x].getName() << endl;
    }

}

void runXF(string tarPath)
{
	//cout << "XF" << endl;
    File buff;
    vector<File> listOfFiles;
    fstream  infile(tarPath.c_str(), ios::in | ios::out | ios::binary);
    while(infile.read ((char *) &buff, sizeof(buff)))
    {
        
        listOfFiles.push_back(buff);
        if(!buff.isADir())
        {
            fstream  outfile(buff.getName().c_str(), ios::out);
            char largeBuffer[atoi(buff.getSize().c_str())];
            infile.read((char *) &largeBuffer, sizeof(largeBuffer));
            outfile  << largeBuffer << endl;
            system(("touch -t " + buff.getStamp() + " " + buff.getName()).c_str());
        }
        else
        {
            if(!fileExists(buff.getName()))
                system(("mkdir " + to_string(buff.getName())).c_str());
        }


        //cout << buff.getName() <<endl;
        //file.seekp(sizeof(buff), ios::cur);
    }
    infile.clear();

    /*
    cout  << "output" << endl;
    for(int x = 0; x<listOfFiles.size(); x++)
    {
        cout << listOfFiles[x].getName() << endl;
    }
    */  


}