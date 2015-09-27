//
//  copyHelper.cpp
//  cpProgressBar
//
//  Created by Christopher Jordan on 26.09.15.
//  Copyright © 2015 Christopher Jordan. All rights reserved.
//

#include "copyHelper.h"

bool copyHelper::is_dir(const char* path) {
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

bool copyHelper::is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

string copyHelper::getFileName( string path ) {

    size_t lastSlash = path.find_last_of("/");
    if ( lastSlash == string::npos )
        return path;
    lastSlash++;
    string name = path;
    name.erase(0, lastSlash);
    return name;
}

string copyHelper::getDirName(string path) {
    if (path.length() > 1) {
        if (path[path.length() - 1] == '/') {
            path.erase(path.length() - 1, 1);
        }
        string dirname = copyHelper::getFileName(path);
        path.erase(path.length() - dirname.length(), dirname.length());
        return path;
    } else {
        return path;
    }
}

long long copyHelper::getFileSize( ifstream &file ) {
    streampos begin,end;
    begin = file.tellg();
    file.seekg(0, ios::end);
    end = file.tellg();
    file.seekg(0, ios::beg);
    return end - begin;
}
