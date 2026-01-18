

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Utl.h"
#include "FileLoader.h"

/**
	 It loads a latency table ( instruction : latency) from the file
		 <dataPath>/<archiname>Latency.data if the dataPath is not equal to "%UNDEFINED%".

		 No effect when the dataPath is equal to "%UNDEFINED%".
*/
void FileLoader::loadDataLatency(string archiname, const string &dataPath)
{

	if (dataPath != "%UNDEFINED%")
	{
		// cout << " >>>>>>>Loading " << archiname  << " Data Latency" << endl;
		string namefile = dataPath + string("/") + string(archiname) + string("Latency.data");

		// cout << "[FILELOADER DEBUG] Loading " << archiname << " Data Latency from dataPath: " << dataPath << endl;
		// cout << "[FILELOADER DEBUG] Full file path: " << namefile << endl;

		ifstream vfile(namefile, ios::in);
		if (vfile.is_open())
		{
			string uneLigne;
			int n = 0;
			while (getline(vfile, uneLigne))
			{
				n++;
				if (uneLigne.length() != 0)
				{
					if (uneLigne[0] != '#')
					{
						vector<string> v = Utl::split(uneLigne, ":");
						if (v.size() == 2)
						{
							LatencyTable[v[0]] = Utl::string2int(v[1]);
							// cout << " Instruction = " << v[0] << " latence = " << v[1] << endl;
						}
						else
							cout << " Line num = " << n << " skipped." << endl;

						// cout << "[FILELOADER DEBUG] Loaded: '" << v[0] << "' -> " << v[1] << " cycles" << endl;
					}
				}
				else
					cout << " Line num = " << n << " skipped." << endl;
			}
			vfile.close();
		}
		else
			cout << "Cannot open the file " << namefile << endl;
	}
}

/**
	 @return the latency value assigned to an instruction (vinstr), 1 if not found.
	 Such a value is available after the call to FileLoader::loadDataLatency( ) method.
*/
int FileLoader::GetLatencyDataValue(string vinstr)
{
	LatencyTableType::iterator it;

	it = LatencyTable.find(vinstr);
	if (it == LatencyTable.end())
	{
		// cout << "[FILELOADER DEBUG] Instruction '" << vinstr << "' NOT FOUND in table, using _DEFAULT_VALUE_" << endl;
		it = LatencyTable.find("_DEFAULT_VALUE_");
		if (it == LatencyTable.end())
		{
			// cout << "[FILELOADER DEBUG] _DEFAULT_VALUE_ also NOT FOUND! Returning 1" << endl;
			return 1;
		}
		// cout << "[FILELOADER DEBUG] Using _DEFAULT_VALUE_ = " << it->second << endl;
	}
	return it->second;
}
