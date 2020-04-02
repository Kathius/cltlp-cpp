#ifndef ALOGLINEPROCESSOR_H_
#define ALOGLINEPROCESSOR_H_

class ALogLineProcessor
{
public:
	ALogLineProcessor();
	virtual ~ALogLineProcessor();
	
	virtual void ProcessLine(string Line);
};

#endif /*ALOGLINEPROCESSOR_H_*/
