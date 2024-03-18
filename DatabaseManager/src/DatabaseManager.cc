#include <DatabaseManager.hh>

namespace mysql {

MYSQL* mysql_connect(string ip_address, string user_name, string password, string db_name, int port, string charset) {
    MYSQL* mysql = new MYSQL();
    mysql_init(mysql);
    mysql_set_character_set(mysql, charset.c_str());
    if (mysql_real_connect(
        mysql, 
        ip_address.c_str(), 
        user_name.c_str(), 
        password.c_str(), 
        db_name.c_str(), 
        (unsigned int)port, 
        NULL, 
        0)) {
		return mysql;
    } else {
        cerr << "Connecting to Mysql " << ip_address << ":" << port << " error: " << mysql_error(mysql) << endl;
        return nullptr;
	}
}

MYSQL* mysql_connect(string config_file) {
    ifstream config_f(config_file);
    nlohmann::json j_config;
    if (!config_f.good()) { return nullptr; }
    config_f >> j_config;
    return mysql_connect(
        j_config["ip_address"],
        j_config["user_name"],
        j_config["password"],
        j_config["db_name"],
        j_config["port"]
    );
}

bool mysql_raw_query(MYSQL* mysql, const string& raw_query) {
    if (mysql_query(mysql, raw_query.c_str())) {
        cerr << "Mysql query error: " << raw_query << "| " << mysql_error(mysql) << endl;
        return false;
    }
    return true;
}

string __mysql_expend_columns(const vector<string>& vec) {
    if (vec.empty()) return "";
    string res = "(";
    for (string s : vec) res += (s + ", ");
    res.erase(res.end() - 2, res.end());
    res += ")";
    return res;
}

string __mysql_expend_values(const vector<string>& vec) {
    if (vec.empty()) return "";
    string res = "(";
    for (string s : vec) res += ("\'" + s + "\'" + ", ");
    res.erase(res.end() - 2, res.end());
    res += ")";
    return res;
}

bool mysql_create_table(MYSQL* mysql, string table_name, vector<string>& columns, vector<string>& pks, string tail) {
    vector<string> t_info(columns);
    t_info.insert(t_info.end(), pks.begin(), pks.end());
    string query = util::Format("CREATE TABLE {0} {1} {2};", table_name, __mysql_expend_columns(t_info), tail);
    return mysql_raw_query(mysql, query);
}

bool mysql_drop_table(MYSQL* mysql, string table_name) {
    string query = util::Format("DROP TABLE IF EXISTS {0};", table_name);
    return mysql_raw_query(mysql, query);
}

bool mysql_insert(MYSQL* mysql, string table, vector<string>& values) {
    string query = util::Format("insert into {0} values {1};", 
        table, __mysql_expend_values(values));
    return mysql_raw_query(mysql, query);
}

bool mysql_insert(MYSQL* mysql, string table, vector<string>& columns, vector<string>& values) {
    string query = util::Format("insert into {0} {1} values {2};", 
        table, __mysql_expend_columns(columns), __mysql_expend_values(values));
    return mysql_raw_query(mysql, query);
}

/* the raw_res will be freed after success. */
MysqlResult* __raw_res_to_mysql_res(MYSQL* mysql, MYSQL_RES* raw_res, string table, bool map_result) {
    if (raw_res == nullptr) throw; // should never be triggered;
    MysqlResult* res = new MysqlResult();
    if (map_result) {
        res->is_map_result = true;
        while (MYSQL_FIELD* field = mysql_fetch_fields(raw_res)) {
            res->headers.push_back(string(field->name));
        }

        int row_count = 0;
        while (MYSQL_ROW row = mysql_fetch_row(raw_res)) {
            map<string, string> *temp_map = new map<string, string>();
            for (int i = 0; i < res->headers.size(); i++) {
                (*temp_map)[res->headers[i]] = row[i];
            }
            res->data.push_back(*temp_map);
            row_count++;
        }    
    } else {
        res->is_row_result = true;
        int row_count = 0;
        unsigned int num_fields = mysql_num_fields(raw_res);
        while (MYSQL_ROW row = mysql_fetch_row(raw_res)) {
            vector<string>* r = new vector<string>();
            for (size_t i = 0; i < num_fields; i++) {
                r->push_back(row[i]);
            }
            res->rows.push_back(*r);
            row_count++;
        }        
    }
    mysql_free_result(raw_res);
    return res;
}


MysqlResult* mysql_select(MYSQL* mysql, string table, string limitation, bool map_result) {
    MysqlResult* res = new MysqlResult();
    string query = util::Format("select * from {0} {1};", table, limitation); 
    if (mysql_raw_query(mysql, query.c_str())) {
        MYSQL_RES* raw_res = mysql_store_result(mysql);
        return __raw_res_to_mysql_res(mysql, raw_res, table, map_result);
    } else {
        return nullptr;
    }
}

MysqlResult* mysql_select(MYSQL* mysql, string table, vector<string>& columns, string limitation, bool map_result) {
    MysqlResult* res = new MysqlResult();
    string selected_columns = __mysql_expend_columns(columns);
    selected_columns = string(selected_columns.begin() + 1, selected_columns.end() - 1);
    string query = util::Format("select {0} from {1} {2};", selected_columns, table, limitation); 
    if (mysql_raw_query(mysql, query.c_str())) {
        MYSQL_RES* raw_res = mysql_store_result(mysql);
        return __raw_res_to_mysql_res(mysql, raw_res, table, map_result);
    } else {
        return nullptr;
    }
}


}
