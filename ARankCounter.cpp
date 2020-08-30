#include "stdio.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <list>
#include <map>

#include "yaml-cpp/yaml.h"
#include "pcrecpp.h"
#include "ALogLineProcessor.h"
#include "ARankCounter.h"

#define REGEX_OPTIONAL_TIMESTAMP "(?:\\d\\d?/\\d\\d?/\\d\\d \\d\\d?:\\d\\d:\\d\\d(?:a|p) )?"

#define max(a, b) (a > b) ? a : b

ARankCounter::ARankCounter(string character)
{
	Charname = character;

	readYamlConfig();

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

void ARankCounter::readYamlConfig()
{
    YAML::Node trainers = YAML::LoadFile("trainers.yaml");
    for (size_t iTrainer = 0; iTrainer < trainers["trainers"].size(); iTrainer++) {
        string message = trainers["trainers"][iTrainer]["message"].as<string>();
        string trainer = trainers["trainers"][iTrainer]["name"].as<string>();
        RankMessages[message] = trainer;
        Trainers.push_back(trainer);
        if (trainers["trainers"][iTrainer]["class"]) {
            string trainerClass = trainers["trainers"][iTrainer]["class"].as<string>();
            TrainerClasses[trainer] = trainerClass;
        }
    }

    for (size_t iMessage = 0; iMessage < trainers["progressmessages"].size(); iMessage++) {
        string message = trainers["progressmessages"][iMessage]["message"].as<string>();
        int ranks = trainers["progressmessages"][iMessage]["ranks"].as<int>();
        TrainerMessages[message] = ranks;
    }
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
	    string trainer = RankMessages[msg];
		Ranks[trainer]++;
		TrainedRanks[trainer]++;
		RanksToday[trainer]++;
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

	std::map<string, int> ClassRanks;
	std::map<string, int> TrainedClassRanks;
	std::map<string, int> ClassRanksToday;

	for (string trainer : Trainers)
	{
	    totalRanks += Ranks[trainer];
	    totalTrainedRanks += TrainedRanks[trainer];
		if (Ranks[trainer] == 0 && TrainedRanks[trainer] == 0 ) continue;
		std::cout
		    << std::setw(20) << trainer << ": "
		    << std::setw(4) << Ranks[trainer]
		    << " (" << TrainedRanks[trainer] << ")";

        if (RanksToday[trainer] > 0) {
            std::cout << " +" << RanksToday[trainer];
        }

		std::cout << std::endl;

        if (TrainerClasses.find(trainer) != TrainerClasses.end()) {
            string trainerClass = TrainerClasses[trainer];
            ClassRanks[trainerClass] += Ranks[trainer];
            TrainedClassRanks[trainerClass] += TrainedRanks[trainer];
            ClassRanksToday[trainerClass] += RanksToday[trainer];
        }
	}

    std::cout << std::endl;

    for (std::pair<string, int> classRank : ClassRanks) {
        std::cout
            << classRank.first << ":"
            << std::setw(4) << classRank.second
            << " (" << TrainedClassRanks[classRank.first] << ")";

        if (ClassRanksToday[classRank.first] > 0) {
            std::cout << " +" << ClassRanksToday[classRank.first];
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

	for (std::pair<string, int> petRank : PetRanks)
	{
		std::cout
		    << petRank.first << ": "
		    << std::setw(4) << petRank.second
		    << std::endl;
	}

}
