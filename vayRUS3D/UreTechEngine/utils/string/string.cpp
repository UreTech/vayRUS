#include "string.h"

using namespace UreTechEngine;

//local funcs

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
	this->resize(len + 1);
	memcpy(this->s_data, _cstr, len + 1);
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
	this->assign(_str, _lenght);
}

string::string(const char* _str)
{
	assign_cstr_type(_str);
}

string::string(std::string _str)
{
	assign_cstr_type(_str.c_str());
}

string::string(const string& _str)
{
	if (_str.s_size != 0) {
		this->assign(_str.s_data, _str.s_size);
	}
	else {
		//bruh normal construct
		s_size = 0;
		s_data = nullptr;
		haltWithFatalError = false;
	}
}

UreTechEngine::string::~string()
{
	this->clear();
}

// operators
void* string::operator new(size_t lenght)
{
	string* tmp = (string*)malloc(sizeof(string));
	tmp->resize(lenght);
	return tmp;
}


char& string::operator[](size_t index)
{
	return s_data[index];
}

bool string::operator==(const string& other)
{
	if (this->s_size == other.s_size) {
		return !memcmp(this->s_data, other.s_data, this->s_size);
	}
	else {
		return false;
	}
}

bool string::operator==(const std::string& other)
{
	if (this->s_size == other.size()) {
		return memcmp(this->s_data, other.c_str(), this->s_size);
	}
	else {
		return false;
	}
}

bool string::operator==(const char* other) {
	string ot(other);
	if (this->s_size == ot.s_size) {
		return memcmp(this->s_data, ot.s_data, this->s_size);
	}
	else {
		return false;
	}
}

string string::operator=(string other) {
	this->assign(other.s_data, other.s_size);
	return *this;
}

string string::operator=(const char* _str) {
	this->assign_cstr_type(_str);
	return *this;
}

string string::operator+=(string other) {
	this->append(other);
	return *this;
}

string string::operator+(string other) {
	string tmp(*this);
	tmp.append(other);
	return tmp;
}

string string::operator+(std::string other) {
	string tmp(*this);
	tmp.append(u_str(other));
	return tmp;
}

string string::operator+(const char* other) const
{
	string tmp(*this);
	tmp.append(string(other));
	return tmp;
}

string UreTechEngine::operator+(const char* other0, string& other1)
{
	string tmp(other0);
	tmp.append(other1);
	return tmp;
}

string UreTechEngine::operator+(std::string& other0, string& other1)
{
	string tmp(other0.data(),other0.size());
	tmp.append(other1);
	return tmp;
}

UreTechEngine::string::operator std::string()
{
	return std::string(s_data, s_size);
}

// public funcs
bool string::empty()
{
	return s_size == 0;
}

size_t string::push_back(char c)
{
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

string& string::append(string& other)
{

	for (uint64_t i = 0; i < other.s_size; i++) {
		this->push_back(other.s_data[i]);
	}

	return *this;
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
	s_size = 0;
	delete[] s_data;
	s_data = nullptr;
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
	char* buf = (char*)malloc(s_size + 1);
	buf[s_size] = '\0';
	memcpy(buf, s_data, s_size);
	return buf;
}

std::string string::std_str()
{
	return std::string(this->s_data, this->s_size);
}

void string::assign(char* data, size_t size)
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
