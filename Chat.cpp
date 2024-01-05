#include "Chat.h"

Chat::Chat(){
	greeting();
}

Chat::Chat(std::string active_user_login, std::string active_recipient_login, std::string active_user_name) :
	_active_user_login(active_user_login), _active_recipient_login(active_recipient_login), _active_user_name(active_user_name) {}

//----------------------------------------------------- Работа СУБД ------------------------------------------------
//---------------- ввод пользователя и пароля mysql -------------------
std::string Chat::reg_data_mysql(){
	std::string str;
	std::cin >> str;
	return str;
}

//------------------ Проверка на получение дискриптора ------------------
void Chat::test_msql_descriptor(MYSQL& ms) {

	if (&ms == NULL) {
		// Если дескриптор не получен — выводим сообщение об ошибке
		std::cout << " Error: can't create MySQL-descriptor" << std::endl;
		exit(1);
	};
	std::cout << " MySQL-descriptor created" << std::endl;
}

//-------------------- Подключаемся к базе данных ----------------------
bool Chat::connect_to_db(MYSQL& ms){
	while(true){
		std::cout << " Enter username of your database MySQL: ";
		username_db = reg_data_mysql();
		std::cout << " Enter password of your database MySQL: ";
		password_db = reg_data_mysql();

		if (!mysql_real_connect(&ms, "localhost", username_db.c_str(), password_db.c_str(), database_chat.c_str(), 0, NULL, 0)) {
			// Если нет возможности установить соединение с БД выводим сообщение об ошибке
			std::cout << "Error: can't connect to database " << mysql_error(&ms) << std::endl;
			if (mysql_errno(&ms) == not_db){
				std::cout << " No database " <<  database_chat << std::endl;
				std::cout << " need for create database" << std::endl;
				return false;
				break;
			}
			else std::cout << " Invalid username or password" << std::endl;
			continue;
		}
		else {
			// Если соединение успешно установлено выводим фразу — "Success!"
			std::cout << "\n Success!" << std::endl;
			return true;
			break;
		}
	}
}

//----------------------- Сщздание базы двнных -------------------------
void Chat::create_database(MYSQL&ms){
	if(!mysql_real_connect(&ms, "localhost", username_db.c_str(), password_db.c_str(), NULL, 0, NULL, 0))
		std::cout << " Error: don't connect to server MySQL" << mysql_error(&ms) << std::endl;
		else std::cout << " Connected to server MySQL" << std::endl;

	if(!mysql_query(&ms, "CREATE DATABASE chat"))
		std::cout << " Created database chat" << std::endl;
		else std::cout << " Error: don't create database chat" << std::endl;

	if(!mysql_query(&ms, "USE chat"))
		std::cout << " use chat applied" << std::endl;
		else std::cout << " Error: don't use database chat" << std::endl;
}

//----------------------- Создание таблицы ------------------------------
void Chat::create_table(MYSQL& ms){
	if(!mysql_query(&ms, "CREATE TABLE users(id INT AUTO_INCREMENT PRIMARY KEY, login VARCHAR(255) NOT NULL UNIQUE, name VARCHAR(255), hash VARCHAR(255))"))
		std::cout << " Table " << table_users << " created" <<std::endl;
		else std::cout << " Error: don't created table " << table_users << mysql_error(&ms) << std::endl;

	insert_into_users(ms, table_users, "ALL USERS", "Общий чат", "root");

	if(!mysql_query(&ms, "CREATE TABLE messages(id INT AUTO_INCREMENT PRIMARY KEY, sender VARCHAR(255), recipient varchar(255), event VARCHAR(255), mess TEXT)"))
		std::cout << " Table " << table_mess << " created" << std::endl;
		else std::cout << " Error: don't created table " << table_mess << mysql_error(&ms) << std::endl;

	insert_into_messsage(ms, table_mess, "non", "non", "non", "non");
}

//----------------------- Вставка данных в таблицу пользователей --------
void Chat::insert_into_users(MYSQL&ms, std::string db, std::string log, std::string name, std::string hash){
	std::string str = "INSERT INTO " + db + "(id, login, name, hash) VALUES (default, \'" + log + "\', \'" + name + "\', \'" + hash + "\')";
	if (!mysql_query(&ms, str.c_str())) 
		std::cout << " Участник "<< log << " добавлен" << std::endl;
		else std::cout << " Error: участник "<< log << " не добавлен" << std::endl << mysql_error(&ms) << std::endl;
}

