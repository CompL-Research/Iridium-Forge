#pragma once
#include <msgpack.hpp>
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include "Iridium/IridiumBuildContext.h"

class IridiumFlag
{
public:
  enum class IridiumPrimitives
  {
    number,
    boolean,
    string,
    null
  };

  IridiumFlag(std::string key, double d)
      : key(std::move(key)), value(d), valueKind(IridiumPrimitives::number) {}

  IridiumFlag(std::string key, bool b)
      : key(std::move(key)), value(b), valueKind(IridiumPrimitives::boolean) {}

  IridiumFlag(std::string key, std::string s)
      : key(std::move(key)), value(std::move(s)), valueKind(IridiumPrimitives::string) {}

  IridiumFlag(std::string key)
      : key(std::move(key)), valueKind(IridiumPrimitives::null) {}

  std::string getKey() const { return key; }
  IridiumPrimitives getKind() const { return valueKind; }

  // Type-safe accessors (throw if wrong type)
  double getNumber() const { return std::get<double>(value); }
  bool getBoolean() const { return std::get<bool>(value); }
  const std::string &getString() const { return std::get<std::string>(value); }

  void dump(int indent = 0, std::ostream & oss = std::cout) const
  {
    std::string pad(indent, ' ');
    oss << pad << "Flag(" << key << ": ";
    switch (valueKind)
    {
    case IridiumPrimitives::number:
      oss << getNumber();
      break;
    case IridiumPrimitives::boolean:
      oss << (getBoolean() ? "true" : "false");
      break;
    case IridiumPrimitives::string:
      oss << "\"" << getString() << "\"";
      break;
    case IridiumPrimitives::null:
      oss << "null";
      break;
    }
    oss << ")\n";
  }

private:
  std::string key;
  std::variant<double, bool, std::string> value;
  IridiumPrimitives valueKind;
};

class IridiumSEXP
{
public:
  std::string tag;
  std::vector<std::shared_ptr<IridiumSEXP>> args;
  std::vector<std::shared_ptr<IridiumFlag>> flags;

  void dump(int indent = 0, std::ostream & oss = std::cout) const
  {
    std::string pad(indent, ' ');
    oss << pad << "SEXP(" << tag;

    if (!flags.empty())
    {
      oss << " [";
      for (size_t i = 0; i < flags.size(); ++i)
      {
        auto &f = flags[i];
        oss << f->getKey() << "=";
        switch (f->getKind())
        {
        case IridiumFlag::IridiumPrimitives::number:
          oss << f->getNumber();
          break;
        case IridiumFlag::IridiumPrimitives::boolean:
          oss << (f->getBoolean() ? "true" : "false");
          break;
        case IridiumFlag::IridiumPrimitives::string:
          oss << "\"" << f->getString() << "\"";
          break;
        case IridiumFlag::IridiumPrimitives::null:
          oss << "null";
          break;
        }
        if (i + 1 < flags.size())
          oss << ", ";
      }
      oss << "]";
    }

    oss << ")\n";

    for (auto &s : args)
      s->dump(indent + 2, oss);
  }

  bool hasFlag(const std::string & flagToCheck) {
    for (auto & flag : flags) {
      auto flagName = flag->getKey();
      if (flagName == flagToCheck) {
        return true;
      }
    }
    return false;
  }

  std::shared_ptr<IridiumFlag> getFlag(const std::string & flagToGet) {
    for (auto & flag : flags) {
      auto flagName = flag->getKey();
      if (flagName == flagToGet) {
        return flag;
      }
    }
    throw std::runtime_error("Tried to get flag " + flagToGet + " which does not exist!");
    exit(1);
  }
};

std::shared_ptr<IridiumSEXP> parseSEXP(const msgpack::object &obj);