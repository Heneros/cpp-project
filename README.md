
# Tcp server on C++

A modern, high-performance multithreaded TCP server written in C++ with epoll-based event handling, IPv4/IPv6 support, static file serving, and TLS encryption.




###  Planned for Future

- **AES message decryption** — End-to-end encryption for message payloads
- **Message logging system** — Comprehensive logging for monitoring and debugging
- **Google Test integration** — Unit and integration test suite
- **Client management API** — `deleteClient()` functionality for runtime client removal
- **IP-based client banning** — Security feature to block malicious clients
- **10,000+ concurrent connections** — Scalable architecture supporting massive client loads
- **Epoll + Thread Pool** — Efficient event-driven I/O with worker thread pool for optimal performance
- **Static file serving** — Serves files from directory with automatic MIME type detection
- **Directory traversal protection** — Security measures to prevent path traversal attacks
- **Browser-accessible static pages** — Serve HTML, CSS, JavaScript, images, and other static content
- **OpenSSL integration** — TLS/SSL support for secure encrypted connections




## Implemented

- **Docker** - Launch through 
- **Auto-detection of IPv4/IPv6** — Automatically determines and handles both address families
- **IPv6 support with sockaddr_storage** — Unified address handling for seamless IPv4/IPv6 compatibility


---

##  Build and Launch

### Prerequisites
- Linux (required for `epoll`)
- C++20 compatible compiler (GCC 11+ or Clang 14+)
- CMake
- OpenSSL

**Build project** `./build.sh`

**Start Server** `./build/tcp_server`

**Start Client** `./build/tcp_client`
