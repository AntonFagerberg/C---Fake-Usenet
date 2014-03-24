#include "useclient.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include <iostream>
#include <string>

using namespace std;

UseClient::UseClient(const char *host, int port) : conn(host, port) {
  if (!conn.isConnected()) {
    cerr << "Unable to connect." << endl;
    exit(1);
  }

  run();
}

void UseClient::write_num(int i) {
  conn.write(Protocol::PAR_NUM);
  write_int(i);
}

int UseClient::read_num() {
  expect_response(Protocol::PAR_NUM);
  return read_int();
}

void UseClient::write_string(const string s) {
  conn.write(Protocol::PAR_STRING);
  write_int(s.size());

  for (auto c : s) {
    conn.write(c);
  }
}

string UseClient::read_string() {
  expect_response(Protocol::PAR_STRING);
  string s;

  for (unsigned int x = read_int(); x != 0; --x) {
    s.push_back(conn.read());
  }

  return s;
}

int UseClient::read_int() {
  return (conn.read() << 24) | (conn.read() << 16) | (conn.read() << 8) | conn.read();
}

void UseClient::expect_response(unsigned int resp) {
  unsigned char c;
  if ((c = conn.read()) != resp) {
    cout << "[Server error] Excpected " << c << " to be " << resp << ". Closing connection." << endl;
    exit(1);
  }
}

void UseClient::write_int(int i) {
  conn.write((i >> 24) & 0xFF);
  conn.write((i >> 16) & 0xFF);
  conn.write((i >> 8) & 0xFF);
  conn.write(i & 0xFF);
}

void UseClient::com_list_ng() {
  conn.write(Protocol::COM_LIST_NG);
  conn.write(Protocol::COM_END);

  expect_response(Protocol::ANS_LIST_NG);

  for (auto nbr = read_num(); nbr != 0; --nbr) {
    cout << "[" << read_num() << "] " << read_string() << endl;
  }

  expect_response(Protocol::ANS_END);
}

void UseClient::com_create_ng() {
  string name;
  cout << "Name> ";
  cin >> name;

  conn.write(Protocol::COM_CREATE_NG);
  write_string(name);
  conn.write(Protocol::COM_END);

  expect_response(Protocol::ANS_CREATE_NG);
  
  switch (conn.read()) {
    case Protocol::ANS_ACK:
      cout << "Group created." << endl;
      break;
    case Protocol::ANS_NAK:
      expect_response(Protocol::ERR_NG_ALREADY_EXISTS);
      cout << "Group name already exists." << endl;
      break;
    default:
      cerr << "Invalid response. Closing connection." << endl;
      exit(1);
      break;
  }

  expect_response(Protocol::ANS_END);
}

void UseClient::com_delete_ng() {
  int id;
  cout << "Id-number> ";
  cin >> id;

  conn.write(Protocol::COM_DELETE_NG);
  write_num(id);
  conn.write(Protocol::COM_END);

  expect_response(Protocol::ANS_DELETE_NG);
  switch (conn.read()) {
    case Protocol::ANS_ACK:
      cout << "Group deleted." << endl;
      break;
    case Protocol::ANS_NAK:
      expect_response(Protocol::ERR_NG_DOES_NOT_EXIST);
      cout << "Invalid group id." << endl;
      break;
    default:
      cerr << "Invalid response. Closing connection." << endl;
      exit(1);
      break;
  }
  expect_response(Protocol::ANS_END);
}

void UseClient::com_list_art() {
  int id;
  cout << "Id-number> ";
  cin >> id;

  conn.write(Protocol::COM_LIST_ART);
  write_num(id);
  conn.write(Protocol::COM_END);

  expect_response(Protocol::ANS_LIST_ART);
  switch (conn.read()) {
    case Protocol::ANS_ACK:
      for (auto i = read_num(); i != 0; --i) {
        cout << "[" << read_num() << "] " << read_string() << endl;
      }
      break;
    case Protocol::ANS_NAK:
      expect_response(Protocol::ERR_NG_DOES_NOT_EXIST);
      cout << "Invalid group id." << endl;
      break;
    default:
      cerr << "Invalid response. Closing connection." << endl;
      exit(1);
      break;
  }

  expect_response(Protocol::ANS_END);
}

