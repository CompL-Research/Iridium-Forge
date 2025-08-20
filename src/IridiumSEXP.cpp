#include "Iridium/IridiumSEXP.h"

std::shared_ptr<IridiumSEXP> parseSEXP(const msgpack::object &obj)
{
  if (obj.type != msgpack::type::ARRAY || obj.via.array.size != 3)
    throw std::runtime_error("Invalid SEXP object");

  auto sexp = std::make_shared<IridiumSEXP>();

  // Add Tag
  auto tagHolder = obj.via.array.ptr[0];
  assert(tagHolder.type == msgpack::type::STR);
  sexp->tag = tagHolder.as<std::string>();

  // Recurse SEXP for Args
  auto argsHolder = obj.via.array.ptr[1];
  assert(argsHolder.type == msgpack::type::ARRAY);
  for (uint32_t i = 0; i < argsHolder.via.array.size; i++)
  {
    const msgpack::object &arg = argsHolder.via.array.ptr[i];
    sexp->args.push_back(parseSEXP(arg));
  }

  // Add Flags
  auto flagsHolder = obj.via.array.ptr[2];
  assert(flagsHolder.type == msgpack::type::ARRAY);
  for (uint32_t i = 0; i < flagsHolder.via.array.size; i++)
  {
    const msgpack::object &flag = flagsHolder.via.array.ptr[i];
    assert(
        flag.type == msgpack::type::ARRAY &&
        flag.via.array.size == 2 &&
        flag.via.array.ptr[0].type == msgpack::type::STR);
    std::string key = flag.via.array.ptr[0].as<std::string>();
    const msgpack::object &val = flag.via.array.ptr[1];
    switch (val.type)
    {
    case msgpack::type::NIL:
      sexp->flags.push_back(std::make_shared<IridiumFlag>(key));
      break;
    case msgpack::type::BOOLEAN:
      sexp->flags.push_back(std::make_shared<IridiumFlag>(key, val.as<bool>()));
      break;
    case msgpack::type::FLOAT32:
    case msgpack::type::FLOAT64:
    case msgpack::type::POSITIVE_INTEGER:
    case msgpack::type::NEGATIVE_INTEGER:
      sexp->flags.push_back(std::make_shared<IridiumFlag>(key, val.as<double>()));
      break;
    case msgpack::type::STR:
      sexp->flags.push_back(std::make_shared<IridiumFlag>(key, val.as<std::string>()));
      break;
    default:
      throw std::runtime_error("Invalid val field for flag");
      break;
    }
  }

  // Populate bbIdxToSEXPMap
  if (sexp->tag == "BB") {
    assert(sexp->hasFlag("IDX"));
    auto idxFlag = sexp->getFlag("IDX");
    bbIdxToSEXPMap[idxFlag->getNumber()] = sexp;
  }

  return sexp;
}