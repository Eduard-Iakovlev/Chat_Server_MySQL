#pragma once
#include <iostream>
#include <string>


class Message {
public:
	Message() = default;
	Message(std::string, std::string, std::string, std::string);

	void create_message(std::string, std::string, std::string, std::string);
	void show_message();

	std::string login_sender();
	std::string login_recipient();
	std::string name_sender();
	std::string return_mess();

private:
	std::string _mass;
	std::string _name_sender;
	std::string _login_sender;
	std::string _login_recipient;
};
