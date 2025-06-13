#pragma once
#include <string>
#include <UreTechEngine/gmodule/gmodule.h>

namespace UreTechEngine {
	class EngineConsole;
	typedef void (*uStrConsoleLogFuncPtr)(const char*, bool);
	extern uStrConsoleLogFuncPtr ENGINE_DEFINE strLog;
	class ENGINE_DEFINE string {
	private:
		char* s_data = nullptr;// data
		char* s_data_c_str_type = nullptr;// c_str type holder
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

		// c5 copier
		string(const string& _str);

		// c6 std::string copier
		string(const std::string& _sstr);

		// d1
		~string();

		// new operator
		void* operator new(size_t lenght);

		// index operator
		char& operator[](size_t index);

		// string cmp operator
		bool operator==(const string& other) const;

		// copies data into it self
		string operator=(const string other);

		// copies data into it self
		string operator=(const std::string other);

		// copies data into it self
		string operator=(const char* _str);

		// append operator
		string operator+=(const string other);

		// string + string add operator
		string operator+(const string other);

		// char + string add operator
		friend ENGINE_DEFINE UreTechEngine::string operator+(const char* other0, const UreTechEngine::string& other1);

		// string + char add operator
		//friend UreTechEngine::string operator+(const UreTechEngine::string& other0, const char* other1);

		// std::string + string add operator
		//friend UreTechEngine::string operator+(const std::string& other0, const UreTechEngine::string& other1);

		// std::string convertion
		//operator std::string();

		// is empty
		bool empty();

		// pushes new item to end
		size_t push_back(char c);

		// returns item at index (can be unsafe if not checked return)
		char& at(size_t i);

		// pops last item
		void pop_back();

		// += operator
		string& append(const string& other);

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

		static string stdStrToUStr(std::string _str);
	};
}