// ---------------------- Вставка данных в таблицу сообщений --------------
void Chat::insert_into_messsage(MYSQL& ms, std::string db, std::string send, std::string rec, std::string ev, std::string mess){
	std::string str = "INSERT INTO " + db + "(id, sender, recipient, event, mess) VALUES (default, \'" + send + "\', \'" + rec + "\' ,\'" + ev + "\' ,\'" + mess + "\')";
	if (!mysql_query(&ms, str.c_str()))
		std::cout << " Сообщение от " << send << " добавлено в таблицу" << std::endl;
		else std::cout << " Error: сообщение от " << send << " не добавлено в таблицу" << std::endl << mysql_error(&ms) << std::endl;
}

//----------------------- Вывод таблицы ------------------------------------
void Chat::show_table(MYSQL&ms, MYSQL_RES* res, MYSQL_ROW& row, std::string table){
	std::string str = "SELECT * FROM " + table;
	if(!mysql_query(&ms, str.c_str())){
		std::cout << " Таблица " << table << ":" << std::endl;
		if( res = mysql_store_result(&ms)){
		while(row = mysql_fetch_row(res)){
			for (int i = 0; i < mysql_num_fields(res); i++){
				std::cout << "| " << row[i] << " |";
			}
			std::cout << std::endl;
		}
	}
	else std::cout << " Ошибка mysql: " << mysql_error(&ms);
	}
		else std::cout << " Error: таблица не выведена " << std::endl << mysql_error(&ms) << std::endl;
}

//--------------------------------------------------------------------------------------------

//--------------------------------- Работа сети ----------------------------------------------
//----------- Создание сокета ---------------------------
void Chat::socket_file() {
	_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_file_descriptor == -1) {
		std::cout << " Не удалось создать сокет!" << std::endl;
		exit(1);
	}
}

//----------- Закрытие сокета ------------------------------
void Chat::close_socket() {
	close(_socket_file_descriptor);
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	std::cout << std::ctime(&end_time) << " сокет закрыт" << std::endl;

}

//------------ Настройка порта и привязка сокета -------------
void Chat::server_address() {
	// 
	_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	// Зададим номер порта для связи
	_server_address.sin_port = htons(PORT);
	// Используем IPv4
	_server_address.sin_family = AF_INET;
	// Привяжем сокет
	_bind_status = bind(_socket_file_descriptor, (struct sockaddr*)&_server_address,
		sizeof(_server_address));
	if (_bind_status == -1) {
		std::cout << " Не удалось выполнить привязку сокета!" << std::endl;
		exit(1);
	}
}

//------------ Постановка сервера на прием ----------------------
void Chat::connect() {
	_connection_status = listen(_socket_file_descriptor, 5);
	if (_connection_status == -1) {
		std::cout << " Сокет не может прослушивать новые подключения!" << "\n";
		exit(1);
	}
	else {
		std::cout << " Ожидание данных: " << "\n";
	}
	_length = sizeof(_client);
	_connection = accept(_socket_file_descriptor, (struct sockaddr*)&_client, &_length);
	if (_connection == -1) {
		std::cout << " Сервер не может принять данные от клиента!" << "\n";
		exit(1);
	}
}

//----------- Обмен данными ------------------------------
void Chat::exchange(const std::string& mess) {
	transmitting(mess);
	std::cout << " Ожидание данных \n";
	receiving_user();
}


//---------- Парсинг сообщения -------------------------------
/*std::string Chat::get_parsing(const std::string& mess, std::string word, int count) {
	int counter{ 0 };
	for (int i = 0; i < mess.size(); i++) {
		if (mess[i] != ' ' && counter == count) {
			word += mess[i];
			continue;
		}
		else if (mess[i] == ' ') counter++;
		if (counter > count) break;
	}
}*/

//-------------- Отправка данных -----------------------------------
void Chat::transmitting(const std::string& mess){
	memset(_message, 0, sizeof(_message));
	strcpy(_message, mess.c_str());
	ssize_t bytes = write(_connection, _message, sizeof(_message));
	transmitted(message());
}
//----------------------------------------

