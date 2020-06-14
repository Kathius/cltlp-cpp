#include "stdio.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <list>
#include <map>

#include "pcrecpp.h"
#include "ALogLineProcessor.h"
#include "ARankCounter.h"

#define REGEX_OPTIONAL_TIMESTAMP "(?:\\d\\d?/\\d\\d?/\\d\\d \\d\\d?:\\d\\d:\\d\\d(?:a|p) )?"

#define max(a, b) (a > b) ? a : b

ARankCounter::ARankCounter(string character)
{
	Charname = character;
	
	bool bTrainer = true;
	FILE *pFile=NULL;
	pFile = fopen("trainers.cfg", "r");
	char buffer[256];
	try
	{
		while (fgets(buffer, 255, pFile))
		{
			if (!strcmp("\r\n", buffer))
			{
				bTrainer=false;
				continue;
			}
			if (bTrainer)
			{
				string Trainer, Message;
				if (!pcrecpp::RE("([a-zA-Z' ]+)\\t(.*)\\r\\n", pcrecpp::UTF8()).FullMatch(buffer, &Trainer, &Message))
				{
					throw "Fehler in trainers.cfg";
				}
				RankMessages[Message] = Trainer;
				Trainers.push_back(Trainer);
			}else
			{
				string Message;
				int Ranks;
				pcrecpp::RE("(\\d+)\\t(.*)\\r\\n", pcrecpp::UTF8()).FullMatch(buffer, &Ranks, &Message);
				TrainerMessages[Message] = Ranks;
			}
		}
	}catch (string str)
	{
		fclose(pFile);
		throw str;
	}
	fclose(pFile);

	// Trainer-Regex
	Regexes["Trainers"] = "";
	for (iTrainer=Trainers.begin();iTrainer!=Trainers.end();iTrainer++)
	{
		if (iTrainer != Trainers.begin()) Regexes["Trainers"] += "|";
		Regexes["Trainers"] += *iTrainer;
	}
}

ARankCounter::~ARankCounter()
{
}

void ARankCounter::processNextFile()
{
    RanksToday.clear();
	return;
}

void ARankCounter::ProcessLine(string Line)
{
	static pcrecpp::RE re(REGEX_OPTIONAL_TIMESTAMP"•((?:Du|Deine?|Your?) .*)(\\r\\n)?$", pcrecpp::UTF8());
	static pcrecpp::RE re2(REGEX_OPTIONAL_TIMESTAMP"("+Regexes["Trainers"]+") (?:sagt|says), \"(.*)\"(\\r\\n)?$", pcrecpp::UTF8());
	static pcrecpp::RE petMessage(REGEX_OPTIONAL_TIMESTAMP"\\* (.*) grows stronger.(\\r\\n)?$", pcrecpp::UTF8());
	string msg;
	string trainer;
	string pet;
	if (re.FullMatch(Line, &msg))
	{
		CheckGameMessage(msg);
	}else if (re2.FullMatch(Line, &trainer, &msg))
	{
		CheckTrainerMessage(trainer, msg);
	}else if (petMessage.FullMatch(Line, &pet, &msg))
    {
        CheckPetMessage(pet, msg);
    }
}

bool ARankCounter::CheckGameMessage(string msg)
{
	// Schauen, ob die Nachricht in den Rankmeldungen vorkommt...
	if (RankMessages.find(msg) != RankMessages.end())
	{
		Ranks[RankMessages[msg]]++;
		TrainedRanks[RankMessages[msg]]++;
		RanksToday[RankMessages[msg]]++;
		return true;
	}
	return false;
}
bool ARankCounter::CheckTrainerMessage(string trainer, string msg)
{
	static pcrecpp::RE re("(?:Hail|Sei gegrüßt),[a-zA-Z ]+"+Charname+"\\. (.*)", pcrecpp::UTF8());
	
	// Schauen, ob die Nachricht in den Trainermeldungen vorkommt...
	string msg2;
	if (!re.FullMatch(msg, &msg2)) return false;
	if (TrainerMessages.find(msg2) != TrainerMessages.end())
	{
		Ranks[trainer] = max(Ranks[trainer], TrainerMessages[msg2]);
		return true; 		
	}
	return false;
}

void ARankCounter::CheckPetMessage(string pet, string msg)
{
    PetRanks[pet]++;
}

void ARankCounter::PrintRanks()
{
    unsigned int totalRanks = 0;
    unsigned int totalTrainedRanks = 0;

	for (iTrainer=Trainers.begin();iTrainer!=Trainers.end();iTrainer++)
	{
	    totalRanks += Ranks[*iTrainer];
	    totalTrainedRanks += TrainedRanks[*iTrainer];
		if (Ranks[*iTrainer] == 0 && TrainedRanks[*iTrainer] == 0 ) continue;
		std::cout
		    << std::setw(20) << *iTrainer << ": "
		    << std::setw(4) << Ranks[*iTrainer]
		    << " (" << TrainedRanks[*iTrainer] << ")";

        if (RanksToday[*iTrainer] > 0) {
            std::cout << " +" << RanksToday[*iTrainer];
        }

		std::cout << std::endl;
	}

    std::cout << std::endl
        << "Total: "
        << totalRanks << std::endl
        << "Total trained: " << totalTrainedRanks << ""
        << std::endl;

    if (PetRanks.begin() != PetRanks.end()) {
        std::cout << std::endl;
    }

	for (std::map<string, int>::iterator iPet = PetRanks.begin(); iPet != PetRanks.end(); iPet++)
	{
		std::cout
		    << iPet->first << ": "
		    << std::setw(4) << iPet->second
		    << std::endl;
	}

}
