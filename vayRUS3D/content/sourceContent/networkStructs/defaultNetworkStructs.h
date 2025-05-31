#pragma once

#include<../EngineCore.h>
#include<string>

using namespace UreTechEngine;

typedef char func_name[20];
typedef char ent_name[30];

#define network_max_func_per_tick 24

// func_rep_Test definiton
struct func_rep_Test_input_struct {
	int test_val0 = 2;
	std::string test = "im test";
};


struct networkReplicationStruct
{
public:
	bool isServer = false;
	func_name func_notify[network_max_func_per_tick];//only network_max_func_per_tick functions on tick
	unsigned int func_notify_count = 0;

	ent_name func_notify_to_ent[network_max_func_per_tick];//only network_max_func_per_tick entities on tick
	unsigned int func_notify_to_ent_count = 0;
	// func_rep_Test input data
	func_rep_Test_input_struct func_test_input;


	networkReplicationStruct();

	void run_noticed_funcs();
};