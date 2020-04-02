#include "dirent.h"

#include <iostream>
#include <vector>
#include <list>

#include <pcrecpp.h>

#include "ALogLineProcessor.h"
#include "ATextLogCollection.h"

ATextLogCollection::ATextLogCollection(string Dir, string Char)
{
	ArindalRootDir = Dir;
	Character = Char;
	TextLogDir = Dir + "/data/Text Logs/" + Char;
}

ATextLogCollection::~ATextLogCollection()
{
}

void ATextLogCollection::SetRankCounter(ALogLineProcessor *p)
{
	rc = p;
}

void ATextLogCollection::ProcessFiles()
{
	if (rc == NULL) return;
	DIR *dir = opendir(TextLogDir.c_str());
	if (!dir)
	{
		std::cout << "Error opening \"" << TextLogDir << "\"!" << std::endl;
		return;
	}
	pcrecpp::RE testreg("Willkommen");
    
    std::list<string> fileNames;
    dirent *file;
	while ((file = readdir(dir)) != NULL)
	{
  		if (!pcrecpp::RE("^CL.*\\.txt$").FullMatch(file->d_name)) continue;
        string Filename = TextLogDir + "/" + file->d_name;
    	fileNames.push_back(Filename);
    }
    closedir(dir);
    
    fileNames.sort();
    for (std::list<string>::iterator itFilename = fileNames.begin(); itFilename != fileNames.end(); itFilename++) {
		FILE *pFile = fopen((*itFilename).c_str(), "r");
		char buffer[256];
		while (fgets(buffer, 255, pFile))
		{
			//if (testreg.PartialMatch(buffer)) std::cout << buffer << std::endl;
			rc->ProcessLine(buffer);
		} 
		fclose(pFile);
	}
	

}
