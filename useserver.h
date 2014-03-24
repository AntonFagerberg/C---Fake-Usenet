#ifndef USE_SERVER_H
#define USE_SERVER_H

#include "connection.h"
#include "server.h"
#include "db.h"

class UseServer {
public:
  UseServer(int, bool);
private:
  void listen();
  void com_list_ng(const std::shared_ptr<Connection>);
  void com_create_ng(const std::shared_ptr<Connection>);
  void com_delete_ng(const std::shared_ptr<Connection>);
  void com_list_art(const std::shared_ptr<Connection>);
  void com_create_art(const std::shared_ptr<Connection>);
  void com_delete_art(const std::shared_ptr<Connection>);
  void com_get_art(const std::shared_ptr<Connection>);

  void expect_response(unsigned int, const std::shared_ptr<Connection>);

  std::string read_string(const std::shared_ptr<Connection>);
  void write_string(const std::string, const std::shared_ptr<Connection>);

  void write_num(int, const std::shared_ptr<Connection>);
  int read_num(const std::shared_ptr<Connection>);

  void write_int(int, const std::shared_ptr<Connection>);
  int read_int(const std::shared_ptr<Connection>);

  Server server;
  Db db;
};

#endif