//-------------- Прием данных от пользователе --------------------
void Chat::receiving_user() {
	memset(_message, 0, sizeof(_message));
	read(_connection, _message, sizeof(_message));
	accepted(message());
}

//-------------- Возврат первого символа сообщения --------------------
char Chat::message0() {
	return _message[0];
}

//------------- Перевод сообщения в string ---------------------------
std::string Chat::message() {
	return std::string(_message);
}

void Chat::accepted(std::string mess){
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	std::cout << std::ctime(&end_time) << " принято: " << mess << std::endl;
}

void Chat::transmitted(std::string mess){
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	std::cout << std::ctime(&end_time) << " передано: " << mess << std::endl;
}
//------------------------------------------------------------------------------------------

//---------------- Работа чата -------------------------------------------------------------

//---------------- Приветствие ------------------------------------------------------------
void Chat::greeting() {
	clean_console();
	std::cout << "\n          Сервер включён!\n\n";
}

//----------------- Прощание --------------------------------------------------------------
void Chat::farewell() {
	std::cout << " Пользователь отключился от сети.\n";
}

//--------------- Проверка логина -------------------------------------------------------
bool Chat::finding(std::string login) {
	if (_users.find(login) == _users.end()) return true;
	else return false;
}

//--------------- Проверка пароля -------------------------------------------------------
bool Chat::check_password(std::string password, std::string login) {
	if (_users.at(login).user_password() == password) return true;
	else return false;
}

//--------------- Вход и регистрация ----------------------------------------------------
void Chat::registration(char menu, bool* check_user) {
	User user;

	*check_user = false;
	// Вход в чат
	if (menu == '1') {
		exchange(" Введите логин(латинский алфавит, цифры, символы): ");
		user.get_user_login(message());
		exchange(" Введите пароль(латинский алфавит, цифры, символы): ");
		user.get_user_password(message());
		int counter = 0;

		if (!finding(user.user_login()) && check_password(user.user_password(), user.user_login())) {
			get_user(user.user_login(), _users.at(user.user_login()).user_name());
			transmitting("вход выполнен");
			*check_user = true;
		}
		else {
			transmitting("false");
			return;
		}

	}
	// регистрация нового пользователя
	else {
		*check_user = true;
		user.get_user_name(message());
		exchange(user.user_name());
		exchange(" Введите логин (латинский алфавит, цифры, символы): ");
		bool check_login;
		do {
			check_login = true;
			user.get_user_login(message());
			if (!finding(user.user_login())) {
				user.clear_login();
				exchange("false");
				check_login = false;
			}
		} while (!check_login);
		exchange(" логин принят");
		exchange( " Введите пароль (латинский алфавит, цифры, символы): ");
		user.get_user_password(message());

		_users.emplace(user.user_login(), user);
		get_user(user.user_login(), user.user_name());
		exchange("регистрация прошла успешно");
	}
}

//----------------- Регистрация общего чата ---------------------------------------
void Chat::reg_all_user() {
	User user;
	user.get_user_login("ALL_USERS");
	user.get_user_password("admin");
	user.get_user_name("общий чат");
	_users.emplace(user.user_login(), user);
}

//--------------- Возвращает логин активного пользователя --------------------------
std::string Chat::active_user_login() {
	return _active_user_login;
}

//--------------- Возвращает имя активного пользователя -----------------------------
std::string Chat::active_user_name() {
	return _active_user_name;
}

//--------------- Возвращает логин получателя ---------------------------------------
std::string Chat::active_recipient_login() {
	return _active_recipient_login;
}

//--------------- Активация пользователя --------------------------------------------
void Chat::get_user(std::string login, std::string name)
{
	_active_user_login = login;
	_active_user_name = name;
}

//--------------- Деактивация пользователя ------------------------------------------
void Chat::out_user() {
	_active_user_login = '\0';
	_active_user_name = '\0';

}

