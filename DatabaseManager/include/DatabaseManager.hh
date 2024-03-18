#pragma once
#include <mysql/mysql.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <format.hh>
#include <json.hpp>

using namespace std;

namespace mysql {
class MysqlResult {
public:
    vector<string> headers;
    bool is_row_result;
    bool is_map_result;
    vector<vector<string>> rows;
    vector<map<string, string>> data;
};

MYSQL* mysql_connect(string ip_address, string user_name, string password, string db_name, int port=3306, string charset="utf8");

MYSQL* mysql_connect(string config_file);

bool mysql_raw_query(MYSQL* mysql, const string& raw_query);

bool mysql_create_table(MYSQL* mysql, string table_name, vector<string>& columns, vector<string>& pks, string tail);

bool mysql_drop_table(MYSQL* mysql, string table_name);

vector<string> mysql_query_header(MYSQL* mysql, string table);

bool mysql_insert(MYSQL* mysql, string table, vector<string>& values);

bool mysql_insert(MYSQL* mysql, string table, vector<string>& columns, vector<string>& values);

MysqlResult* mysql_select(MYSQL* mysql, string table, string limitation="", bool map_result=false);

MysqlResult* mysql_select(MYSQL* mysql, string table, vector<string>& columns, string limitation="", bool map_result=false);


}