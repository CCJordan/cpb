//
//  copyHelper.h
//  cpProgressBar
//
//  Created by Christopher Jordan on 26.09.15.
//  Copyright © 2015 Christopher Jordan. All rights reserved.
//

#ifndef __cpProgressBar__copyHelper__
#define __cpProgressBar__copyHelper__

#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <dirent.h>
#include <fstream>
#include <list>

using namespace std;

class copyHelper {
public:
    static bool is_dir( const char* path );
    static bool is_file( const char* path );
    static string getFileName( string path );
    static string getDirName( string path );
    static long long getFileSize( ifstream &file );
    static string removeTrailingSlash( string path );
};

#endif /* defined(__cpProgressBar__copyHelper__) */
