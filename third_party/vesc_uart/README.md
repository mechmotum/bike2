Interface library for UART interaction with the VESC motor controller
=====================================================================
The library has a high-level API for sending commands to the motor controller.
It requires a UART abstraction library with the following API in order to
build.

```cpp
// HardwareSerial.hpp

struct HardwareSerial {
    auto available() const -> bool;

    auto read() const -> std::uint8_t;

    auto write(std::uint8_t* buffer, std::size_t size) const -> void;
};
```

A global variable named `Serial` of the above type has to be provided when
linking the code.
