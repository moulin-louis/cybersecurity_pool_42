import requests
import argparse
import sys


def perform_get_test(url):
    try:
        r = requests.get(url + '?id=1\' --\'')
        print('Status code:', r.status_code)

        if r.status_code != 200:
            print('Error request')
            return

        # Check for common SQL error messages
        error_messages = ["You have an error in your SQL syntax",
                          "ODBC Microsoft Access Driver",
                          "MySQL syntax error"]

        for error in error_messages:
            if error in r.text:
                print(f'Potential SQL injection found! Message: {error}')
                return

        # For debugging:
        print('Headers:', r.headers)
        print('Body:', r.text[:500])  # Print only the first 500 characters to avoid clutter

    except requests.RequestException as e:
        print(f"Request error: {e}")


def detect_sql_injection(url, method, archive_file):
    # Your SQL injection detection logic here.
    if method == "POST":
        pass
        # perform_post_test(url)
    elif method == "GET":
        perform_get_test(url)
    else:
        print("WRONG METHOD!!")
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description='Detect SQL Injection vulnerabilities.')
    parser.add_argument('URL', type=str, help='URL to test against.')
    parser.add_argument('-o', metavar='archive', type=str, help='Archive file for storage.',
                        default="default_archive")
    parser.add_argument('-X', metavar='request', type=str, help='Type of request (GET/POST).', default="GET")

    args = parser.parse_args()

    detect_sql_injection(args.URL, args.X, args.o)


if __name__ == "__main__":
    main()
