#pragma once
#include <string>
#include <iostream>

namespace UreTechEngine {
	class EngineConsole;
	typedef void (*uStrConsoleLogFuncPtr)(const char*, bool);
	extern uStrConsoleLogFuncPtr strLog;
	class uStr {
	private:
		char* s_data = nullptr;// data
		size_t s_size = 0;// data size

		// cstr to string
		void assign_cstr_type(const char* _cstr) {
			this->clear();
			size_t strLen = strlen(_cstr);
			this->assign((char*)_cstr, strLen);
		}
	public:
		bool haltWithFatalError = false;

		// c1
		uStr() {
			// null array
		}

		// c2
		uStr(char* _str, size_t _lenght) {
			this->assign(_str, _lenght);
		}

		// c3
		uStr(const char* _str) {
			assign_cstr_type(_str);
		}

		// c4
		uStr(std::string _str) {
			assign_cstr_type(_str.c_str());
		}

		// index operator
		char& operator[](size_t index) {
			return s_data[index];
		}

		// string cmp operator
		bool operator==(const uStr& other) {
			if (this->s_size == other.s_size) {
				return memcmp(this->s_data, other.s_data, this->s_size);
			}
			else {
				return false;
			}
		}

		// copies data into it self
		uStr& operator=(uStr other) {
			this->assign(other.s_data, other.s_size);
			return *this;
		}

		// copies data into it self
		uStr operator=(const char* _str) {
			this->assign_cstr_type(_str);
			return *this;
		}

		// append operator
		uStr& operator+=(uStr other) {
			this->append(other);
			return *this;
		}

		// string + string add operator
		uStr& operator+(uStr other) {
			uStr tmp(*this);
			tmp.append(other);
			return tmp;
		}

		// string + std::string add operator
		uStr& operator+(std::string other) {
			uStr tmp(*this);
			tmp.append(uStr(other.c_str()));
			return tmp;
		}

		// string + char add operator
		uStr& operator+(const char* other)const {
			uStr tmp(*this);
			tmp.append(uStr(other));
			return tmp;
		}

		// char + string add operator
		friend uStr operator+(const char* other0, uStr& other1) {
			uStr tmp(other0);
			tmp.append(other1);
			return tmp;
		}

		// std::string + string add operator
		friend uStr operator+(std::string& other0, uStr& other1) {
			uStr tmp(other0.c_str());
			tmp.append(other1);
			return tmp;
		}

		// std::string convertion
		operator std::string() {
			return std::string(s_data, s_size);
		}

		// is empty
		bool empty() {
			return s_size == 0;
		}

		// pushes new item to end
		size_t push_back(char c) {
			s_size++;
			char* new_s_data = (char*)realloc(s_data, s_size * sizeof(char));
			if (new_s_data == nullptr) {
				if (haltWithFatalError) {
					strLog("string: Invalid resize operation in 'push_back'! (FATAL STRING)", true);
				}
				else {
					strLog("string: Invalid resize operation in 'push_back'! (CAN CAUSE FATAL ERROR)", false);
				}
			}
			else {
				s_data = new_s_data;
			}
			s_data[s_size - 1] = c;
			return this->s_size - 1;
		}

		// returns item at index (can be unsafe if not checked return)
		char& at(size_t i) {
			if (i < s_size) {
				return *(s_data + i);
			}
			else {
				strLog("string: Invalid array access in 'at'!", false);
				return *(s_data);// maybe ultra unsafe
			}
		}

		// pops last item
		void pop_back() {
			s_size--;
			s_data = (char*)realloc(s_data, s_size * sizeof(char));
			if (s_data == nullptr) {
				if (haltWithFatalError) {
					strLog("string: Invalid resize operation in 'pop_back'! (FATAL STRING)", true);
				}
				else {
					strLog("string: Invalid resize operation in 'pop_back'! (CAN CAUSE FATAL ERROR)", false);
				}
			}
		}

		// += operator
		uStr& append(const uStr& other) {
			for (uint64_t i = 0; i < other.s_size; i++) {
				this->push_back(other.s_data[i]);
			}
			return *this;
		}

		// removes index
		bool remove(size_t index) {
			if (index >= s_size) {
				strLog("string: Invalid array access in 'remove'!", false);
				return false;
			}

			for (size_t i = index; i < s_size - 1; ++i) {
				*(s_data + i) = *(s_data + i + 1);
			}

			s_size--;
			this->resize(s_size);
		}

		// resizes array
		void resize(size_t size) {
			s_size = size;
			s_data = (char*)realloc(s_data, size * sizeof(char));
			if (s_data == nullptr) {
				if (haltWithFatalError) {
					strLog("string: Invalid resize operation in 'resize'! (FATAL STRING)", true);

				}
				else {
					strLog("string: Invalid resize operation in 'resize'! (CAN CAUSE FATAL ERROR)", false);
				}
			}
		}

		// clears everything
		void clear() {
			s_size = 0;
			delete[] s_data;
		}

		// swaps two item
		void swap(size_t i1, size_t i2) {

			if (i1 < s_size && i2 < s_size) {
				char tmp = this->at(i1);
				this->at(i1) = this->at(i2);
				this->at(i2) = tmp;
			}
			else {
				strLog("string: Invalid array access in 'swap'!", false);
			}
		}

		// returns stored data
		const char* data() {
			return s_data;
		}

		// returns item count
		size_t lenght() {
			return s_size;
		}

		//converts c string type (\0)
		const char* c_str() {
			char* buf = (char*)malloc(s_size + 1);
			buf[s_size] = '\0';
			memcpy(buf, s_data, s_size);
			return buf;
		}

		//converts to std::string
		std::string std_str() {
			return std::string(this->s_data, this->s_size);
		}

		// copies data into it self
		void assign(char* data, size_t size) {
			this->clear();
			s_data = new char[size];
			memcpy((void*)s_data, data, size * sizeof(char));
			s_size = size;
		}

		// sets direct pointer to data
		void rawAssign(char* data, size_t size) {
			this->clear();
			s_size = size;
			s_data = data;
		}
	};
}
#define uStr_def_end