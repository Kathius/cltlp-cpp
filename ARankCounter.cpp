#include "stdio.h"

#include <iostream>
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

void ARankCounter::ProcessLine(string Line)
{
	static pcrecpp::RE re(REGEX_OPTIONAL_TIMESTAMP"•((?:Du|Deine?|Your?) .*)(\\r\\n)?$", pcrecpp::UTF8());
	static pcrecpp::RE re2(REGEX_OPTIONAL_TIMESTAMP"("+Regexes["Trainers"]+") (?:sagt|says), \"(.*)\"(\\r\\n)?$", pcrecpp::UTF8());
	string msg;
	string trainer;
	if (re.FullMatch(Line, &msg))
	{
		CheckGameMessage(msg);
	}else if (re2.FullMatch(Line, &trainer, &msg))
	{
		CheckTrainerMessage(trainer, msg);
	}
}

bool ARankCounter::CheckGameMessage(string msg)
{
	// Schauen, ob die Nachricht in den Rankmeldungen vorkommt...
	if (RankMessages.find(msg) != RankMessages.end())
	{
		Ranks[RankMessages[msg]]++;
		return true;
	}
	return false;
}
bool ARankCounter::CheckTrainerMessage(string trainer, string msg)
{
	static pcrecpp::RE re("Hail,[a-zA-Z ]+"+Charname+"\\. (.*)", pcrecpp::UTF8());
	
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

void ARankCounter::PrintRanks()
{
	for (iTrainer=Trainers.begin();iTrainer!=Trainers.end();iTrainer++)
	{
		if (Ranks[*iTrainer]==0) continue;
		std::cout << *iTrainer << ": " << Ranks[*iTrainer] << std::endl;
	}
}
