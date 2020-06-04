#ifndef ARANKCOUNTER_H_
#define ARANKCOUNTER_H_

class ARankCounter : public ALogLineProcessor
{
protected:
	std::list<string> Trainers;
	std::list<string>::iterator iTrainer;
	std::map<string, int> Ranks;
	std::map<string, int> TrainedRanks;
	std::map<string, string> RankMessages;
	std::list<string>::iterator iTrainerMsg;
	std::map<string, int> TrainerMessages;
	std::map<string, string> Regexes;
	
	string Charname;

public:
	ARankCounter(string character);
	virtual ~ARankCounter();
	
	void ProcessLine(string Line);
	void PrintRanks();
protected:
	bool CheckGameMessage(string msg);
	bool CheckTrainerMessage(string trainer, string msg);
};

#endif /*ARANKCOUNTER_H_*/
