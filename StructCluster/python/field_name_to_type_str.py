import os, sys
sys.path.append(os.getcwd())
import json
from Scripts.mysql_util import *

def field_name_to_type_str():
    db, cursor = connect_db()
    sql_str = "select struct_id, member_name, type_str from struct_info_member"
    all_fields = raw_query(cursor, sql_str)
    print(all_fields.__len__())
    print(type(all_fields))
    res = {}
    for field in all_fields:
        member_name = field[1]
        type_str = field[2]
        assert type_str != "total"
        if member_name not in res:
            res[member_name] = {"total": 0}
        if type_str not in res[member_name]:
            res[member_name][type_str] = 1
            res[member_name]["total"] += 1
        else:
            res[member_name][type_str] += 1
            res[member_name]["total"] += 1
    json.dump(res, open("./field_name_to_type_str.json", "w"))
    

def filter_result(curr_result: str="field_name_to_type_str.json"):
    d = json.load(open(curr_result, "r"))
    for field_name, statistics in d.items():
        if statistics.__len__() == 2:
            d[field_name] = None
        # if statistics["total"] <= 100:
        #     d[field_name] = None

    d = {k: v for k, v in d.items() if v != None}
    print(d.__len__())
    json.dump(d, open("./field_name_to_type_str.filter.json", "w"), indent=4)

def only_member_name(curr_result: str="field_name_to_type_str.filter.lessthan100.json"):
    d = json.load(open(curr_result, "r"))
    res = ""
    for k in d:
        res += k + ","
    res = res[1:-1]
    return res
        

if __name__ == "__main__":
    # field_name_to_type_str()
    # filter_result()
    print(only_member_name())