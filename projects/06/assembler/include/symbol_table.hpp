#pragma once

#include <string>
#include <unordered_map>

class SymbolTable {
 public:
  SymbolTable();

  void AddEntry(const std::string& symbol, int address);
  bool Contains(const std::string& symbol) const;
  int GetAddress(const std::string& symbol) const;

 private:
  std::unordered_map<std::string, int> table_;
};
