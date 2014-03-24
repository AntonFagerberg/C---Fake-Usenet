#ifndef USE_CLIENT_H
#define USE_CLIENT_H

#include <string>
#include <vector>
#include "connection.h"

class UseClient {
public:
  UseClient(const char*, int);
private:
  void run();
  void com_list_ng();
  void com_create_ng();
  void com_delete_ng();
  void com_list_art();
  void com_create_art();
  void com_delete_art();
  void com_get_art();

  void expect_response(unsigned int);

  std::string read_string();
  void write_string(const std::string);

  void write_num(int);
  int read_num();

  void write_int(int);
  int read_int();

  std::vector<std::string> help = {
    "1) - List groups",
    "2) - Create group",
    "3) - Delete group",
    "4) - List articles",
    "5) - Create article",
    "6) - Delete article",
    "7) - Get article",
    "0) - Exit"
  };

  Connection conn;
};

#endif