#pragma once
#include <string>
#include <iostream>

namespace UreTechEngine {
	class EngineConsole;
	typedef void (*uStrConsoleLogFuncPtr)(const char*, bool);
	extern uStrConsoleLogFuncPtr strLog;
	class string {
	private:
		char* s_data = nullptr;// data
		size_t s_size = 0;// data size
		bool haltWithFatalError = false;

		// cstr to string
		void assign_cstr_type(const char* _cstr);
	public:

		// c1
		string();

		// c2
		string(char* _str, size_t _lenght);

		// c3
		string(const char* _str);

		// c4
		string(std::string _str);

		// c5 copier
		string(const string& _str);

		// d1
		~string();

		// new operator
		void* operator new(size_t lenght);

		// index operator
		char& operator[](size_t index);

		// string cmp operator
		bool operator==(const string& other);

		// std::string cmp operator
		friend bool operator==(std::string& other0, string& other1);

		// cstr cmp operator
		bool operator==(const char* other);

		// copies data into it self
		string& operator=(string other);

		// copies data into it self
		string& operator=(const char* _str);

		// append operator
		string& operator+=(string other);

		// string + string add operator
		string& operator+(string other);

		// string + std::string add operator
		string& operator+(std::string other);

		// char + string add operator
		friend string& operator+(const char* other0, string& other1);

		// char + string add operator
		friend string& operator+(string& other0, const char* other1);

		// std::string + string add operator
		friend string& operator+(std::string& other0, string& other1);

		// std::string convertion
		operator std::string();

		// is empty
		bool empty();

		// pushes new item to end
		size_t push_back(char c);

		// returns item at index (can be unsafe if not checked return)
		char& at(size_t i);

		// pops last item
		void pop_back();

		// += operator
		string& append(string& other);

		// substr
		string& substr(size_t len, size_t off = 0);

		// removes index
		bool remove(size_t index);

		// resizes array
		void resize(size_t size);

		// clears everything
		void clear();

		// swaps two item
		void swap(size_t i1, size_t i2);

		// returns stored data
		char* data();

		// returns item count
		size_t lenght();

		//converts c string type (\0)
		const char* c_str();

		//converts to std::string
		std::string std_str();

		// copies data into it self
		void assign(char* data, size_t size);
		void assign(const char* data, size_t size);

		// sets direct pointer to data
		void rawAssign(char* data, size_t size);
	};
}
