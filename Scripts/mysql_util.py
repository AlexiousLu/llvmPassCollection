import pymysql
import json

from typing import Tuple
from pymysql.cursors import Cursor


def connect_db(config_path: str="Config/mysql.conf") -> Tuple[pymysql.Connection, Cursor]:
    config = json.load(open(config_path))
    db = pymysql.connect(
        host=config["ip_address"], 
        port=config["port"],
        user=config["user_name"],
        password=config["password"], 
        database=config["db_name"], 
        charset="utf8", 
    )
    cursor = db.cursor()
    return db, cursor

def close_db(db: pymysql.Connection, cursor: Cursor):
    cursor.close()
    db.close()


def raw_query(cursor: Cursor, query: str):
    cursor.execute(query)
    return cursor.fetchall()

