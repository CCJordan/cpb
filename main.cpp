//
//  main.cpp
//  cpProgressBar
//
//  Created by Christopher Jordan on 25.09.15.
//  Copyright © 2015 Christopher Jordan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <regex>
#include <cstring>
#include <string>
#include <sstream>
#include <list>
#include <unistd.h>
#include <glob.h>
#include "copyFileSet.h"
#include "copyHelper.h"

#define GetCurrentDir getcwd

using namespace std;

const int BARLENGTH = 40;

void copyFile( string src, string dst );
void drawProgressBar( int milage, string filename );
string getCurrentPath();
list<string> getInputFiles( string src, string dest );
list<string> globPath( string path );
void printHelp( string cmdName );
void printUsage( string cmdName );
vector<string> split(string str, char delimiter);
bool hasTheSameEnd(string str1, string str2);
string matchPaths(string relPath, string absPath);

unsigned int blocksize = 1024 * 1024 * 10;
bool breakForNewFile = false;

int main(int argc, const char * argv[]) {
    string destination;
    char *dest = new char[PATH_MAX];

    realpath(argv[argc - 1], dest);
    destination = dest;
    // check is the end of argv[2] equal to the end of dest?
    // if not, do something about it.
    if (!hasTheSameEnd(argv[argc - 1], destination)) {
        destination = matchPaths(argv[argc - 1], destination);
    }
    // else: proced with normal operation.
    int lastSwitchFound = 1;
    
    // check for switches or a to short argument list
    if (argc < 3) {
        printUsage( copyHelper::getFileName(argv[0]) );
        return 0;
    }
    
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i],"-b") == 0) {
                if (argc >= i + 1) {
                    blocksize = atoi(argv[i + 1]);
                    lastSwitchFound = i + 1;
                }
            } else if (strcmp(argv[i],"-n") == 0) {
                breakForNewFile = true;
            } else if (strcmp(argv[i],"-h") == 0) {
                printHelp( copyHelper::getFileName(argv[0]) );
                return 0;
            } else {
                cout << "illegal option: " << argv[i] << endl;
                printUsage( copyHelper::getFileName(argv[0]) );
                return 0;
            }
        }
    }
    
    // switches:
    // -h / --help      shows help screen
    // -b               Blocksize to be read and writen on each cycle
    // -n               break line for every new file
    
    // check if destination exists
    if (copyHelper::is_file(dest) && argc > 3) {
        cout << "Destination is a file, you have multiple sources..." << endl;
    } 
    
    string wd = getCurrentPath();
    list<copyFileSet> toCopy;
    
    for( int i = lastSwitchFound; i < argc - 1; i++ ) {
        string src = argv[i];

        list<string> paths = getInputFiles(src, destination);
        for(string path:paths) {
            copyFileSet *tmp = new copyFileSet( destination );
            tmp->addFile(path);
            toCopy.push_back(*tmp);
        }
    }
    
    for (copyFileSet set:toCopy) {
        do {
            string fileName = set.getSourcePath();
            string outPath = set.getDestinationPath();
            
            if (copyHelper::is_file(fileName.c_str())) {
                const char * targetDir = copyHelper::getDirName(outPath).c_str();
                if (!copyHelper::is_dir(targetDir)) {
                    mkdir(targetDir, 0755);
                }
                
                copyFile(fileName, outPath);
            } else if (copyHelper::is_dir(fileName.c_str())) {
                bool dstDirExists = copyHelper::is_dir(outPath.c_str());
                if ( !dstDirExists ) {
                    cout << "Creating dir " << outPath << endl;
                    mkdir(outPath.c_str(), 0755);
                }
            } else {
                cout << "The file " << fileName << " does not seem to exist." << endl;
            }
        } while(set.nextFile());
    }
    cout << "\n";
    return 0;
}

vector<string> split(string str, char delimiter) {
    vector<string> internal;
    stringstream ss(str);
    string tok;
    
    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }
    
    return internal;
}

bool hasTheSameEnd(string str1, string str2) {
    vector<string> relPathVec = split(str1, '/');
    vector<string> absPathVec = split(str2, '/');
    return relPathVec.back() == absPathVec.back();
}


