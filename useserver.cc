#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "useserver.h"
#include "protocol.h"
#include "db.h"
#include <iostream>
#include <string>

using namespace std;

UseServer::UseServer(int port) : server(port) {
  if (!server.isReady()) {
    cerr << "Server initialization error." << endl;
    exit(1);
  }

  listen();
}

void UseServer::writeNum(unsigned int i, const std::shared_ptr<Connection> conn) {
  conn->write(Protocol::PAR_NUM);
  writeInt(i, conn);
}

unsigned int UseServer::readNum(const std::shared_ptr<Connection> conn) {
  expectResponse(Protocol::PAR_NUM, conn);
  return readInt(conn);
}

void UseServer::writeString(const string s, const std::shared_ptr<Connection> conn) {
  conn->write(Protocol::PAR_STRING);
  writeInt(s.size(), conn);
  for (auto c : s) {
    conn->write(c);
  }
}

string UseServer::readString(const std::shared_ptr<Connection> conn) {
  expectResponse(Protocol::PAR_STRING, conn);
  string s;

  for (unsigned int x = readInt(conn); x != 0; --x) {
    s.push_back(conn->read());
  }

  return s;
}

unsigned int UseServer::readInt(const std::shared_ptr<Connection> conn) {
  return (conn->read() << 24) | (conn->read() << 16) | (conn->read() << 8) | conn->read();
}

void UseServer::expectResponse(unsigned int prot, const std::shared_ptr<Connection> conn) {
  unsigned char c;
  if ((c = conn->read()) != prot) {
    cerr << "Excpected " << c << " to be " << prot << "." << endl;
    server.deregisterConnection(conn);
  }
}

void UseServer::writeInt(int i, const std::shared_ptr<Connection> conn) {
  conn->write((i >> 24) & 0xFF);
  conn->write((i >> 16) & 0xFF);
  conn->write((i >> 8) & 0xFF);
  conn->write(i & 0xFF);
}

void UseServer::comListNg(const std::shared_ptr<Connection> conn) {
  expectResponse(Protocol::COM_END, conn);

  auto newsGroups = db.list_ng();
  conn->write(Protocol::ANS_LIST_NG);
  writeNum(newsGroups.size(), conn);

  for (auto const &n : newsGroups) {
    writeNum(n.id, conn);
    writeString(n.name, conn);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::comCreateNg(const std::shared_ptr<Connection> conn) {
  auto name = readString(conn);
  expectResponse(Protocol::COM_END, conn);

  conn->write(Protocol::ANS_CREATE_NG);

  if (db.create_newsgroup(name)) {
    conn->write(Protocol::ANS_ACK);
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write(Protocol::ERR_NG_ALREADY_EXISTS);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::comDeleteNg(const std::shared_ptr<Connection> conn) {
  auto idNum = readNum(conn);

  conn->write(Protocol::ANS_DELETE_NG);
  if (db.delete_newsgroup(idNum)) {
    conn->write(Protocol::ANS_ACK);
  } else {
    conn->write(Protocol::ANS_NAK);
    conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
  }

  conn->write(Protocol::ANS_END);
}

void UseServer::comListArt(const std::shared_ptr<Connection> conn) {
  expectResponse(Protocol::PAR_NUM, conn);
  auto idNum = readInt(conn);
  expectResponse(Protocol::COM_END, conn);

  conn->write(Protocol::ANS_LIST_ART);
  // Articles or
  conn->write(Protocol::ANS_NAK);
  conn->write(Protocol::ERR_NG_DOES_NOT_EXIST);
  conn->write(Protocol::ANS_END);
}

void UseServer::comCreateArt(const std::shared_ptr<Connection> conn) {
  expectResponse(Protocol::PAR_NUM, conn);
  auto idNum = readInt(conn);
  auto title = readString(conn);
  auto author = readString(conn);
  auto text = readString(conn);
  expectResponse(Protocol::COM_END, conn);

  conn->write(Protocol::ANS_CREATE_ART);
  conn->write(Protocol::ANS_ACK);
  conn->write(Protocol::ANS_END);
}

void UseServer::listen() {
  while (true) {
    auto conn = server.waitForActivity();
    if (conn != nullptr) {
      try {
        switch (conn->read()) {
          case Protocol::COM_LIST_NG:
            comListNg(conn);
            break;
          case Protocol::COM_CREATE_NG:
            comCreateNg(conn);
            break;
          case Protocol::COM_DELETE_NG:
            comDeleteNg(conn);
            break;
          case Protocol::COM_LIST_ART:
            comListArt(conn);
            break;
          case Protocol::COM_CREATE_ART:
            comCreateArt(conn);
            break;
        }

      } catch (ConnectionClosedException&) {
        server.deregisterConnection(conn);
        cout << "Client closed connection." << endl;
      }
    } else {
      conn = make_shared<Connection>();
      server.registerConnection(conn);
      cout << "New client connected." << endl;
    }
  }
}

int main() {
  UseServer u(1234);
}