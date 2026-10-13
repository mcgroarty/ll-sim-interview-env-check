// env_check.cpp - toolchain and networking check.
// Prints compiler and C++ standard info, then opens a TCP listener, accepts
// a single connection, echoes back one line, and exits. Used before the
// interview to confirm the build tools and local networking work.
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>

namespace {

constexpr uint16_t kDefaultPort = 7777;  // TCP port to listen on
constexpr int kListenBacklog = 1;        // only one connection is expected
constexpr size_t kReadChunkSize = 256;   // bytes requested per recv() call
constexpr long kCxx17 = 201703L;         // value of __cplusplus for C++17

// Prints which compiler built this program and which C++ standard it used.
void print_toolchain_info() {
#if defined(__clang__)
    std::cout << "compiler: clang " << __clang_major__ << '.' << __clang_minor__ << '\n';
#elif defined(__GNUC__)
    std::cout << "compiler: gcc " << __GNUC__ << '.' << __GNUC_MINOR__ << '\n';
#else
    std::cout << "compiler: unknown\n";
#endif
    std::cout << "__cplusplus: " << __cplusplus
              << (__cplusplus >= kCxx17 ? " (C++17 or newer)" : " (older than C++17)") << '\n';
    // std::optional is a C++17 library feature; using it proves the library supports it.
    std::optional<std::string> probe = "ok";
    std::cout << "C++17 library: " << *probe << std::endl;
}

// Reads bytes until a newline arrives or the peer closes. Returns what was read.
std::string read_until_newline(int fd) {
    std::string data;
    char chunk[kReadChunkSize];
    while (data.find('\n') == std::string::npos) {
        ssize_t n = ::recv(fd, chunk, sizeof(chunk), 0);
        if (n < 0 && errno == EINTR) continue;  // interrupted by a signal; retry
        if (n <= 0) break;
        data.append(chunk, static_cast<size_t>(n));
    }
    return data.substr(0, data.find('\n') + 1);
}

// Sends all of `data`, looping because send() may write only part of it.
bool send_all(int fd, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = ::send(fd, data.data() + sent, data.size() - sent, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    print_toolchain_info();

    uint16_t port = kDefaultPort;
    if (argc > 1) port = static_cast<uint16_t>(std::atoi(argv[1]));

    // IPv4 TCP socket. SO_REUSEADDR allows re-running immediately on the same port.
    int listener = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) { std::perror("socket"); return EXIT_FAILURE; }
    int enable = 1;
    ::setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    // Listen on localhost only; htons converts the port to network byte order.
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);
    if (::bind(listener, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::perror("bind (is the port already in use?)");
        return EXIT_FAILURE;
    }
    if (::listen(listener, kListenBacklog) < 0) { std::perror("listen"); return EXIT_FAILURE; }
    std::cout << "waiting for one connection on 127.0.0.1:" << port << std::endl;

    int conn = ::accept(listener, nullptr, nullptr);
    if (conn < 0) { std::perror("accept"); return EXIT_FAILURE; }

    std::string line = read_until_newline(conn);
    bool ok = !line.empty() && send_all(conn, line);
    std::cout << (ok ? "echoed: " + line : std::string("no line received\n")) << std::flush;

    ::close(conn);
    ::close(listener);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
