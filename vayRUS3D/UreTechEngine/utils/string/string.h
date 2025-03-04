#ifndef vayRUS_string_hpp
#define vayRUS_string_hpp
#include <../EngineCore.h>
namespace UreTechEngine {

	class string {
	private:
		char* s_data = nullptr;// data
		size_t s_size = 0;// data size

		// cstr to string
		void assign_cstr_type(const char* _cstr) {
			this->clear();
			while (_cstr != '\0') {
				this->push_back(*_cstr);
				_cstr++;
			}
		}
	public:
		bool haltWithFatalError = false;

		

		// c1
		string() {
			// null array
		}

		// c2
		string(size_t _size) {
			s_data = new char[_size];
		}

		// c3
		string(char* _str, size_t _lenght) {
			s_data = new char[_lenght];
			s_size = _lenght;
			memcpy(s_data, _str, _lenght * sizeof(char));
		}

		// c4
		string(const char* _str) {
			
		}

		// index operator
		char& operator[](size_t index) {
			return *(s_data + index);
		}

		// copies data into it self
		string& operator=(string other) {
			this->assign(other.s_data, other.s_size);
			return *this;
		}

		// copies data into it self
		string operator=(const char* _str) {
			this->assign_cstr_type(_str);
			return *this;
		}

		operator std::string() {
			return std::string(s_data, s_size);
		}

		// pushes new item to end
		size_t push_back(char c) {
			s_size++;
			s_data = (char*)realloc(s_data, s_size * sizeof(char));
			if (s_data == nullptr) {
				if (haltWithFatalError) {
					EngineConsole::log("string: Invalid resize operation in 'push_back'! (FATAL STRING)", EngineConsole::t_error::ERROR_FATAL);
				}
				else {
					EngineConsole::log("string: Invalid resize operation in 'push_back'! (CAN CAUSE FATAL ERROR)", EngineConsole::t_error::ERROR_NORMAL);
				}
			}
			this[s_size - 1] = c;
			return this->s_size - 1;
		}

		// returns item at index (can be unsafe if not checked return)
		char& at(size_t i) {
			if (i < s_size) {
				return *(s_data + i);
			}
			else {
				EngineConsole::log("string: Invalid array access in 'at'!", EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
				return;
			}
		}

		// pops last item
		void pop_back() {
			s_size--;
			s_data = (char*)realloc(s_data, s_size * sizeof(char));
			if (s_data == nullptr) {
				if (haltWithFatalError) {
					EngineConsole::log("string: Invalid resize operation in 'pop_back'! (FATAL STRING)", EngineConsole::t_error::ERROR_FATAL);
				}
				else {
					EngineConsole::log("string: Invalid resize operation in 'pop_back'! (CAN CAUSE FATAL ERROR)", EngineConsole::t_error::ERROR_NORMAL);
				}
			}
		}

		// removes index
		bool remove(size_t index) {
			if (index >= s_size) {
				EngineConsole::log("string: Invalid array access in 'remove'!", EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
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
					EngineConsole::log("string: Invalid resize operation in 'resize'! (FATAL STRING)", EngineConsole::t_error::ERROR_FATAL);
				}
				else {
					EngineConsole::log("string: Invalid resize operation in 'resize'! (CAN CAUSE FATAL ERROR)", EngineConsole::t_error::ERROR_NORMAL);
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
				EngineConsole::log("string: Invalid array access in 'swap'!", EngineConsole::t_error::WARN_CAN_CAUSE_ERROR);
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

		// copies data into it self
		void assign(char* data, size_t size) {
			this->clear();
			s_data = new char[size+1];
			memcpy((void*)s_data, data, size * sizeof(char));
		}

		// sets direct pointer to data
		void rawAssign(char* data, size_t size) {
			this->clear();
			s_size = size;
			s_data = data;
		}
	};
}
#endif // !vayRUS_string_hpp
