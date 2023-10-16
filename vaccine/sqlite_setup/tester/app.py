from flask import Flask, request, jsonify, render_template_string
import sqlite3


app = Flask(__name__)

sqlite_db = sqlite3.connect('mydatabase.db', check_same_thread=False)


@app.route('/', methods=['GET'])
def index():
    name = request.args.get('name')
    results = query = error = None
    if name:
        cur = sqlite_db.cursor()
        query = f"SELECT * FROM users WHERE name = '{name}'"
        try:
            cur.execute(query)
            results = cur.fetchall()
        except Exception as e:
            error = str(e)
    return render_template_string(open("main.html").read(), results=results, query=query, error=error)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
