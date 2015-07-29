#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
using namespace std;

int main(int argc, char ** argv)
{
        struct stat buf;
        struct utimbuf timebuf;
        char *token;

        for (int i = 1;  i < argc;  i++)
        {
                lstat (argv[i], &buf);
                if (S_ISREG(buf.st_mode))
                {
                  cout << argv[i] << ", ";
                  cout << "regular";
                  cout << ", size = " << buf.st_size << endl;


                  timebuf.actime = buf.st_atime;
                  timebuf.modtime = buf.st_mtime;


                  utime (argv[i], &timebuf);
                }
                else if (S_ISDIR(buf.st_mode))
                {
                  cout << argv[i] << ", ";
                  cout << "directory" << endl;
                }
        }
        return 0;
}

