//
//  copyFileSet.cpp
//  cpProgressBar
//
//  Created by Christopher Jordan on 26.09.15.
//  Copyright © 2015 Christopher Jordan. All rights reserved.
//

#include "copyFileSet.h"

copyFileSet::copyFileSet(string destination) {
    dest = destination;
    copiedFiles = 0;
    destIsFile = !copyHelper::is_dir(dest.c_str());
    if (!destIsFile) {
        if (dest[dest.length() - 1] != '/')
            dest = dest + '/';
    }
}

void copyFileSet::addFile(string srcPath) {
    files = addDirs(srcPath);
    filesInSet = files.size();
    arrFiles = new string[filesInSet];
    int i = 0;
    for(string file:files) {
        arrFiles[i] = file;
        i++;
    }
    it = files.begin();
    if (copyHelper::is_dir(srcPath.c_str())) {
        rootDir = copyHelper::getDirName(srcPath);
    } else {
        char * cAbsSrc = new char[PATH_MAX];
        realpath(srcPath.c_str(), cAbsSrc);
        string strAbsSrc = cAbsSrc;
        string fileName = copyHelper::getFileName(srcPath);
        rootDir = strAbsSrc.erase(strAbsSrc.length() - fileName.length(), fileName.length());
    }
    if (destIsFile && files.size() > 1) {
        cout << "WTF?! Mehrere Dateien in einer zu überschreiben ist jetzt nicht so der Bringer. " << endl;
    }
}

string copyFileSet::getSourcePath() {
    if (!endReached())
        return *it; // arrFiles[copiedFiles];
    return NULL;
}

string copyFileSet::getDestinationPath() {
    if (endReached())
        return NULL;
    
    // destination is a file
    if (destIsFile) {
        return dest;
    }
    
    string ret = *it; // arrFiles[copiedFiles];
    ret.erase(0, rootDir.length());
    ret = dest + ret;
    return ret;
}

bool copyFileSet::endReached() {
    bool end = (copiedFiles >= filesInSet);
    return end;
}

bool copyFileSet::nextFile() {
    copiedFiles++;
    if (!endReached()) {
        it++;
        return true;
    }
    return false;
}

list<string> copyFileSet::addDirs(string path) {
    list<string> paths;
    paths.push_back(path);
    
    list<string> absolutePaths;
    char *resName = new char[PATH_MAX];
    string tmpAbsPath;
    for (string strTmpPath:paths) {
        if (realpath(strTmpPath.c_str(), resName) != NULL) {
            tmpAbsPath = resName;
        }
        if (copyHelper::is_dir(tmpAbsPath.c_str())) {
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir( tmpAbsPath.c_str() )) != NULL) {
                absolutePaths.push_back(tmpAbsPath);
                while ((ent = readdir (dir)) != NULL) {
                    string tmpFileName = ent->d_name;
                    if (tmpFileName.compare(".") != 0 && tmpFileName.compare("..") != 0) {
                        string tmpFullPath = getFullFileName( tmpAbsPath, tmpFileName );
                        absolutePaths.push_back(tmpFullPath);
                        if (copyHelper::is_dir(tmpFullPath.c_str())) {
                            paths.push_back(tmpFullPath);
                        }
                    }
                }
                closedir (dir);
            } else {
                cout << "Can't access " << tmpAbsPath << endl;
            }
        } else {
            absolutePaths.push_back( tmpAbsPath );
        }
    }
    return absolutePaths;
}

string copyFileSet::getFullFileName(string outPathArg, string fileName) {
    if (outPathArg.length() > 0 && outPathArg[outPathArg.length() - 1] == '/') {
        return outPathArg + fileName;
    }
    // test if outPathArg is a folder
    if (copyHelper::is_dir(outPathArg.c_str())) {
        return outPathArg + "/" + fileName;
    }
    
    return outPathArg;
}
