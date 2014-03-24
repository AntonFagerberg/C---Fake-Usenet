#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "useserver.h"
#include "protocol.h"
#include "db.h"
#include <iostream>
#include <string>

using namespace std;

UseServer::UseServer(int port, bool saveInMemory) : server(port), db(saveInMemory) {
  if (!server.isReady()) {
    cerr << "[Server error] Initialization error. Shutting down." << endl;
    exit(1);
  }

  listen();
}

void UseServer::write_num(int i, const std::shared_ptr<Connection> conn) {
  conn->write(Protocol::PAR_NUM);
  write_int(i, conn);
}

int UseServer::read_num(const std::shared_ptr<Connection> conn) {
  expect_response(Protocol::PAR_NUM, conn);
  return read_int(conn);
}

void UseServer::write_string(const string s, const std::shared_ptr<Connection> conn) {
  conn->write(Protocol::PAR_STRING);
  write_int(s.size(), conn);

  for (auto c : s) {
    conn->write(c);
  }
}

string UseServer::read_string(const std::shared_ptr<Connection> conn) {
  expect_response(Protocol::PAR_STRING, conn);
  string s;

  for (unsigned int x = read_int(conn); x != 0; --x) {
    s.push_back(conn->read());
  }

  return s;
}

int UseServer::read_int(const std::shared_ptr<Connection> conn) {
  return (conn->read() << 24) | (conn->read() << 16) | (conn->read() << 8) | conn->read();
}

void UseServer::expect_response(unsigned int resp, const std::shared_ptr<Connection> conn) {
  unsigned char c;
  if ((c = conn->read()) != resp) {
    cout << "[Client error] Excpected " << c << " to be " << resp << ". Closing connection." << endl;
    server.deregisterConnection(conn);
  }
}

void UseServer::write_int(int i, const std::shared_ptr<Connection> conn) {
  conn->write((i >> 24) & 0xFF);
  conn->write((i >> 16) & 0xFF);
  conn->write((i >> 8) & 0xFF);
  conn->write(i & 0xFF);
}

void UseServer::com_list_ng(const std::shared_ptr<Connection> conn) {
  expect_response(Protocol::COM_END, conn);

  auto ngs = db.list_ng();
  conn->write(Protocol::ANS_LIST_NG);
  write_num(ngs.size(), conn);

  for (auto const &ng : ngs) {
    write_num(ng.id, conn);
    write_string(ng.name, conn);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::com_create_ng(const std::shared_ptr<Connection> conn) {
  auto name = read_string(conn);
  expect_response(Protocol::COM_END, conn);

  conn->write(Protocol::ANS_CREATE_NG);

  if (db.create_newsgroup(name)) {
    conn->write(Protocol::ANS_ACK);
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write(Protocol::ERR_NG_ALREADY_EXISTS);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::com_delete_ng(const std::shared_ptr<Connection> conn) {
  auto id = read_num(conn);

  conn->write(Protocol::ANS_DELETE_NG);
  if (db.delete_newsgroup(id)) {
    conn->write(Protocol::ANS_ACK);
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::com_list_art(const std::shared_ptr<Connection> conn) {
  auto id = read_num(conn);
  expect_response(Protocol::COM_END, conn);

  conn->write(Protocol::ANS_LIST_ART);

  auto ngs = db.list_ng();
  auto ng = find_if(ngs.cbegin(), ngs.cend(), [id](const Newsgroup &n){ return n.id == id; });

  if (ng != ngs.cend()) {
    conn->write(Protocol::ANS_ACK);
    auto articles = db.list_article(id);
    write_num(articles.size(), conn);

    for (auto a : articles) {
      write_num(a.id, conn);
      write_string(a.title, conn);
    }
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::com_create_art(const std::shared_ptr<Connection> conn) {
  auto id = read_num(conn);
  auto title = read_string(conn);
  auto author = read_string(conn);
  auto text = read_string(conn);
  expect_response(Protocol::COM_END, conn);

  conn->write(Protocol::ANS_CREATE_ART);
  if (db.create_article(title, author, text, id)) {
    conn->write(Protocol::ANS_ACK);
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::com_delete_art(const std::shared_ptr<Connection> conn) {
  auto g_id = read_num(conn);
  auto a_id = read_num(conn);
  expect_response(Protocol::COM_END, conn);

  auto ngs = db.list_ng();
  auto ng = find_if(ngs.cbegin(), ngs.cend(), [g_id](const Newsgroup &n){ return n.id == g_id; });

  auto as = db.list_article(g_id);
  auto a = find_if(as.cbegin(), as.cend(), [a_id](const Article &a) { return a.id == a_id; });

  conn->write(Protocol::ANS_DELETE_ART);
  if (ng != ngs.cend() && a != as.cend() && db.delete_article(a_id)) {
    conn->write(Protocol::ANS_ACK);
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write((ng == ngs.cend()) ? Protocol::ERR_NG_DOES_NOT_EXIST : Protocol::ERR_ART_DOES_NOT_EXIST);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::com_get_art(const std::shared_ptr<Connection> conn) {
  auto g_id = read_num(conn);
  auto a_id = read_num(conn);
  expect_response(Protocol::COM_END, conn);

  auto ngs = db.list_ng();
  auto ng = find_if(ngs.cbegin(), ngs.cend(), [g_id](const Newsgroup &n){ return n.id == g_id; });

  auto as = db.list_article(g_id);
  auto a = find_if(as.cbegin(), as.cend(), [a_id](const Article &a) { return a.id == a_id; });

  conn->write(Protocol::ANS_GET_ART);
  if (ng != ngs.cend() && a != as.cend()) {
    conn->write(Protocol::ANS_ACK);
    write_string(a->title, conn);
    write_string(a->author, conn);
    write_string(a->text, conn);
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write((ng == ngs.cend()) ? Protocol::ERR_NG_DOES_NOT_EXIST : Protocol::ERR_ART_DOES_NOT_EXIST);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::listen() {
  while (true) {
    auto conn = server.waitForActivity();
    if (conn != nullptr) {
      try {
        switch (conn->read()) {
          case Protocol::COM_LIST_NG:
            com_list_ng(conn);
            break;
          case Protocol::COM_CREATE_NG:
            com_create_ng(conn);
            break;
          case Protocol::COM_DELETE_NG:
            com_delete_ng(conn);
            break;
          case Protocol::COM_LIST_ART:
            com_list_art(conn);
            break;
          case Protocol::COM_CREATE_ART:
            com_create_art(conn);
            break;
          case Protocol::COM_DELETE_ART:
            com_delete_art(conn);
            break;
          case Protocol::COM_GET_ART:
            com_get_art(conn);
            break;
          default:
            cout << "[Client error] invalid command. Closing connection." << endl;
            server.deregisterConnection(conn);
            break;
        }

      } catch (ConnectionClosedException&) {
        server.deregisterConnection(conn);
        cout << "[Server info] Client closed connection." << endl;
      }
    } else {
      conn = make_shared<Connection>();
      server.registerConnection(conn);
      cout << "[Server info] New client connected." << endl;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "[Server error] Usage userserver port-number(int) save-in-memory(1/0). Shutting down." << endl;
    exit(1);
  }

  int unsigned port;
  bool saveInMemory;

  try {
    port = stoi(argv[1]);
    saveInMemory = stoi(argv[2]);
  } catch (exception &e) {
    cerr << "[Server error] Unable to parse port number. Shutting down." << endl;
    exit(1);
  }

  UseServer u(port, saveInMemory);
}