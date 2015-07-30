# SimpleTarProgram
Creates a tar like archive.

This program was created to make a simmillar archive to the unix command Tar. 
This program uses the Unix command line. To compile use:
```
c++ -c file.cpp
c++ jtar.cpp file.o
```

To run the program use 
```
  tar -cf archive.tar foo bar  # Create archive.tar from files foo and bar.
  tar -tf archive.tar          # List all files in archive.tar verbosely.
  tar -xf archive.tar          # Extract all files from archive.tar.
```