// @param relPath:    relative path to be attachet to the absolute path
// @param absPath:    absolute path to be extended
string matchPaths(string relPath, string absPath) {
    vector<string> relPathVec = split(relPath, '/');
    // remove leading ".." and "." dirs
    while (relPathVec.front() == ".." || relPathVec.front() == ".") {
        relPathVec.erase(relPathVec.begin());
    }
    
    vector<string> absPathVec = split(absPath, '/');
    absPathVec.erase(absPathVec.end() - 1);
    
    vector<string>::iterator relPathIterator = relPathVec.begin();
    vector<string>::iterator absPathIterator = absPathVec.end() - 1;
    while(*relPathIterator != *absPathIterator) {
        absPathVec.erase(absPathIterator);
        absPathIterator--;
    }
    string returnPath = "";
    for(vector<string>::iterator i = absPathVec.begin() + 1; i <= absPathIterator; i++) {
        returnPath += "/" + *i;
    }
    
    vector<string>::iterator iEnd = relPathVec.end() - 1;
    for(vector<string>::iterator i = relPathVec.begin() + 1; i <= iEnd; i++) {
        returnPath += "/" + *i;
        if (i != iEnd) {
            mkdir(returnPath.c_str(), 0755);
        }
    }
    
    return returnPath;
}

void copyFile(string src, string dst) {
    ifstream infile;
    infile.open(src, ios::binary | ios::in);
    ofstream output;
    output.open(dst, ios::out | ios::trunc | ios::binary );
    string fileName = copyHelper::getFileName( src );
    if (output.is_open() && infile.is_open()) {
        unsigned int bs = blocksize;
        char * memblock;
        memblock = new char[bs];

        long long fileSize = copyHelper::getFileSize( infile );
        long long restSize = fileSize;
        int lastLenght = 0;
        
        while( restSize > 0 ) {
            if (restSize - bs < 0) {
                bs = (unsigned int) restSize;
            }
            infile.read(memblock, bs);
            output.write(memblock, bs);
            restSize -= bs;
            int milage = ((double) (fileSize - restSize) / (double) fileSize) * BARLENGTH;
            if (lastLenght < milage) {
                drawProgressBar(milage, fileName);
                lastLenght = milage;
            }
        }
        delete [] memblock;
    }
    infile.close();
    output.close();
    if (breakForNewFile)
        cout << "\n";
}

string getCurrentPath() {
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
        char *nillValue = new char[1];
        nillValue[0] = '\0';
        return nillValue;
    }
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

    return cCurrentPath;
}

void drawProgressBar(int milage, string filename) {
    static unsigned long maxLength;
    if ( maxLength < filename.length() ) {
        maxLength = filename.length();
    }
    // carrige return.
    cout << "\r";
    cout << "[";
    for (int i = 0; i < BARLENGTH; i++) {
        if ( i < milage ) {
        	cout << "=";
        } else {
        	cout << " ";
        }
    }
    cout << "] ";
    
    // write filename
    cout << filename;
    
    // overwrite possible older filenames
    cout << string(maxLength - filename.length(), ' ');
    cout.flush();
}

list<string> getInputFiles(string src, string dest) {
    list<string> paths = globPath(src);
    return paths;
}

list<string> globPath( string path ) {
    glob_t globbuf;
    
    globbuf.gl_offs = 0;
    glob(path.c_str(), GLOB_BRACE | GLOB_TILDE | GLOB_MARK, NULL, &globbuf);
    list<string> paths;
    for(int i = 0; i < globbuf.gl_pathc; i++) {
        char * tmpPath = globbuf.gl_pathv[i];
        char * absPath = new char[PATH_MAX];
        realpath(tmpPath, absPath);
        paths.push_back(absPath);
        delete [] tmpPath;
        delete [] absPath;
    }
    //  globfree(&globbuf);
    return paths;
}

void printHelp( string cmdName ) {
    printUsage(cmdName);
    cout << "-b     Blocksize:  The number of bytes read and writen in a single cycle." << endl;
    cout << "                   The large the blocks are, the less position changes are there for the HDD head," << endl;
    cout << "                   you should also consider the read block will be stored in RAM." << endl;
    cout << "-n     Break for new files" << endl;
    cout << "                   After each file transfer a linebreak will be writen," << endl;
    cout << "                   so you get a history of all transfered files in your shell." << endl;
    cout << "-h     Shows this screen" << endl;
}

void printUsage( string cmdName ) {
    cout << "Usage: " << endl;
    cout << cmdName << "[-b <number of bytes>] [-n] <source file / dir> <destination file / dir>" << endl;
    cout << cmdName << "[-b <number of bytes>] [-n] <source file / dir> <source file / dir> ... <destination file / dir>" << endl;
    cout << cmdName << " -h" << endl;
    cout << endl;
}
