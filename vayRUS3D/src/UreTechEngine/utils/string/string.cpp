#include "string.h"
#include <iostream>
using namespace UreTechEngine;

//local funcs

const char nullStrRet = '\0';

string u_str(std::string _str) {
	string tmp;
	tmp.assign(_str.data(), _str.size());
	return tmp;
}

// private funcs
void string::assign_cstr_type(const char* _cstr)
{
	this->clear();
	size_t len = strlen(_cstr);
	this->resize(len);
	memcpy(this->s_data, _cstr, len);
}

// constructors
string::string()
{
	// null array
	s_size = 0;
	s_data = nullptr;
	haltWithFatalError = false;
}

string::string(char* _str, size_t _lenght)
{
	s_size = 0;
	s_data = nullptr;
	haltWithFatalError = false;
	this->assign(_str, _lenght);
}

string::string(const char* _str)
{
	s_size = 0;
	s_data = nullptr;
	haltWithFatalError = false;
	assign_cstr_type(_str);
}

string::string(const string& _str)
{
	s_size = 0;
	s_data = nullptr;
	haltWithFatalError = false;
	this->assign(_str.s_data, _str.s_size);
}

string::string(const std::string& _sstr)
{
	s_size = 0;
	s_data = nullptr;
	haltWithFatalError = false;
	this->assign(_sstr.data(), _sstr.size());
}

UreTechEngine::string::~string()
{
	if (this != nullptr) {
		this->clear();
	}
}

// operators
void* string::operator new(size_t lenght)
{
	string* tmp = (string*)malloc(sizeof(string));
	if (tmp != nullptr) {
		memset(tmp, 0, sizeof(string));
		tmp->s_data = nullptr;
		tmp->s_size = 0;
	}
	return tmp;
}

char& string::operator[](size_t index)
{
	if (index < s_size) {
		return s_data[index];
	}
	else {
		return *(char*)nullStrRet;
	}
}

bool string::operator==(const string& other) const
{
	//std::cout << ("string: Comparing strings: '" + *this + "' == '" + other + "'\n").c_str();
	return s_size == other.s_size &&
		memcmp(s_data, other.s_data, s_size) == 0;
}

string string::operator=(const string other) {
	this->assign(other.s_data, other.s_size);
	return *this;
}

string string::operator=(const std::string other) {
	this->assign(other.data(), other.size());
	return *this;
}

string string::operator=(const char* _str) {
	this->assign_cstr_type(_str);
	return *this;
}

string string::operator+=(const string other) {
	this->append(other);
	return *this;
}

string string::operator+(const string other) {
	string tmp(*this);
	tmp.append(other);
	return tmp;
}

UreTechEngine::string UreTechEngine::operator+(const char* other0, const UreTechEngine::string& other1)
{
	string tmp(other0);
	tmp.append(string(other1));
	return tmp;
}

/*
UreTechEngine::string UreTechEngine::operator+(const UreTechEngine::string& other0, const char* other1)
{
	string tmp(other0);
	tmp.append(string(other1));
	return tmp;
}

UreTechEngine::string UreTechEngine::operator+(const std::string& other0, const UreTechEngine::string& other1)
{
	string tmp((char*)other0.c_str(), other0.size());
	tmp.append(string(other1));
	return tmp;
}
*/
/*
UreTechEngine::string::operator std::string()
{
	return std::string(s_data, s_size);
}
*/
// public funcs
bool string::empty()
{
	return s_size == 0;
}

size_t string::push_back(char c)
{
	//std::cout << c << "\n";
	s_size++;
	char* new_s_data = (char*)realloc(s_data, s_size);

	if (new_s_data == nullptr) {
		if (haltWithFatalError) {
			strLog("string: Invalid resize operation in 'push_back'! (FATAL STRING)", true);
		}
		else {
			strLog("string: Invalid resize operation in 'push_back'! (CAN CAUSE FATAL ERROR)", false);
			this->clear();
		}
	}
	else {
		s_data = new_s_data;
	}
	s_data[s_size - 1] = c;

	return this->s_size - 1;
}

char& string::at(size_t i)
{
	if (i < s_size) {
		return *(s_data + i);
	}
	else {
		strLog("string: Invalid array access in 'at'!", false);
		return *(s_data);// maybe ultra unsafe
	}
}

void string::pop_back()
{
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

string& string::append(const string& other)
{
	for (uint64_t i = 0; i < other.s_size; i++) {
		this->push_back(*(other.s_data + i));
		//std::cout << *(other.s_data + i) << "\n";
	}

	return *this;
}

string& UreTechEngine::string::substr(size_t len, size_t off)
{
	string* tmp = new string();
	for (uint64_t i = off; i < off + len && i < this->s_size; i++) {
		tmp->push_back(s_data[i]);
	}
	return *tmp;
}

bool string::remove(size_t index)
{
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

void string::resize(size_t size)
{
	s_size = size;
	char* new_s_data = (char*)realloc(s_data, size);
	if (new_s_data == nullptr) {
		if (haltWithFatalError) {
			strLog("string: Invalid resize operation in 'resize'! (FATAL STRING)", true);
		}
		else {
			strLog("string: Invalid resize operation in 'resize'! (CAN CAUSE FATAL ERROR)", false);
		}
	}
	else {
		s_data = new_s_data;
	}
}

void string::clear()
{
	if (s_data != nullptr) {
		free(s_data);
	}
	if (s_data_c_str_type != nullptr) {
		free(s_data_c_str_type);
	}
	s_size = 0;
	s_data = nullptr;
	s_data_c_str_type = nullptr;
}

void string::swap(size_t i1, size_t i2)
{
	if (i1 < s_size && i2 < s_size) {
		char tmp = this->at(i1);
		this->at(i1) = this->at(i2);
		this->at(i2) = tmp;
	}
	else {
		strLog("string: Invalid array access in 'swap'!", false);
	}
}

char* string::data()
{
	return s_data;
}

size_t string::lenght()
{
	return s_size;
}

const char* string::c_str()
{
	if (s_data_c_str_type != nullptr) {
		free(s_data_c_str_type);
		s_data_c_str_type = (char*)malloc(s_size + 1);
	}
	else {
		s_data_c_str_type = (char*)realloc(s_data_c_str_type, s_size + 1);
	}
	memcpy(s_data_c_str_type, s_data, s_size);
	s_data_c_str_type[s_size] = '\0';
	return s_data_c_str_type;
}

std::string string::std_str()
{
	return std::string(this->s_data, this->s_size);
}

void string::assign(char* data, size_t size)
{
	this->clear();
	if ((data == (void*)0xcdcdcdcdcdcdcdcd)) {
		std::cout << "null1 XD";
	}
	if ((this->s_data == (void*)0xcdcdcdcdcdcdcdcd)) {
		std::cout << "null2 XD";
	}
	if (size) {
		this->s_size = size;
		this->s_data = (char*)malloc(this->s_size);
		if (this->s_data != nullptr) {
			memcpy(this->s_data, data, size);
		}
	}
}

void string::assign(const char* data, size_t size)
{
	this->clear();
	if (size == 0) {
		strLog("string: Null assign in 'assign'!", false);
	}
	this->s_size = size;
	this->s_data = (char*)malloc(this->s_size);
	if (this->s_data != nullptr) {
		memcpy(this->s_data, data, size);
	}
}

void string::rawAssign(char* data, size_t size)
{
	this->clear();
	s_size = size;
	s_data = data;
}

string UreTechEngine::string::stdStrToUStr(std::string _str)
{
	return string(_str.data(), _str.size());
}
