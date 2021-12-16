#include "config_manager.h"
#include "thread_func.h"
#include "tcp_wrap.h"
#include "connection_info.h"
#include "newfd_queue.h"
#include "task_queue.h"
#include "handler.h"
#include "constants.h"


#include <unistd.h>
#include <sys/epoll.h>


#include <thread>
#include <memory>
#include <map>
#include <iostream>


// Read Configuration Before This Thread
void Leader()
{

	std::thread thrd_Acceptor, thrd_IOReceiver,thrd_Worker[10],
		thrd_HB_XingZhi, thrd_HB_HaiXin, thrd_HB_HaiKang, thrd_HB_XiMenZi, thrd_HB_Device5;
	
	thrd_Acceptor = std::thread(Acceptor);
	thrd_IOReceiver = std::thread(IOReceiver);
	
	for(int i=0; i<ConfigManager::getInstance().GetWorkerNumber();i++)
	{
		thrd_Worker[i]=std::thread(Worker,i);
	}
	
	const Machine& info = ConfigManager::getInstance().GetMachine();		
	if(info.XINGZHI.number>0)thrd_HB_XingZhi=std::thread(HBKeeper, MachineType::XINGZHI);
	if(info.HAIXIN.number>0)thrd_HB_HaiXin=std::thread(HBKeeper, MachineType::HAIXIN);
	if(info.HAIKANG.number>0)thrd_HB_HaiKang=std::thread(HBKeeper, MachineType::HAIKANG);
	if(info.XIMENZI.number>0)thrd_HB_XiMenZi=std::thread(HBKeeper, MachineType::XIMENZI);
	if(info.DEVICE5.number>0)thrd_HB_Device5=std::thread(HBKeeper, MachineType::DEVICE5);

	while(true)
	{
		pause();
	}
}

std::shared_ptr<ConnectionInfo> Fd2ConnectionInfo(int fd);

void Acceptor()
{
	Tcp_Wrap acceptor;
	acceptor.SetAddr(ConfigManager::getInstance().GetListenIP(),
			ConfigManager::getInstance().GetListenPort());
	acceptor.Listen();
	
	while(true)
	{
		int client_fd = acceptor.Accept();
		/* std::shared_ptr<ConnectionInfo> conn = std::make_shared<ConnectionInfo>(); */
		std::shared_ptr<ConnectionInfo> conn = Fd2ConnectionInfo(client_fd);
		NewConnQ::getInstance().Push(conn);
	}

}

void IOReceiver()
{
	std::map<std::shared_ptr<char>, std::shared_ptr<ConnectionInfo>> ipv4_conn_map;

	int EpFd = epoll_create(1024);
	struct epoll_event general_ev;
	general_ev.events=EPOLLET|EPOLLIN;

	struct epoll_event active_ev[1024];
	

	while(true)
	{
		std::shared_ptr<ConnectionInfo> conn = NewConnQ::getInstance().Pop();
		if(conn != nullptr)
		{
			ipv4_conn_map.insert(std::make_pair(conn->ipv4_addr, conn));
			
			epoll_ctl(EpFd, EPOLL_CTL_ADD, conn->fd, &general_ev);
		}

		int active_num = epoll_wait(EpFd, active_ev, 1024, -1);

		for(int i=0;i<active_num; i++)
		{
			/* ConnectionInfo* conn = static_cast<ConnectionInfo*> */
			int fd = active_ev[i].data.fd;
			std::shared_ptr<ConnectionInfo> conn = Fd2ConnectionInfo(fd);
			
			int events = active_ev[i].events;
			
			if(events & EPOLLIN)
			{
				// handle read
				HandleRead(conn);
			}else if(events & EPOLLERR)
			{
				// handle error
			}else
			{
				std::cout<<"Unknown Events"<<std::endl;
			}
		}

		
	}



}
#include <nlohmann/json.hpp>

using json=nlohmann::json;


nlohmann::basic_json<> GetOneTask(std::string str);


