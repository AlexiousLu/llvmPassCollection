# import jieba.analyse
import os, sys
sys.path.append(os.getcwd())

import nltk
# nltk.download('averaged_perceptron_tagger', download_dir="/data/nltk_wordset")
from Scripts.mysql_util import *
from collections import defaultdict

statistics = defaultdict(int)

def struct_name_preprocessing(struct_name: str):
    return struct_name.replace("_", " ")


def struct_name_separate(struct_names: list, of):
    for sn in struct_names:
        _struct_name_separate(struct_name_preprocessing(sn[0]), of)

def _struct_name_separate(struct_name: str, of):
    # jieba.analyse.extract_tags(struct_name)
    tokens = nltk.word_tokenize(struct_name)
    pos_tags = nltk.pos_tag(tokens)
    for tag in pos_tags:
        statistics[str(tag)] += 1
    of.write(f"{struct_name:<30} -> {pos_tags}\n")
    
def member_name_stat(member_name_tuple: tuple):
    pass

if __name__ == "__main__":
    db = connect_db()
    cursor = db.cursor()

    # struct_name
    # rows = raw_query(cursor, "select struct_name from struct_info_new where struct_name != '' group by struct_name")
    # with open("struct_name_temp.txt", "w") as of:
    #     struct_name_separate(rows, of)
    # json.dump(statistics, open("struct_name_temp.stat", "w"))

    rows = raw_query(cursor, "select member_name, from struct_info_new where member_name != '' group by member_name")
    with open("member_name_temp.txt", "w") as of:
        struct_name_separate(rows, of)
    json.dump(statistics, open("member_name_temp.stat", "w"))    
