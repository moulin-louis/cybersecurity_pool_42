from utils import write_result
from vectors import find_potential_vector, test_vector_potential
from union_attack import union_based_inject
import argparse


result = []


def check_vectors(vectors, base_url):
    for vector in vectors:
        payload = test_vector_potential(base_url, vector, method)
        if payload:
            result.append(f"Vector vulnerable: {vector}")
            result.append(f"Payload Used: {payload}")
            union_based_inject(base_url, vector, payload, result, method)


def detect_sql_injection(url, archive_file):
    if method == "GET":
        vectors, base_url = find_potential_vector(url, method)
        check_vectors(vectors, base_url)
    elif method == "POST":
        vectors, base_url = find_potential_vector(url, method)
        print('- LOG: Found ' + str(len(vectors)) + ' potential vectors')
        # perform_post(vector, base_url)
    else:
        print('- LOG: Wrong method, choose between GET and POST: ', method)
        return
    if len(result) == 0:
        print('- LOG: Failed to extract any data')
        return
    write_result(archive_file, result)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Detect SQL Injection vulnerabilities.')
    parser.add_argument('URL', type=str, help='URL to test against.')
    parser.add_argument('-o', metavar='archive', type=str, help='Archive file for storage.', default="default_archive")
    parser.add_argument('-X', metavar='request', type=str, help='Type of request (GET/POST).', default="GET")
    args = parser.parse_args()
    method = args.X
    detect_sql_injection(args.URL, args.o)
