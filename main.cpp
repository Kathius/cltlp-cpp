#include "stdio.h"

#include <iostream>
#include <list>
#include <string>
#include <map>
#include <set>

#include <pcrecpp.h>

#include "ALogLineProcessor.h"
#include "ARankCounter.h"
#include "ATextLogCollection.h"

int main(int argc, char *argv[])
{
	printf("Arindal Text Log Processor\n\n");
	
	bool bLoad=false;
	string arpath, character, endTimestamp;
	
	if (argc < 4)
	{
		FILE *pFile;
		if ((pFile = fopen("settings.cfg", "r")) == NULL)
		{
			printf("Usage: arlp -p \"PATH_TO_ARINDAL\" -c \"CHARACTERNAME\" [-s]\n");
			return -1;
		}
		bLoad=true;
		char buffer[255];
		fgets(buffer, 255, pFile);
		pcrecpp::RE("(.*)\\n").FullMatch(buffer, &arpath);
		fgets(buffer, 255, pFile);
		pcrecpp::RE("(.*)\\n").FullMatch(buffer, &character);
		fclose(pFile);
	}

	if (!bLoad)
	{
		bool bSave=false;
		for (int i=0;i<argc;i++)
		{
			if (!strcmp(argv[i], "-p"))
			{
				arpath = argv[i+1];
				i++;
			}else if (!strcmp(argv[i], "-c"))
			{
				character = argv[i+1];
				i++;
			}else if (!strcmp(argv[i], "-s"))
			{
				if (argc < 5)
				{
					printf("Usage: arlp -p \"PATH_TO_ARINDAL\" -c \"CHARACTERNAME\" [-s]\n");
					return -1;
				}
				bSave=true;
			}
		}
		if (bSave)
		{
			FILE *pFile;
			pFile = fopen("settings.cfg", "w");
			fwrite(arpath.c_str(), arpath.length(), 1, pFile);
			fwrite("\n", 1, 1, pFile);
			fwrite(character.c_str(), character.length(), 1, pFile);
			fwrite("\n", 1, 1, pFile);
			fclose(pFile);
		}
	}

    for (int i=0;i<argc;i++) {
        if (!strcmp(argv[i], "-e")) {
            endTimestamp = argv[i+1];
            i++;
        }
    }
	
	try
	{
		//ATextLogCollection tlc("e:/spiele/arindal", "Kathius");
		//ARankCounter rc("Kathius");
		ATextLogCollection tlc(arpath, character);
		ARankCounter rc(character);
		tlc.SetRankCounter(&rc);
		//string endTimestamp = "6/8/20 8:43:45p";
		tlc.ProcessFiles(endTimestamp);
		rc.PrintRanks();
	}catch (string str)
	{
		std::cout << str;
	}
	return 0;
}
