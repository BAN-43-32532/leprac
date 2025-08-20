#include <iostream>
#include <toml.hpp>

int main() {
  std::string a{"123"};
  toml::value v = toml::table{
    {a, 1},
    {"1.1", 1}
  };
  auto& va = v[a];
  va = 111;
  std::cout << toml::format(v) << std::endl;
}
