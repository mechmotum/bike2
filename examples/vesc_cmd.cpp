#include "third_party/vesc_uart/VescUart.h"

auto main() -> int
{
  ::VescUartSetCurrent(1.0F);

  return 0;
}
