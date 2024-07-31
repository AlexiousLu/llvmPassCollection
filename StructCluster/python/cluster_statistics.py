import json


if __name__ == "__main__":
    # stat = json.load(open("struct_name_temp.stat"))
    # stat = sorted(stat.items(), key=lambda x: x[1], reverse=True)
    # json.dump(stat, open("struct_name_temp.sort", "w"), indent=4)
    
    stat = json.load(open("member_name_temp.stat"))
    stat = sorted(stat.items(), key=lambda x: x[1], reverse=True)
    json.dump(stat, open("member_name_temp.sort", "w"), indent=4)
