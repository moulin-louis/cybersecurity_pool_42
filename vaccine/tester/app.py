from flask import Flask, request, jsonify, render_template_string
from flaskext.mysql import MySQL

app = Flask(__name__)

# Configure MySQL
app.config['MYSQL_DATABASE_HOST'] = 'mariadb'
app.config['MYSQL_DATABASE_USER'] = 'root'
app.config['MYSQL_DATABASE_PASSWORD'] = 'password'
app.config['MYSQL_DATABASE_DB'] = 'testdb'
app.config['DEBUG'] = True
mysql = MySQL(app)


@app.route('/', methods=['GET'])
def index():
    name = request.args.get('name')
    results = query = error = None
    if name:
        cur = mysql.get_db().cursor()
        query = f"SELECT * FROM users WHERE name = '{name}'"
        try:
            cur.execute(query)
            results = cur.fetchall()
        except Exception as e:
            error = str(e)
    return render_template_string(open("main.html").read(), results=results, query=query, error=error)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