bool CheckIfValid(nlohmann::basic_json<>& rqs)
{
	// meta check
	if(rqs["meta"].is_null()||rqs["meta"]["action"].is_null()
			||rqs["meta"]["action"].is_null()||rqs["meta"]["ipv4"].is_null())
		return false;
	
	if(rqs["meta"].is_object()||rqs["meta"]["action"].is_string()
			||rqs["meta"]["action"].is_string()||rqs["meta"]["ipv4"].is_string())
		return false;

	
	// data check
	if(rqs["meta"]["action"].get<std::string>().compare("query"))
	{
		if(rqs["data"].is_null()||!rqs["data"].is_object())
			return false;
		
		auto data=rqs["data"];

		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::ActionTable))
		{
			if(data["action_id"].is_null()||!data["action_id"].is_number_integer()
					||data["plan_id"].is_null()||!data["plan_id"].is_number_integer())
				return false;
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::CharacterParameterVersion))
		{
			if(data["version"].is_null()||!data["version"].is_number_integer())
				return false;

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Detector))
		{
			if(data["content"].is_null()||!data["content"].is_array())
				return false;
			
			std::for_each(data["content"].begin(), data["content"].end(), [](nlohmann::basic_json<>& j)->bool{
						if(j["id"].is_null()||!j["id"].is_number_integer()) return false;
						if(j["option"].is_null()||!j["option"].is_number_integer()) return false;
						if(j["open_phase"].is_null()||!j["open_phase"].is_number_integer()) return false;
						if(j["close_phase"].is_null()||!j["close_phase"].is_number_integer()) return false;
						if(j["delay_time"].is_null()||!j["delay_time"].is_number_integer()) return false;
						if(j["delay"].is_null()||!j["delay"].is_number_integer()) return false;
						if(j["queue_limit"].is_null()||!j["queue_limit"].is_number_integer()) return false;
						if(j["no_reponse"].is_null()||!j["no_reponse"].is_number_integer()) return false;
						if(j["max_exist"].is_null()||!j["max_exist"].is_number_integer()) return false;
						if(j["error_count"].is_null()||!j["error_count"].is_number_integer()) return false;
						if(j["failure_time"].is_null()||!j["failure_time"].is_number_integer()) return false;
						if(j["warning"].is_null()||!j["warning"].is_number_integer()) return false;
						if(j["warning_log"].is_null()||!j["warning_log"].is_number_integer()) return false;
						if(j["reset"].is_null()||!j["reset"].is_number_integer()) return false;
					});


		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::FailureConfig))
		{
			if(data["config"].is_null()||!data["config"].is_array()) return false;
			
			std::for_each(data["config"].begin(),data["config"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["id"].is_null()||!j["id"].is_number_integer()) return false;
					if(j["mode"].is_null()||!j["mode"].is_number_integer()) return false;
					if(j["operation"].is_null()||!j["operation"].is_number_integer()) return false;
					if(j["operation_value"].is_null()||!j["operation_value"].is_number_integer()) return false;
					});


		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Failure))
		{
			if(data["failure"].is_null()||!data["failure"].is_array())
				return false;
			std::for_each(data["failure"].begin(), data["failure"].end(), [](nlohmann::basic_json<>& j)->bool{
						if(j["type"].is_null()||!j["type"].is_number_integer()) return false;
						if(j["timestamp"].is_null()||!j["timestamp"].is_number_integer()) return false;
						if(j["content"].is_null()||!j["content"].is_number_integer()) return false;
					});
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::FollowPhaseTable))
		{
			if(data["table"].is_null()||!data["table"].is_array()) return false;
			
			std::for_each(data["table"].begin(),data["table"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["id"].is_null()||!j["id"].is_number_integer()) return false;
					if(j["type"].is_null()||!j["type"].is_number_integer()) return false;
					if(j["father_phase"].is_null()||!j["father_phase"].is_number_integer()) return false;
					if(j["correct_phase"].is_null()||!j["correct_phase"].is_number_integer()) return false;
					if(j["green_time"].is_null()||!j["green_time"].is_number_integer()) return false;
					if(j["yellow_time"].is_null()||!j["yellow_time"].is_number_integer()) return false;
					if(j["red_time"].is_null()||!j["red_time"].is_number_integer()) return false;
					if(j["green_flash"].is_null()||!j["green_flash"].is_number_integer()) return false;
					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::IdentificationCode))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::LightStatus))
		{
			if(data["light_group"].is_null()||!data["light_group"].is_array()) return false;
			
			std::for_each(data["light_group"].begin(),data["light_group"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["status"].is_null()||!j["status"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PedestrianDetector))
		{
			if(data["content"].is_null()||!data["content"].is_array()) return false;
			
			std::for_each(data["content"].begin(),data["content"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["detector_id"].is_null()||!j["detector_id"].is_number_integer()) return false;
					if(j["trigger_phase"].is_null()||!j["trigger_phase"].is_number_integer()) return false;
					if(j["no_response"].is_null()||!j["no_response"].is_number_integer()) return false;
					if(j["max_trigger_time"].is_null()||!j["max_trigger_time"].is_number_integer()) return false;
					if(j["error_count"].is_null()||!j["error_count"].is_number_integer()) return false;
					if(j["warning"].is_null()||!j["warning"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Phase))
		{
			if(data["status"].is_null()||!data["status"].is_array()) return false;
			
			std::for_each(data["status"].begin(),data["status"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["phase_id"].is_null()||!j["phase_id"].is_number_integer()) return false;
					if(j["pedastrain_pass"].is_null()||!j["pedastrain_pass"].is_number_integer()) return false;
					if(j["pedastrain_clear"].is_null()||!j["pedastrain_clear"].is_number_integer()) return false;
					if(j["min_green"].is_null()||!j["min_green"].is_number_integer()) return false;
					if(j["lag_green"].is_null()||!j["lag_green"].is_number_integer()) return false;
					if(j["max_green_1"].is_null()||!j["max_green_1"].is_number_integer()) return false;
					if(j["max_green_2"].is_null()||!j["max_green_2"].is_number_integer()) return false;
					if(j["yellow_transition"].is_null()||!j["yellow_transition"].is_number_integer()) return false;
					if(j["red_clear"].is_null()||!j["red_clear"].is_number_integer()) return false;
					if(j["protection"].is_null()||!j["protection"].is_number_integer()) return false;
					if(j["inc_init"].is_null()||!j["inc_init"].is_number_integer()) return false;
					if(j["inc_max"].is_null()||!j["inc_max"].is_number_integer()) return false;
					if(j["dec_previous_time"].is_null()||!j["dec_previous_time"].is_number_integer()) return false;
					if(j["dec_previous_traffic"].is_null()||!j["dec_previous_traffic"].is_number_integer()) return false;
					if(j["dec_time"].is_null()||!j["dec_time"].is_number_integer()) return false;
					if(j["dec_rate"].is_null()||!j["dec_rate"].is_number_integer()) return false;
					if(j["min_interval"].is_null()||!j["min_interval"].is_number_integer()) return false;
					if(j["dynamic_max"].is_null()||!j["dynamic_max"].is_number_integer()) return false;
					if(j["dynamic_step"].is_null()||!j["dynamic_step"].is_number_integer()) return false;
					if(j["initial_para"].is_null()||!j["initial_para"].is_number_integer()) return false;
					if(j["loop_id"].is_null()||!j["loop_id"].is_number_integer()) return false;
					if(j["cocurrency_phase"].is_null()||!j["cocurrency_phase"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PhaseSequenceTable))
		{
			if(data["phasesequence"].is_null()||!data["phasesequence"].is_array()) return false;
			
			std::for_each(data["phasesequence"].begin(),data["phasesequence"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["table_number"].is_null()||!j["table_number"].is_number_integer()) return false;
					if(j["loop_number"].is_null()||!j["loop_number"].is_number_integer()) return false;

					});


		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PriorityConfig))
		{
			if(data["config"].is_null()||!data["config"].is_array()) return false;
			
			std::for_each(data["config"].begin(),data["config"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["control_interface"].is_null()||!j["control_interface"].is_number_integer()) return false;
					if(j["link"].is_null()||!j["link"].is_number_integer()) return false;
					if(j["delay"].is_null()||!j["delay"].is_number_integer()) return false;
					if(j["min_existence"].is_null()||!j["min_existence"].is_number_integer()) return false;
					if(j["min_green"].is_null()||!j["min_green"].is_number_integer()) return false;
					if(j["motor_linger"].is_null()||!j["motor_linger"].is_number_integer()) return false;
					if(j["pedastrain_linger"].is_null()||!j["pedastrain_linger"].is_number_integer()) return false;
					if(j["pass_time"].is_null()||!j["pass_time"].is_number_integer()) return false;
					if(j["min_interval"].is_null()||!j["min_interval"].is_number_integer()) return false;
					if(j["max_continue"].is_null()||!j["max_continue"].is_number_integer()) return false;
					if(j["track_phase"].is_null()||!j["track_phase"].is_number_integer()) return false;
					if(j["motor_linger_phase"].is_null()||!j["motor_linger_phase"].is_number_integer()) return false;
					if(j["pedastrain_linger_phase"].is_null()||!j["pedastrain_linger_phase"].is_number_integer()) return false;
					if(j["exit_phase"].is_null()||!j["exit_phase"].is_number_integer()) return false;
					if(j["priority_status"].is_null()||!j["priority_status"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::RemoteControl))
		{
			if(data["control_type"].is_null()||!data["control_type"].is_number_integer()) return false;
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Schedule))
		{
			if(data["plan"].is_null()||!data["plan"].is_array()) return false;
			
			std::for_each(data["plan"].begin(),data["plan"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["plan_id"].is_null()||!j["plan_id"].is_number_integer()) return false;
					if(j["month_data"].is_null()||!j["month_data"].is_number_integer()) return false;
					if(j["week_data"].is_null()||!j["week_data"].is_number_integer()) return false;
					if(j["day_data"].is_null()||!j["day_data"].is_number_integer()) return false;
					if(j["time_schedule_number"].is_null()||!j["time_schedule_number"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::SchemaTable))
		{
			if(data["schema"].is_null()||!data["schema"].is_array()) return false;
			
			std::for_each(data["schema"].begin(),data["schema"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["cycle_length"].is_null()||!j["cycle_length"].is_number_integer()) return false;
					if(j["phase_difference"].is_null()||!j["phase_difference"].is_number_integer()) return false;
					if(j["green_sig_rate_table_id"].is_null()||!j["green_sig_rate_table_id"].is_number_integer()) return false;
					if(j["phase_sequence_id"].is_null()||!j["phase_sequence_id"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::SemaphoreGroup))
		{
			if(data["group"].is_null()||!data["group"].is_array()) return false;
			
			std::for_each(data["group"].begin(),data["group"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["group_number"].is_null()||!j["group_number"].is_number_integer()) return false;
					if(j["control_type"].is_null()||!j["control_type"].is_number_integer()) return false;
					if(j["control_number"].is_null()||!j["control_number"].is_number_integer()) return false;
					if(j["flash"].is_null()||!j["flash"].is_number_integer()) return false;
					if(j["brightness"].is_null()||!j["brightness"].is_number_integer()) return false;
					if(j["location_x"].is_null()||!j["location_x"].is_number_integer()) return false;
					if(j["location_y"].is_null()||!j["location_y"].is_number_integer()) return false;
					if(j["timer_number"].is_null()||!j["timer_number"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Time))
		{
			if(data["timestamp"].is_null()||!data["timestamp"].is_number_integer()) return false;

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TimeTable))
		{
			if(data["table"].is_null()||!data["table"].is_array()) return false;
			
			std::for_each(data["table"].begin(),data["table"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["time_table_id"].is_null()||!j["time_table_id"].is_number_integer()) return false;
					if(j["time_segment_id"].is_null()||!j["time_segment_id"].is_number_integer()) return false;
					if(j["time_segment_begin_hour"].is_null()||!j["time_segment_begin_hour"].is_number_integer()) return false;
					if(j["time_segment_begin_minu"].is_null()||!j["time_segment_begin_minu"].is_number_integer()) return false;
					if(j["action_id"].is_null()||!j["action_id"].is_number_integer()) return false;
					if(j["failure_plan_id"].is_null()||!j["failure_plan_id"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TimingScheme))
		{
			if(data["scheme"].is_null()||!data["scheme"].is_array()) return false;
			
			std::for_each(data["scheme"].begin(),data["scheme"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["time_id"].is_null()||!j["time_id"].is_number_integer()) return false;
					if(j["phase"].is_null()||!j["phase"].is_number_integer()) return false;
					if(j["time_len"].is_null()||!j["time_len"].is_number_integer()) return false;
					if(j["mode"].is_null()||!j["mode"].is_number_integer()) return false;
					if(j["mode"].is_null()||!j["mode"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TrafficInfo))
		{
			if(data["detector"].is_null()||!data["detector"].is_array()) return false;
			
			std::for_each(data["detector"].begin(),data["detector"].end(), [](nlohmann::basic_json<>& j)->bool{
					if(j["detector_id"].is_null()||!j["detector_id"].is_number_integer()) return false;
					if(j["detect_interval"].is_null()||!j["detect_interval"].is_number_integer()) return false;
					if(j["flow_data"].is_null()||!j["flow_data"].is_number_integer()) return false;
					if(j["occupation"].is_null()||!j["occupation"].is_number_integer()) return false;

					});

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::UnitParameter))
		{
			if(data["boot_yellow"].is_null()||!data["boot_yellow"].is_number_integer()) return false;
			if(data["boot_red"].is_null()||!data["boot_red"].is_number_integer()) return false;
			if(data["gps_sync_time"].is_null()||!data["gps_sync_time"].is_number_integer()) return false;
			if(data["dec_time"].is_null()||!data["dec_time"].is_number_integer()) return false;
			if(data["speed_factor"].is_null()||!data["speed_factor"].is_number_integer()) return false;
			if(data["communication_overtime"].is_null()||!data["communication_overtime"].is_number_integer()) return false;
			if(data["flash_frequence"].is_null()||!data["flash_frequence"].is_number_integer()) return false;
			if(data["cross_street_time_lag"].is_null()||!data["cross_street_time_lag"].is_number_integer()) return false;
			if(data["cross_reverse_street_time_lag"].is_null()||!data["cross_reverse_street_time_lag"].is_number_integer()) return false;
			if(data["transition_cycle"].is_null()||!data["transition_cycle"].is_number_integer()) return false;
			if(data["data_flow_collect_interval"].is_null()||!data["data_flow_collect_interval"].is_number_integer()) return false;
			if(data["data_flow_interval_unit"].is_null()||!data["data_flow_interval_unit"].is_number_integer()) return false;
			if(data["min_red"].is_null()||!data["min_red"].is_number_integer()) return false;
			if(data["pedastrain_auto_clear"].is_null()||!data["pedastrain_auto_clear"].is_number_integer()) return false;
		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Version))
		{
			if(data["version"].is_null()||!data["version"].is_number_integer()) return false;

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::WorkMode))
		{
			if(data["constant_cycle"].is_null()||!data["constant_cycle"].is_number_integer()) return false;
			if(data["reaction"].is_null()||!data["reaction"].is_number_integer()) return false;
			if(data["mannual"].is_null()||!data["mannual"].is_number_integer()) return false;
			if(data["off"].is_null()||!data["off"].is_number_integer()) return false;
			if(data["yellow_flash"].is_null()||!data["yellow_flash"].is_number_integer()) return false;
			if(data["phase_lock"].is_null()||!data["phase_lock"].is_number_integer()) return false;
			if(data["certain_phase"].is_null()||!data["certain_phase"].is_number_integer()) return false;

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::WorkStatus))
		{
			if(data["is_auto"].is_null()||!data["is_auto"].is_number_integer()) return false;
			if(data["workmode"].is_null()||!data["workmode"].is_number_integer()) return false;
			if(data["plan_number"].is_null()||!data["plan_number"].is_number_integer()) return false;
			if(data["scheme"].is_null()||!data["scheme"].is_number_integer()) return false;
			if(data["green_signal_number"].is_null()||!data["green_signal_number"].is_number_integer()) return false;
			
		}
		

	}
	/* else if(rqs["meta"]["action"].get<std::string>().compare("setting")){ */
	/* } */
	else return false;
	if(rqs["data"].is_null())
		return false;

}

bool HandleRqs(nlohmann::basic_json<>& rqs)
{
	if(rqs["meta"]["action"].is_null())
		return false;

		if(rqs["data"].is_null()||!rqs["data"].is_object())
			return false;
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::ActionTable))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::CharacterParameterVersion))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Detector))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::FailureConfig))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Failure))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::FollowPhaseTable))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::IdentificationCode))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::LightStatus))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PedestrianDetector))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Phase))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PhaseSequenceTable))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::PriorityConfig))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::RemoteControl))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Schedule))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::SchemaTable))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::SemaphoreGroup))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Time))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TimeTable))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TimingScheme))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::TrafficInfo))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::UnitParameter))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::Version))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::WorkMode))
		{

		}else
		if(rqs["meta"]["type"].get<std::string>().compare(IFTYPE::WorkStatus))
		{

		}
}


void Worker(int work_id)
{
	std::shared_ptr<TaskInfo> task = TaskQ::getInstance().Pop();
	
	auto request_json = GetOneTask(task->buffer);
	
	HandleRqs(request_json);
	// do the work
}

void HBKeeper(int MachineType)
{

}
