#include "config_manager.h"
/* #include "INIReader.h" */
#include <nlohmann/json.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

using json=nlohmann::json;

int ConfigManager::ReadConfig(const char *path)
{
	int res=0;
	int config_fd = open(path,O_RDONLY);
	ssize_t file_size = lseek(config_fd, 0, SEEK_END);	
	res = lseek(config_fd,0, SEEK_SET);

	char* buffer = new char[file_size];

	ssize_t n = read(config_fd,buffer,file_size);

	if(n!=file_size) std::cout<<"Error here"<<std::endl;

	auto config_json = json::parse(buffer);

	auto config_server_json = config_json["Server"];
	strcmp(listenAddr_,config_server_json["Host"].get<std::string>().c_str());
	port_ = config_server_json["Port"].get<int>();
	WorkerNumber_ = config_server_json["WorkerThreadNumber"].get<int>();
	IOReceiveNumber_ = config_server_json["IOReaderThreadNumber"].get<int>();

	auto config_machine_json = config_json["Machine"];

	// assum config_machine_json.empty() is false
	
	/* for(json::iterator it = config_machine_json.begin(), */
	/* 		it!=config_machine_json.end(), it++) */
	/* {} */

	for(auto& element : config_machine_json)
	{
		EachMachineType* m_type=nullptr;
		MachineInfo* m_info=nullptr;
		if(strcmp(element["MachineType"].get<std::string>().c_str(),"XINGZHI"))
		{
			m_type=&machine_config_.XINGZHI;
		}else if(strcmp(element["MachineType"].get<std::string>().c_str(),"HAIXIN"))
		{
			m_type=&machine_config_.HAIXIN;
		}else if(strcmp(element["MachineType"].get<std::string>().c_str(),"HAIKANG"))
		{
			m_type=&machine_config_.HAIKANG;
		}else if(strcmp(element["MachineType"].get<std::string>().c_str(),"XIMENZI"))
		{
			m_type=&machine_config_.XIMENZI;
		}else if(strcmp(element["MachineType"].get<std::string>().c_str(),"DEVICE5"))
		{
			m_type=&machine_config_.DEVICE5;
		}

		m_info=m_type->head;
		for(int i=0; i<m_type->number;i++) m_info=m_info->next;
		m_info->id=element["Id"].get<int>();
		strcpy(m_info->ipv4,element["IpAddr"].get<std::string>().c_str());
		m_type->number++;
	}

	AlreadyRead_=true;
	return AlreadyRead_;
}
