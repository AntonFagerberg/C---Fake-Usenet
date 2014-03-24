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

void UseClient::run() {
  string cmd;

  for (auto const h : help) {
    cout << h << endl;
  }

  while (true) {
    cout << "Command> ";
    cin >> cmd;

    if (cmd == "help") {
      for (auto const h : help) {
        cout << h << endl;
      }
    } else {
      try {
        switch (stoi(cmd)) {
          case 1:
            com_list_ng();
            break;
          default:
            cout << "???" << endl;
            break;
        }
      } catch (invalid_argument) {
        cout << "Invalid argument: " << cmd << ". Type 'help' for commands." << endl;
      }
    }
  }
}

int main() {
  //string addr = "localhost"
  UseClient c("localhost", 1234);
}