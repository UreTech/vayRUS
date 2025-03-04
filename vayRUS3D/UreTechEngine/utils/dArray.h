#ifndef Array_hpp
#define Array_hpp
#include <initializer_list>
#include <../UreTechEngine/utils/string/string.h>
namespace UreTechEngine {
	extern uStrConsoleLogFuncPtr strLog;
	template<typename arrType>
	class dArray {
	private:
		arrType* a_data = nullptr;// data
		size_t a_size = 0;// data size
	public:
		bool haltWithFatalError = false;
		// c1
		dArray() {
			// null array
		}

		// c2
		dArray(size_t _size) {
			a_data = new arrType[_size];
			a_size = _size;
		}

		// c3
		dArray(arrType* _data, size_t _size) {
			a_data = new arrType[_size];
			a_size = _size;
			memcpy((void*)a_data, _data, _size * sizeof(arrType));
		}

		// c4
		dArray(std::initializer_list<arrType> list) {
			this->assign((arrType*)list.begin(), list.size());
		}

		// index operator
		arrType& operator[](size_t index) {
			return this->at(index);
		}

		// copies data into it self
		dArray operator=(dArray other) {
			this->assign(other.a_data, other.a_size);
			return *this;
		}

		// copies data into it self
		dArray operator=(std::initializer_list<arrType> list) {
			this->assign((arrType*)list.begin(), list.size());
			return *this;
		}

		// pushes new item to end
		size_t push_back(arrType v) {
			a_size++;
			a_data = (arrType*)realloc(a_data, a_size * sizeof(arrType));
			if (a_data == nullptr) {
				if (haltWithFatalError) {
					strLog("Array: Invalid resize operation in 'push_back'! (FATAL ARRAY)", true);
				}
				else {
					strLog("Array: Invalid resize operation in 'push_back'! (CAN CAUSE FATAL ERROR)", false);
				}
			}
			memcpy(&a_data[a_size - 1], &v, sizeof(arrType));
			return this->a_size - 1;
		}

		// returns item at index (can be unsafe if not checked return)
		arrType& at(size_t i) {
			if (i < a_size) {
				return *(a_data + i);
			}
			else {
				strLog("Array: Invalid array access in 'at'!", false);
				return *(a_data);// maybe ultra unsafe too :_(
			}
		}

		// pops last item
		void pop_back() {
			a_size--;
			a_data = (arrType*)realloc(a_data, a_size * sizeof(arrType));
			if (a_data == nullptr) {
				if (haltWithFatalError) {
					strLog("Array: Invalid resize operation in 'pop_back'! (FATAL ARRAY)", true);
				}
				else {
					strLog("Array: Invalid resize operation in 'pop_back'! (CAN CAUSE FATAL ERROR)", false);
				}
			}
		}

		// removes index
		bool remove(size_t index) {
			if (index >= a_size) {
				strLog("Array: Invalid array access in 'remove'!", false);
				return false;
			}

			for (size_t i = index; i < a_size - 1; ++i) {
				*(a_data + i) = *(a_data + i + 1);
			}

			a_size--;
			this->resize(a_size);
		}

		// resizes array
		void resize(size_t size) {
			a_size = size;
			a_data = (arrType*)realloc(a_data, size * sizeof(arrType));
			if (a_data == nullptr) {
				if (haltWithFatalError) {
					strLog("Array: Invalid resize operation in 'resize'! (FATAL ARRAY)", true);
				}
				else {
					strLog("Array: Invalid resize operation in 'resize'! (CAN CAUSE FATAL ERROR)", false);
				}
			}
		}

		// clears everything
		void clear() {
			a_size = 0;
			delete[] a_data;
		}

		// swaps two item
		void swap(size_t i1, size_t i2) {

			if (i1 < a_size && i2 < a_size) {
				arrType tmp = this->at(i1);
				this->at(i1) = this->at(i2);
				this->at(i2) = tmp;
			}
			else {
				strLog("Array: Invalid array access in 'swap'!", false);
			}
		}

		// returns stored data
		arrType* data(){
			return a_data;
		}

		// returns item count
		size_t size() {
			return a_size;
		}

		// returns raw array size
		size_t rawSize() {
			return a_size * sizeof(arrType);
		}
		
		// copies data into it self
		void assign(arrType* data, size_t size) {
			this->clear();
			a_data = new arrType[size];
			a_size = size;
			memcpy((void*)a_data, data, size * sizeof(arrType));
		}

		// sets direct pointer to data
		void rawAssign(arrType* data, size_t size) {
			this->clear();
			a_size = size;
			a_data = data;
		}
	};
}
#endif // !Array_hpp