//--------------- Выбор получателя -------------------------------------------------
void Chat::get_recipient(char menu) {

	int counter = 0;
	if (menu == '2') _active_recipient_login = "ALL_USERS";
	else {
		int id{ 0 };
		do {
			receiving_user();
			id = stoi(message());
			if (id < 1 || id > _users.size()) {
				transmitting(" Не верный ID, повторите выбор: ");
			}
			else {
				transmitting(" ID принят");
				break;
			}

		} while (true);

		std::map<std::string, User>::iterator it = _users.begin();
		for (; it != _users.end(); it++) {
			counter++;
			if (counter == id) break;
		}

		_active_recipient_login = it->second.user_login();
	}
}

//------------------ Отправка сообщение об отсутствии других пользователей
void Chat::one_user(){
	transmitting(" Вы пока единственный пользователь.\n Дождитесь регистрации других пользователей.\n");
}

//------------- Определение количества пользователей --------------------------------
int Chat::sizeList() {
	return _users.size();
}

//------------- Создание и отправка сообщения ---------------------------------------
void Chat::send_message() {
	Message messages;
	std::string mess{};
	char menu{ ' ' };
	while (true) {
		exchange("запрос на действия");

		if (message() == "Esc") {
			transmitting("очистка");
			break;
		}
		else if (message() == "enter") {
			exchange("сообщение:");
			mess = message();
			messages.create_message(mess, _active_user_name, _active_user_login, _active_recipient_login);
			_messages.push_back(messages);
			std::cout << "\n Сообщение ";
			if (_active_recipient_login == "ALL_USERS") std::cout << " в общий чат\n";
			else {
				std::cout << " для ";
				_users.at(_active_recipient_login).showUserName();
			}
			transmitting(" отправлено");
			break;
		}
		else transmitting("хм, можно повторить ?");
	}
}

//----------------- Основная функция работы чата -------------------------------------------
void Chat::chat_work(){
	MYSQL mysql; // Дескриптор соединения c MySql
	MYSQL_RES* res;
	MYSQL_ROW row;
	mysql_init(&mysql);

	reg_all_user();
	
	test_msql_descriptor(mysql);
	if(!connect_to_db(mysql)){
		create_database(mysql);
		create_table(mysql);
	}
	mysql_set_character_set(&mysql, "utf8");
	std::cout << " connection characterset " << mysql_character_set_name(&mysql) << std::endl;
	show_table(mysql, res, row, table_users);
	show_table(mysql, res, row, table_mess);

	socket_file();
	server_address();
	connect();
	exchange(" проверка связи");

	while(_work) {
		std::cout << " ожидание данных\n";
		receiving_user();
		_menu = message0();
		if (_menu == '3') {
			transmitting(" сервер завершил работу ");
			close_socket();
			mysql_close(&mysql);
			farewell();
			exit(0);
		}
		// вход в аккаунт
		else if (_menu == '1') {
			if (sizeList() == 1) {
				transmitting("error");
				continue;
			}
			if (sizeList() == 2) {
				transmitting("error2");
				one_user();
				continue;
			}
			transmitting(" Вход");
			registration(_menu, &_check_user);
			if (_check_user == false) {
				continue;
			}
		}
		//регистрация нового пользователя
		else {
			exchange(" Регистрация:");
			registration(_menu, &_check_user);
		}
		// проверка кол-ва зарегистрированных
		if (sizeList() == 2) {
			transmitting("error2");
			one_user();
			continue;
		}
		else transmitting("список доступен");
		//работа аккаунта
		account_work();
	}
}

//--------------------- Функция работы аккаунта ------------------------------------
void Chat::account_work(){

	do {
		receiving_user();
		_menu = message0();
		_discussion = true;
		switch (_menu) {
		case '1': // личная беседа
			transmitting(" ID собеседника: ");
			get_recipient(_menu);
			send_message();
			break;
		case '2': // сообщение всем
			get_recipient(_menu);
			send_message();
			break;
		case '3': // выход
			transmitting(" Вы покинули аккаунт.");
			out_user();
			_discussion = false;
			break;

		default:
			break;
		}

	} while (_discussion);
}

//----------------- Очистка консоли -------------------------------------------------------
void Chat::clean_console() {
#ifdef _WIN32
	std::system("cls");  // Очистка экрана в системе Windows
#else
	std::system("clear");  // Очистка экрана в системах UNIX и Linux
#endif
}

//------------------- Пауза ----------------------------------------------------------------
void Chat::system_pause(int second){
	std::this_thread::sleep_for(std::chrono::seconds(second));
}
