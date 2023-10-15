from urllib import parse
from bs4 import BeautifulSoup
from utils import perform_requests


possible_payload = ["'", "\"", "`", "\\", "/*'*/", ")'"]
error_messages = ["You have an error in your SQL syntax",
                  "ODBC Microsoft Access Driver",
                  "MySQL syntax error",
                  ": syntax error"]


def find_potential_vector(url, method):
    vectors = []
    parsed_url = parse.urlparse(url)
    params = parsed_url.query[0:parsed_url.query.find('=')]
    if len(params) != 0:
        vectors.append(params)
    base_url = parsed_url.scheme + "://" + parsed_url.netloc + parsed_url.path
    r = perform_requests(base_url, '', method)
    soup = BeautifulSoup(r.text, 'html.parser')
    for form in soup.find_all('form'):
        if not form.has_attr('method') and form['method'] == method:
            continue
        for input_html in form.find_all("input"):
            if input_html.has_attr('name'):
                vectors.append(input_html.attrs['name'])
    vectors = set(vectors)
    for vec in vectors:
        print(f'Vector: {vec}')
    print('LOG: Found ' + str(len(vectors)) + ' potential vectors')
    return vectors, base_url


def test_vector_potential(based_url, vector, method):
    for pld in possible_payload:
        r = perform_requests(based_url, '?' + vector + '=' + pld, method)
        if r.status_code != 200:
            print('LOG: Error request')
            return None
        for error in error_messages:
            if error in r.text:
                print(f'LOG: Error based SQL injection detected!')
                print(f'LOG: payload [{pld}]')
                return pld
    print("LOG: Cannot detect error based injection")
    return None
