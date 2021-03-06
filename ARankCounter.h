#ifndef ARANKCOUNTER_H_
#define ARANKCOUNTER_H_

class ARankCounter : public ALogLineProcessor
{
protected:
	std::list<string> Trainers;
	std::map<string, string> TrainerClasses;
	std::set<string> combinationTrainers;
	std::list<string>::iterator iTrainer;
	std::map<string, int> Ranks;
	std::map<string, int> TrainedRanks;
	std::map<string, int> RanksToday;
	std::map<string, int> PetRanks;
	std::map<string, int> majorPetRanks;
	std::map<string, string> RankMessages;
	std::list<string>::iterator iTrainerMsg;
	std::map<string, int> TrainerMessages;
	std::map<string, string> Regexes;
	
	string Charname;

public:
	ARankCounter(string character);
	virtual ~ARankCounter();
	
	void ProcessLine(string Line);
	void processNextFile();
	void PrintRanks();
protected:
	bool CheckGameMessage(string msg);
	bool CheckTrainerMessage(string trainer, string msg);
	void CheckPetMessage(string pet, string msg);

private:
    void readYamlConfig();
};

#endif /*ARANKCOUNTER_H_*/
