import socket
import json


def print_response(response):
    print(f'Status code: {response[-1]["status"]}')
    if response[-1]['status'] == 500:
        print(f"Something went wrong!\nServer message: {response[0]['message']}")
    elif response[-1]['status'] == 200:
        print("All ok!\n")
        if 'message' in response[0]:
            print(f"Server message: {response[0]['message']}")
        if len(response[0]) > 2:
            print("Response content:")
            for res in response:
                for key, value in res.items():
                    if key not in ['status', 'message']:
                        print(f'{key}: {value}')
                print()


def send_request(action, data):
    request = json.dumps({'action': action, 'data': data})
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect(('127.0.0.1', 8888))
        sock.sendall(request.encode())
        response = json.loads(sock.recv(4096).decode())
        if action in ['search', 'view']:
            response = response[0]
        print_response(response)


def main():
    print("Phonebook Application")
    while True:
        print("\n1. Add Entry")
        print("2. Delete Entry")
        print("3. Search Entry")
        print("4. View All Entries")
        print("5. Quit")
        choice = input("Choose an option: ")

        if choice == '1':
            data = {
                'name': input("Enter name: "),
                'lastname': input("Enter lastname: "),
                'patronymic': input("Enter patronymic: "),
                'phone': input("Enter phone: "),
                'note': input("Enter note: ")
            }
            send_request('add', data)
        elif choice == '2':
            data = {'phone': input("Enter phone to delete: ")}
            send_request('delete', data)
        elif choice == '3':
            field = input("Enter field to search by (name, lastname, patronymic, phone): ")
            query = input("Enter search query: ")
            data = {
                'field': field,
                'query': query,
            }
            send_request('search', data)
        elif choice == '4':
            send_request('view', {})
        elif choice == '5':
            break
        else:
            print("Invalid choice")


if __name__ == '__main__':
    main()