void UseClient::com_create_art() {
  int groupId;
  string title, author, text, line;

  cout << "Group id> ";
  cin >> groupId;
  cin.ignore(256, '\n');
  cout << "Title> ";
  getline(cin, title);
  cout << "Author> ";
  getline(cin, author);
  cout << "Text (type '$' on a new line to end)>" << endl;
  while (line != "$") {
    text.append(line);
    text.push_back('\n');
    getline(cin, line);
  }

  conn.write(Protocol::COM_CREATE_ART);
  write_num(groupId);
  write_string(title);
  write_string(author);
  write_string(text);
  conn.write(Protocol::COM_END);

  expect_response(Protocol::ANS_CREATE_ART);
  switch (conn.read()) {
    case Protocol::ANS_ACK:
      cout << "Article created." << endl;
      break;
    case Protocol::ANS_NAK:
      expect_response(Protocol::ERR_NG_DOES_NOT_EXIST);
      cout << "Not saved - invalid group id." << endl;
      break;
    default:
      cerr << "Invalid response. Closing connection." << endl;
      exit(1);
      break;
  }
  expect_response(Protocol::ANS_END);
}

void UseClient::com_delete_art() {
  int groupId, articleId;
  cout << "Group id> ";
  cin >> groupId;
  cout << "Article id> ";
  cin >> articleId;

  conn.write(Protocol::COM_DELETE_ART);
  write_num(groupId);
  write_num(articleId);
  conn.write(Protocol::COM_END);

  expect_response(Protocol::ANS_DELETE_ART);

  switch (conn.read()) {
    case Protocol::ANS_ACK:
      cout << "Article deleted." << endl;
      break;
    case Protocol::ANS_NAK:
      switch (conn.read()) {
        case Protocol::ERR_NG_DOES_NOT_EXIST:
          cout << "Invalid group id." << endl;
          break;
        case Protocol::ERR_ART_DOES_NOT_EXIST:
          cout << "Invalid article id." << endl;
          break;
        default:
          cerr << "Invalid response. Closing connection." << endl;
          exit(1);
          break;
      }
      break;
    default:
      cerr << "Invalid response. Closing connection." << endl;
      exit(1);
      break;
  }

  expect_response(Protocol::ANS_END);
}

void UseClient::com_get_art() {
  int groupId, articleId;
  cout << "Group id> ";
  cin >> groupId;
  cout << "Article id> ";
  cin >> articleId;

  conn.write(Protocol::COM_GET_ART);
  write_num(groupId);
  write_num(articleId);
  conn.write(Protocol::COM_END);

  expect_response(Protocol::ANS_GET_ART);
  switch (conn.read()) {
    case Protocol::ANS_ACK:
      cout << "Title: " << read_string() << endl;
      cout << "Author: " << read_string() << endl;
      cout << "Message:" << endl << read_string() << endl;
      break;
    case Protocol::ANS_NAK:
      switch (conn.read()) {
        case Protocol::ERR_NG_DOES_NOT_EXIST:
          cout << "Invalid group id." << endl;
          break;
        case Protocol::ERR_ART_DOES_NOT_EXIST:
          cout << "Invalid article id." << endl;
          break;
        default:
          cerr << "Invalid response. Closing connection." << endl;
          exit(1);
          break;
      }
      break;
    default:
      cerr << "Invalid response. Closing connection." << endl;
      exit(1);
      break;
  }

  expect_response(Protocol::ANS_END);
}

void UseClient::run() {
  string cmd;

  for (auto const h : help) {
    cout << h << endl;
  }

  while (true) {
    cout << "Command (or 'help')> ";
    cin >> cmd;

    if (cmd == "help") {
      for (auto const h : help) {
        cout << h << endl;
      }
    } else {
      try {
        switch (stoi(cmd)) {
          case 0:
            cout << "Good bye!" << endl;
            exit(0);
          case 1:
            com_list_ng();
            break;
          case 2:
            com_create_ng();
            break;
          case 3:
            com_delete_ng();
            break;
          case 4:
            com_list_art();
            break;
          case 5:
            com_create_art();
            break;
          case 6:
            com_delete_art();
            break;
          case 7:
            com_get_art();
        }
      } catch (invalid_argument) {
        cout << "Invalid argument: " << cmd << ". Type 'help' for commands." << endl;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "[Client error] Usage 'userclient host-address(string) port-number(int)'. Shutting down." << endl;
    exit(1);
  }

  int port;

  try {
    port = stoi(argv[2]);
  } catch (exception &e) {
    cerr << "[Client error] Unable to parse port number. Shutting down." << endl;
    exit(1);
  }

  UseClient c(argv[1], port);
}