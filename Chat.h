﻿#pragma once

#include <iostream>
#include<string>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include <mysql/mysql.h>

#include <vector>
#include <map>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "User.h"
#include "Message.h"
#include "Hash.h"

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 7777 // Будем использовать этот номер порта

class Chat
{
public:
	Chat();
	Chat(std::string, std::string, std::string);

	//------------------------------ Работа СУБД -----------------------------------
    std::string reg_data_mysql();// ввод пользователя и пароля mysql
    void test_msql_descriptor(MYSQL& ms); // проверка на получение дискриптора
	bool connect_to_db(MYSQL& ms); // подключение к базе данных
	void create_database(MYSQL& ms); // создание базы дынных
	void create_table(MYSQL& ms); // создание таблиц
	void insert_into_users(MYSQL& ms, std::string table, std::string login, std::string name, std::string hash); // Вставкаданныых в таблицу пользователя
	void insert_into_messsage(MYSQL& ms, std::string table, std::string send, std::string name, std::string rec, std::string mess); // Вставка в таблицу сообщений
	void show_table(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row, std::string table); // Вывод таблицы
	void transmit_table_users(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row, std::string table); // передача данных таблицы users
	void transmit_table_messages(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row, std::string table); // передача данных таблицы messages
	int number_of_users(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW row, std::string table); // Проверка количества пользователей
	bool check_login_table(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row, std::string table, std::string login); // Проверка логина
	bool check_pass_table(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row, std::string table, std::string login, std::string pass); // Проверка пароля
	std::string name_user (MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row, std::string table, std::string login); // получение имени пользователя

	//------------------------------------------------------------------------------

	// ------------------------------ Работа сети ----------------------------------
	void socket_file(); // Создание сокета
	void close_socket(); // Закрытие сокета
	void server_address(); // Настройка порта и привязка сокета
	void connect(); // Постановка сервера на приём
	void exchange(const std::string& mess); // Обмен данными
	//std::string get_parsing(const std::string& mess, std::string word, int count); // Парсинг
	void transmitting(const std::string& mess); //отправка данных
	void receiving_user(); // Прием данных
	char message0();// Возврат первого символа сообщения
	std::string message();// Перевод сообщения в стринг
	void accepted(std::string mess);
	void transmitted(std::string mess);
	void log(std::string mess);
	//------------------------------------------------------------------------------

	void greeting();
	void farewell();

	void registration(char, bool*, MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row);

	std::string active_user_login();
	std::string active_user_name();
	std::string active_recipient_login();

	void get_user(std::string, std::string);
	void out_user();
	//void get_recipient(char menu, MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row);

	void one_user();

	void send_message(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row);

	void clear_show_user();
	void clean_console();
	void system_pause(int second);

	void chat_work();
	void account_work(MYSQL& ms, MYSQL_RES* res, MYSQL_ROW& row);


private:
	// -------------- переменные сети -----------
	int _socket_file_descriptor, _connection, _bind_status, _connection_status;
	struct sockaddr_in _server_address, _client;
	socklen_t _length;
	char _message[MESSAGE_LENGTH];
	// --------------- переменные СУБД ------------
	int not_db = 1049;
	std::string username_db;
	std::string password_db;
	std::string database_chat = "chat";
	std::string table_users = "users";
	std::string table_mess = "messages";

	// ---------------- переменные чата ---------
	std::map <std::string, User> _users;
	std::vector<Message> _messages;
	std::string _active_user_login;
	std::string _active_recipient_login;
	std::string _active_user_name;

	char _menu;
	
	bool _work{ true };
	bool _check_user{ false };
	bool _discussion{ true };

};
