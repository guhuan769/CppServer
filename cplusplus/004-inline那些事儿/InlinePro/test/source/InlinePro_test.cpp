#include "lib.hpp"

auto main() -> int
{
  auto const lib = library {};

  return lib.name == "InlinePro" ? 0 : 1;
}
