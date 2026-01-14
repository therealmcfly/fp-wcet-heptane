
#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stdlib.h>

using namespace std;

typedef map<string, int> LatencyTableType;

class FileLoader {

 public:
   void loadDataLatency(string archiname, const string & dataPath);
   int GetLatencyDataValue(string vinstr);
 private :
   LatencyTableType LatencyTable;
};

#endif
