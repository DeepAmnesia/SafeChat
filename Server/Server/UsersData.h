#pragma once
#include <Windows.h>
#include <string>
#include <deque>

struct UserData
{
	std::string HWID;
	std::string Name;
};

struct Chat
{
	std::string First;
	std::string Second;
	std::deque<std::string> messages;

	void AddMessage(std::string message)
	{
		messages.push_back(message);
	};
};
std::deque<Chat> chats;
std::deque<UserData> users_data;
std::deque<UserData> admin_list;

void AddUser(std::string HWID, std::string Name)
{
	users_data.push_back(UserData{ HWID, Name });
}

bool FindUserByHWID(std::string HWID)
{
	for (int i = 0; i < users_data.size(); i++)
		if (users_data.at(i).HWID == HWID)
			return true;
	return false;
}

bool FindUserByName(std::string Name)
{
	for (int i = 0; i < users_data.size(); i++)
		if (users_data.at(i).Name == Name)
			return true;
	return false;
}