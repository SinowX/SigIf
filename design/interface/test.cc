#include <nlohmann/json.hpp>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>

using json=nlohmann::json;
using std::cout;
using std::endl;

int main()
{
	int fd = open("TrafficInfo.json",O_RDONLY);
	if(fd<0)
	{
		return -1;
	}

	char* buffer = new char[500];

	int n = read(fd,buffer,500);
	if(n<0)
	{
		std::cout<<"read failed"<<std::endl;
		return -1;
	}
	buffer[n]='\0';

	std::cout<<"read success\n";

	cout<<buffer<<endl;

	auto j3 = json::parse(buffer);

	cout<<"parse success"<<endl;

	cout<<j3.contains("meta")<<endl;

	cout<<j3["meta"]<<endl;

	cout<<j3["meta"]["ipv4"]<<endl;

	std::string str;

	j3["meta"]["ipv4"].get_to(str);

 auto abc = j3["meta"]["ipv4"];
 cout<<abc.is_string()<<endl;
 auto detector = j3["data"]["detector"];
  cout<<detector.size()<<endl;


	cout<<detector[1]<<endl;
	
	auto asd = detector[5];

	cout<<asd<<endl;
	cout<<asd.is_null()<<endl;
	cout<<detector[1]["detector_id"].get<std::string>()<<endl;
	/* std::string str; */
	/* j3["meta"].get_to(str); */

	/* std::cout<<str<<std::endl; */
}
