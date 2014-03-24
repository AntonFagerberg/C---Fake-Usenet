#ifndef USE_SERVER_H
#define USE_SERVER_H

#include "protocol.h"
#include "server.h"
#include "connection.h"
#include "db.h"

class UseServer {
public:
  UseServer(int port);
  void listen();
private:
  void comListNg(const std::shared_ptr<Connection>);
  void comCreateNg(const std::shared_ptr<Connection>);
  void comDeleteNg(const std::shared_ptr<Connection>);
  void comListArt(const std::shared_ptr<Connection>);
  void comCreateArt(const std::shared_ptr<Connection>);

  void expectResponse(unsigned int, const std::shared_ptr<Connection>);

  std::string readString(const std::shared_ptr<Connection>);
  void writeString(const std::string, const std::shared_ptr<Connection>);

  void writeNum(unsigned int, const std::shared_ptr<Connection>);
  unsigned int readNum(const std::shared_ptr<Connection>);

  void writeInt(int, const std::shared_ptr<Connection>);
  unsigned int readInt(const std::shared_ptr<Connection>);
  Server server;
  Db db;
};

#endif