#ifndef CONSTANTS_H
#define CONSTANTS_H

const int MAX_LISTEN_QUEUE = 1024;

enum RETURN{
	ERROR=-1,
	SUCCESS=0
};

namespace  IFTYPE{
	const char* ActionTable="ActionTable";
	const char* CharacterParameterVersion="CharacterParameterVersion";
	const char* Detector="Detector";
	const char* FailureConfig="FailureConfig";
	const char* Failure="Failure";
	const char* FollowPhaseTable="FollowPhaseTable";
	const char* IdentificationCode="IdentificationCode";
	const char* LightStatus="LightStatus";
	const char* PedestrianDetector="PedestrianDetector";
	const char* Phase="Phase";
	const char* PhaseSequenceTable="PhaseSequenceTable";
	const char* PriorityConfig="PriorityConfig";
	const char* RemoteControl="RemoteControl";
	const char* Schedule="Schedule";
	const char* SchemaTable="SchemaTable";
	const char* SemaphoreGroup="SemaphoreGroup";
	const char* Time="Time";
	const char* TimeTable="TimeTable";
	const char* TimingScheme="TimingScheme";
	const char* TrafficInfo="TrafficInfo";
	const char* UnitParameter="UnitParameter";
	const char* Version="Version";
	const char* WorkMode="WorkMode";
	const char* WorkStatus="WorkStatus";
};



/* typedef */ 
/* enum SIGMACHINETYPE{ */
/* 	XINGZHI=0; */
/* 	HAIKANG= */
/* }; */


#endif
