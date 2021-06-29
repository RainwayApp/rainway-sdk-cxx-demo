#include "runtime/interface.hpp"
#include <iostream>
#include <optional>
#include <sstream>

/// Template that allows combining multiple lambdas or other callables with different arguments
/// into one callable struct that performs operator overloading. Useful for std::visit.
template<class... Ts>
struct Overload : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
Overload(Ts...) -> Overload<Ts...>;

// Helper to pretty print a packed version
static auto prettyVersion(uint32_t packed)
{
    return (std::stringstream{}
        << ((packed & 0xff000000) >> 24) << "."
        << ((packed & 0x00ff0000) >> 16) << "."
        << ((packed & 0x0000ff00) >> 8))
        .str();
}

int main() {
    using namespace Rainway;
    // Create a config for the runtime
    auto config = Config{
        // Your publishable API key should go here
        "",
        "C++ sandbox", // External Id
        0, // Host port (0 for random)
    };
    // Assign some callbacks for handling some functions
    config.logSink = [&](auto level, const auto& str) { printf("[RW] %s\n", str.c_str()); };
    // Accept all connection requests
    config.onConnectionRequest = [](ConnectionRequest request) { request.accept(); };
    config.onPeerConnected = [](const Peer peer) { printf("new peer %s\n", peer.externalId().c_str()); };
    // Accept all stream request
    config.onStreamRequest = [](const StreamRequest request) {request.accept({true, true, true}); };
    config.onPeerDisconnect = [](const Peer peer) { printf("peer disconnected\n"); };
    config.onPeerMessage = [](Peer peer, const uint8_t* data, size_t size) { peer.send(data, size); };
    // Initialise runtime and wait for it to resolve
    std::unique_ptr<Runtime> runtime;
    auto promise = Runtime::initialize(config);
    promise.wait();
    // Enumerate results of initialize
    std::visit(
        Overload{
            [&](std::unique_ptr<Runtime> resolved) {
                // Consume initialised runtime
                runtime.swap(resolved);
                runtime->setLogLevel(RainwayLogLevel::RW_LOG_LEVEL_INFO);
                printf("Runtime initialized (version %s)\n", prettyVersion(runtime->version()).c_str());
                printf("You are %s\n", runtime->hostname().c_str());
                // Block main thread (was previously blocked by promise.wait())
                auto temp = std::string{};
                std::getline(std::cin, temp);
            },
            [](Error error) {
                printf("Runtime failed to load: %s\n", error.what().c_str());
            },
        }, promise.get());
}
