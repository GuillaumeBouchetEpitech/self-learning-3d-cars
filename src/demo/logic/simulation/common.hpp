
#pragma once

#include "geronimo/system/asValue.hpp"

#include <cstdint>

enum class Groups : uint16_t {
  all = 0b1111'1111'1111'1111,
  sensor = 0b0000'0000'0000'0001,
  ground = 0b0000'0000'0000'0010,
  wall = 0b0000'0000'0000'0100,
  vehicle = 0b0000'0000'0000'1000,
};

enum class Masks : uint16_t {
  // ground collide with anything
  ground = gero::asValue(Groups::all),
  // wall collide with anything
  wall = gero::asValue(Groups::all),
  // vehicle collide only with ground
  vehicle = gero::asValue(Groups::ground),
  // eyeSensor collide only with ground and wall
  eyeSensor = gero::asValue(Groups::ground) | gero::asValue(Groups::wall),
  // groundSensor collide only with ground
  groundSensor = gero::asValue(Groups::ground),
};
