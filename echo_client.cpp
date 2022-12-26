/**
 * @file echo_client.cpp
 * @author Yukun J
 * @expectation this is the demo echo client for illustration and test purpose
 * @init_date Dec 26 2022
 */

#include <memory>

#include "connection.h"
#include "looper.h"
#include "net_address.h"
#include "poller.h"
#include "socket.h"
#include "thread_pool.h"

#define BUF_SIZE 2048

namespace TURTLE_SERVER {
class EchoClient {
 public:
  explicit EchoClient(NetAddress server_address) {
    auto client_socket = std::make_unique<Socket>();
    client_socket->Connect(server_address);
    client_connection = std::make_unique<Connection>(nullptr, std::move(client_socket));
  }

  void Begin() {
    char buf[BUF_SIZE + 1];
    memset(buf, 0, sizeof(buf));
    int fd = client_connection->GetFd();
    while (true) {
      // scan from user keyboard
      auto actual_read = read(STDIN_FILENO, buf, BUF_SIZE);
      send(fd, buf, actual_read, 0);
      memset(buf, 0, sizeof(buf));
      // echo back to screen from server's message
      auto actual_recv = recv(fd, buf, BUF_SIZE, 0);
      write(STDOUT_FILENO, buf, actual_recv);
      memset(buf, 0, sizeof(buf));
    }
  }

 private:
  std::unique_ptr<Connection> client_connection;
};
}  // namespace TURTLE_SERVER

int main() {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  TURTLE_SERVER::EchoClient echo_client(local_address);
  echo_client.Begin();

  return 0;
}