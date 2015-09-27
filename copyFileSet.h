//
//  copyFileSet.h
//  cpProgressBar
//
//  Created by Christopher Jordan on 26.09.15.
//  Copyright © 2015 Christopher Jordan. All rights reserved.
//

#ifndef __cpProgressBar__copyFileSet__
#define __cpProgressBar__copyFileSet__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <queue>
#include <dirent.h>
#include "copyHelper.h"

using namespace std;

class copyFileSet {
private:
    // fields
    string rootDir;
    list<string> files;
    string *arrFiles;
    unsigned long filesInSet;
    list<string>::iterator it;
    string dest;
    bool destIsFile;
    int copiedFiles;
    
    // functions
    list<string> getInputFiles(string src, string dest);
    bool containsRegex(string str, list<regex> &rgxs);
    string getFullFileName(string outPathArg, string fileName);
    list<string> addDirs(string path);
    
public:
    copyFileSet(string destination);
    void addFile(string absPath);
    string getSourcePath();
    string getDestinationPath();
    bool endReached();
    bool nextFile();
};

#endif /* defined(__cpProgressBar__copyFileSet__) */
