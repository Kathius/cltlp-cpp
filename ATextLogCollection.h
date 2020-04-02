#ifndef ATEXTLOGCOLLECTION_H_
#define ATEXTLOGCOLLECTION_H_

class ATextLogCollection
{
protected:
	string ArindalRootDir;
	string TextLogDir;
	string Character;
	ALogLineProcessor *rc; // RankCounter;
public:
	ATextLogCollection(string Directory, string Character);
	virtual ~ATextLogCollection();
	void SetRankCounter(ALogLineProcessor *p);
	void ProcessFiles();
};

#endif /*ATEXTLOGCOLLECTION_H_*/
