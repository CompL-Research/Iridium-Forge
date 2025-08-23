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

  void dump(int indent = 0, std::ostream &oss = std::cout) const
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

  void dumpJSON(std::ostream &oss = std::cout) const
  {
    oss << "\"" << key << "\": ";
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
  }

private:
  std::string key;
  std::variant<double, bool, std::string> value;
  IridiumPrimitives valueKind;
};

class IridiumSEXP
{
public:
  virtual ~IridiumSEXP() = default; // makes it polymorphic? IDK how Cpp works!!
  std::string tag;
  std::vector<IRISEXP> args;
  std::vector<std::shared_ptr<IridiumFlag>> flags;

  void dump(int indent = 0, std::ostream &oss = std::cout) const
  {
    std::string pad(indent, ' ');
    oss << pad << "SEXP(" << tag;

    if (!flags.empty())
    {
      oss << " [";
      for (size_t i = 0; i < flags.size(); ++i)
      {
        auto &f = flags[i];
        // oss << f->getKey() << "=";
        switch (f->getKind())
        {
        case IridiumFlag::IridiumPrimitives::number:
          oss << f->getKey() << "=" << f->getNumber();
          break;
        case IridiumFlag::IridiumPrimitives::boolean:
          oss << f->getKey() << "=" << (f->getBoolean() ? "true" : "false");
          break;
        case IridiumFlag::IridiumPrimitives::string:
          oss << f->getKey() << "=" << "\"" << f->getString() << "\"";
          break;
        case IridiumFlag::IridiumPrimitives::null:
          oss << f->getKey();
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

  void dumpJSON(int indent = 0, std::ostream &oss = std::cout) const
  {
    std::string pad(indent, ' ');
    oss << pad << "{\n";

    // Tag
    oss << pad << "  \"tag\": \"" << tag << "\"";

    // Flags
    if (!flags.empty())
    {
      oss << ",\n"
          << pad << "  \"flags\": {";
      for (size_t i = 0; i < flags.size(); ++i)
      {
        flags[i]->dumpJSON(oss);
        if (i + 1 < flags.size())
          oss << ", ";
      }
      oss << "}";
    }

    // Args
    if (!args.empty())
    {
      oss << ",\n"
          << pad << "  \"args\": [\n";
      for (size_t i = 0; i < args.size(); ++i)
      {
        args[i]->dumpJSON(indent + 4, oss);
        if (i + 1 < args.size())
          oss << ",";
        oss << "\n";
      }
      oss << pad << "  ]";
    }

    oss << "\n"
        << pad << "}";
  }

  void setFlag(const std::string &flagToSet)
  {
    removeFlag(flagToSet);
    flags.push_back(std::make_shared<IridiumFlag>(flagToSet));
  }

  void setFlag(const std::string &flagToSet, double number)
  {
    removeFlag(flagToSet);
    flags.push_back(std::make_shared<IridiumFlag>(flagToSet, number));
  }

  void setFlag(const std::string &flagToSet, bool boolean)
  {
    removeFlag(flagToSet);
    flags.push_back(std::make_shared<IridiumFlag>(flagToSet, boolean));
  }

  void setFlag(const std::string &flagToSet, std::string str)
  {
    removeFlag(flagToSet);
    flags.push_back(std::make_shared<IridiumFlag>(flagToSet, str));
  }

  bool hasFlag(const std::string &flagToCheck)
  {
    for (auto &flag : flags)
    {
      auto flagName = flag->getKey();
      if (flagName == flagToCheck)
      {
        return true;
      }
    }
    return false;
  }

  void removeFlag(const std::string &flagToRemove)
  {
    // Remove all elements where value is even
    flags.erase(
        std::remove_if(flags.begin(), flags.end(),
                       [&](const auto &f)
                       { return f->getKey() == flagToRemove; }),
        flags.end());
  }

  double getFlagDouble(const std::string &flagToGet)
  {
    for (auto &flag : flags)
    {
      auto flagName = flag->getKey();
      if (flagName == flagToGet)
      {
        return flag->getNumber();
      }
    }
    throw std::runtime_error("getFlagDouble failed");
  }

  std::string getFlagString(const std::string &flagToGet)
  {
    for (auto &flag : flags)
    {
      auto flagName = flag->getKey();
      if (flagName == flagToGet)
      {
        return flag->getString();
      }
    }
    throw std::runtime_error("getFlagString failed");
  }

  bool getFlagBoolean(const std::string &flagToGet)
  {
    for (auto &flag : flags)
    {
      auto flagName = flag->getKey();
      if (flagName == flagToGet)
      {
        return flag->getBoolean();
      }
    }
    throw std::runtime_error("getFlagBoolean failed");
  }
};

IRISEXP parseSEXP(const msgpack::object &obj, std::unordered_map<int, std::shared_ptr<BBSEXP>> *bbIdxToSEXPMap = NULL);