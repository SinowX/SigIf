#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include <cstring>

enum MachineType
{
	XINGZHI,
	HAIXIN,
	HAIKANG,
	XIMENZI,
	DEVICE5
};

typedef struct MachineInfo
{
	char ipv4[16];
	int id;
	struct MachineInfo *next;
}MachineInfo;

typedef struct EachMachineType
{
	int number;
	MachineInfo* head;
}EachMachineType;


typedef struct
{
	EachMachineType XINGZHI;
	EachMachineType HAIXIN;
	EachMachineType HAIKANG;
	EachMachineType XIMENZI;
	EachMachineType DEVICE5;
}Machine;


class ConfigManager
{
	public:
		// for Singleton
		static ConfigManager& getInstance()
		{
			static ConfigManager instance_;
			return instance_;
		}
		~ConfigManager()=default;
		ConfigManager(const ConfigManager&)=delete;
		void operator=(const ConfigManager&)=delete;
		
		// ReadConfig
		int ReadConfig(const char* path);
		const char* GetListenIP()const
		{
			return listenAddr_;
		}
		const int GetListenPort()const
		{
			return port_;
		}
		// get detailed mahine infomation
		const Machine& GetMachine()const
		{
			return machine_config_;
		}

		const int GetWorkerNumber()const
		{
			if(!AlreadyRead_) return -1;
			else return WorkerNumber_;
		}
		const int GetIOReceiveNumber()const
		{
			if(!AlreadyRead_) return -1;
			else return IOReceiveNumber_;
		}
		/* const Machine& GetSigMachineNum()const; */
		/* const int GetSigMachineTypeNum()const; */
		/* const MachineInfo* GetMachineInfoList()const; */
	private:
		// Store the ReadConfig Here
		
		// for Singleton
		ConfigManager()
			:AlreadyRead_(false),WorkerNumber_(0),IOReceiveNumber_(0),
			port_(0)
		{
			memset(&machine_config_,0,sizeof(machine_config_));
			memset(listenAddr_,0,sizeof(listenAddr_));
		};
		bool AlreadyRead_;
		int WorkerNumber_;
		int IOReceiveNumber_;
		Machine machine_config_;
		char listenAddr_[16];
		int port_;
		/* Machine machine_num_; */
};


#endif
