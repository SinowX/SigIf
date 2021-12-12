#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include <cstring>

typedef struct
{
	int XINGZHI;
	int HAIXIN;
	int HAIKANG;
	int XIMENZI;
	int DEVICE5;
}Machine;

typedef struct MachineInfo
{
	struct MachineInfo *next;
}MachineInfo;


class ConfigManager
{
	public:
		// for Singleton
		static ConfigManager& GetInstance()
		{
			static ConfigManager instance_;
			return instance_;
		}
		~ConfigManager()=default;
		ConfigManager(const ConfigManager&)=delete;
		ConfigManager& operator=(const ConfigManager&)=delete;
		
		// ReadConfig
		int ReadConfig(const char* path);
		const char* GetListenIP()const;
		const int GetListenPort()const;
		const MachineNum& GetSigMachineNum()const;
		const int GetSigMachineTypeNum()const;
		const MachineInfo* GetMachineInfoList()const;
	private:
		// Store the ReadConfig Here
		
		// for Singleton
		ConfigManager()
			:AlreadyRead(false)
		{
			memset(&machine_num_,0,sizeof(machine_num_));
		};
		bool AlreadyRead;
		MachineNum machine_num_;
};


#endif
