

#include "CircuitBuilder.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/math/BSpline.hpp"
#include "geronimo/system/math/compute-normal.hpp"
#include "geronimo/system/parser-utils/BasicRegexParser.hpp"
#include "geronimo/system/string-utils/trim.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept> // <= std::invalid_argument / runtime_error
#include <string_view>
#include <unordered_map>

#include <cmath>

//
//

namespace {

class CircuitParser {
private:
  gero::parserUtils::BasicRegexParser _regexpParser;

  using StrategyCallback = std::function<void(std::istringstream&)>;
  std::unordered_map<std::string, StrategyCallback> _commandsMap;

private:
  const std::string_view cmd_name_agent_start_transform = "AGENTS_START_TRANSFORM";
  const std::string_view cmd_name_next_knots_states = "NEXT_KNOTS_STATES";
  const std::string_view cmd_name_push_knots = "PUSH_KNOTS";

private:
  CircuitBuilder::StartTransform& _startTransform;
  CircuitBuilder::Knots& _rawKnots;

  float _currentKnotsSize = 2.0f;
  glm::vec3 _currentColor = {0.0f, 0.0f, 0.0f};

public:
  CircuitParser(CircuitBuilder::StartTransform& startTransform, CircuitBuilder::Knots& rawKnots)
    : _startTransform(startTransform), _rawKnots(rawKnots) {
    {
      // AGENTS_START_TRANSFORM
      //   position="{float},{float},{float}"
      //   quaternion="{float},{float},{float},{float}"

      _commandsMap[std::string(cmd_name_agent_start_transform)] = [this](std::istringstream& isstr) {
        _regexpParser.setErrorHint(cmd_name_agent_start_transform);
        _regexpParser.forEachArgs(isstr.str(), [this](const std::string_view key, const std::string_view value) {
          if (key == "position") {
            _startTransform.position = _regexpParser.get3F(value, -10000.0f, +10000.0f);
          } else if (key == "quaternion") {
            _startTransform.quaternion = _regexpParser.get4F(value, -10000.0f, +10000.0f);
          }
        });
      };
    }

    {
      // NEXT_KNOTS_STATES
      //   size="{float}"
      //   color="{float},{float},{float}"
      _commandsMap[std::string(cmd_name_next_knots_states)] = [this](std::istringstream& isstr) {
        _regexpParser.setErrorHint(cmd_name_next_knots_states);
        _regexpParser.forEachArgs(isstr.str(), [this](const std::string_view key, const std::string_view value) {
          if (key == "size") {
            _currentKnotsSize = _regexpParser.get1F(value, 0.001f, +10000.0f);
          } else if (key == "color") {
            _currentColor = _regexpParser.get3F(value, 0.0f, +1.0f);
          }
        });
      };
    }

    {
      // PUSH_KNOTS
      //   left="{float},{float},{float}"
      //   right="{float},{float},{float}"
      _commandsMap[std::string(cmd_name_push_knots)] = [this](std::istringstream& isstr) {
        glm::vec3 left = {0, 0, 0};
        glm::vec3 right = {0, 0, 0};

        _regexpParser.setErrorHint(cmd_name_push_knots);
        _regexpParser.forEachArgs(
          isstr.str(), [this, &left, &right](const std::string_view key, const std::string_view value) {
            if (key == "left") {
              left = _regexpParser.get3F(value, -10000.0, +10000.0f);
            } else if (key == "right") {
              right = _regexpParser.get3F(value, -10000.0, +10000.0f);
            }
          });

        _rawKnots.push_back({left, right, _currentKnotsSize, _currentColor});
      };
    }
  }

public:
  void
  parseLine(const std::string& inTextLine) {
    std::istringstream isstr(inTextLine);

    // extract command type
    std::string commandType;
    if (!(isstr >> commandType))
      D_THROW(std::runtime_error, "failure to extract the line type");

    // retrieve command from type
    auto itCmd = _commandsMap.find(commandType);
    if (itCmd == _commandsMap.end())
      D_THROW(std::runtime_error, "unknown line type, type=" << commandType);

    // run command (parse)
    itCmd->second(isstr);
  }
};

} // namespace

//
//

void
CircuitBuilder::parse(const std::string_view& filename) {
  //
  //
  // open/read file

  std::ifstream fileStream(filename.data());

  if (!fileStream.is_open())
    D_THROW(std::runtime_error, "fail to read a file, name=" << filename);

  //
  //
  // parse file and extract skeleton

  Knots rawKnots;
  CircuitParser tmpParser(_startTransform, rawKnots);

  std::string textLine;
  while (std::getline(fileStream, textLine)) {
    // trim extracted line
    gero::stringUtils::trim(textLine);

    // skip empty line(s) and comment(s)
    if (textLine.empty() || textLine.front() == '#')
      continue;

    tmpParser.parseLine(textLine);
  }

  //
  //
  // concatenate skeleton

  for (CircuitBuilder::Knot& knot : rawKnots) {
    if (!_knots.empty()) {
      const auto& prevKnot = _knots.back();

      // concatenate the knot with the previous one
      // => this is so the next knots can have relative coordinates
      // => it make the circuit text file easier to write
      knot.left += prevKnot.left;
      knot.right += prevKnot.right;
    }

    _knots.push_back(knot);
  }
}

void
CircuitBuilder::load(const CircuitBuilder::StartTransform& startTransform, const CircuitBuilder::Knots& knots) {
  _startTransform = startTransform;
  _knots = knots;
}

//
//

void
CircuitBuilder::generateWireFrameSkeleton(CallbackNoNormals onSkeletonPatch) {
  if (!onSkeletonPatch)
    D_THROW(std::invalid_argument, "no callback provided");

  if (_knots.empty())
    D_THROW(std::runtime_error, "not initialized");

  Vec3Array vertices;
  Indices indices;

  vertices.reserve(_knots.size() * 4);    // pre-allocate
  indices.reserve(_knots.size() * 8 + 8); // pre-allocate

  // should be 0 but is not nice when shown with the chessboard floor
  constexpr float floorValue = 0.0f;

  for (std::size_t ii = 0; ii < _knots.size(); ++ii) {
    const auto& knot = _knots.at(ii);

    // "real" value
    vertices.push_back(knot.left);
    vertices.push_back(knot.right);
    // "on the floor" value
    vertices.emplace_back(knot.left.x, knot.left.y, floorValue);
    vertices.emplace_back(knot.right.x, knot.right.y, floorValue);

    // from "the floor" to the "real" value

    const int currIndex = ii * 4;

    // indices.push_back(currIndex + 0); // upper 0 left-right
    // indices.push_back(currIndex + 1);

    indices.push_back(currIndex + 2); // lower 0 left-right
    indices.push_back(currIndex + 3);

    indices.push_back(currIndex + 0); // lower-upper 0 left
    indices.push_back(currIndex + 2);

    indices.push_back(currIndex + 1); // lower-upper 0 right
    indices.push_back(currIndex + 3);

    if (ii == 0)
      continue;

    // from the "previous" to the "current" value

    const int prevIndex = (ii - 1) * 4;

    indices.push_back(currIndex + 0); // upper 0-1 left
    indices.push_back(prevIndex + 0);

    indices.push_back(currIndex + 1); // upper 0-1 right
    indices.push_back(prevIndex + 1);

    indices.push_back(currIndex + 2); // lower 0-1 left
    indices.push_back(prevIndex + 2);

    indices.push_back(currIndex + 3); // lower 0-1 right
    indices.push_back(prevIndex + 3);
  }

  onSkeletonPatch(vertices, indices);
}

void
CircuitBuilder::generateSmoothedKnotsData(Knots& smoothedKnotsData) {
  smoothedKnotsData.clear();
  smoothedKnotsData.reserve(2048); // pre-allocate, ease the reallocation

  enum class SplineType : uint32_t {
    leftX = 0,
    leftY,
    leftZ,
    rightX,
    rightY,
    rightZ,
    size,
    colorR,
    colorG,
    colorB,

    count,
  };

  gero::math::BSpline::Definition smootherDef;
  smootherDef.dimensions = gero::asValue(SplineType::count);
  smootherDef.degree = 3;
  smootherDef.knotsLength = _knots.size() * smootherDef.dimensions;
  smootherDef.getDataCallback = [this](uint32_t index) {
    const float* dataPtr = &_knots.front().left.x;
    return dataPtr[index];
  };

  gero::math::BSpline bsplineSmoother;
  bsplineSmoother.initialize(smootherDef);

  CircuitBuilder::CircuitVertex vertex;

  constexpr uint32_t maxIterations = 1000;
  constexpr float step = 1.0f / maxIterations; // tiny steps

  for (float coef = 0.0f; coef <= 1.0f; coef += step) {
    vertex.left.x = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::leftX));
    vertex.left.y = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::leftY));
    vertex.left.z = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::leftZ));
    vertex.right.x = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::rightX));
    vertex.right.y = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::rightY));
    vertex.right.z = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::rightZ));

    if (!smoothedKnotsData.empty()) {
      // both left and right vertices must be far enough to be included
      const auto& lastVertex = smoothedKnotsData.back();

      constexpr float triangleLength = 4.0f;

      if (
        glm::distance(lastVertex.left, vertex.left) < triangleLength ||
        glm::distance(lastVertex.right, vertex.right) < triangleLength) {
        continue;
      }
    }

    // check for invalid values (it create graphic and physic artifacts)
    const float minLength = 0.001f;
    if (glm::length(vertex.left) < minLength || glm::length(vertex.right) < minLength)
      continue; // TODO: fix it

    vertex.size = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::size));

    vertex.color.r = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::colorR));
    vertex.color.g = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::colorG));
    vertex.color.b = bsplineSmoother.calcAt(coef, gero::asValue(SplineType::colorB));

    smoothedKnotsData.push_back(vertex);
  }
}

void
CircuitBuilder::generateCircuitGeometry(
  CallbackNormals onNewGroundPatch, CallbackNormals onNewLeftWallPatch, CallbackNormals onNewRightWallPatch) {
  if (_knots.empty())
    D_THROW(std::runtime_error, "not initialized");

  if (!onNewGroundPatch && !onNewLeftWallPatch && !onNewRightWallPatch)
    D_THROW(std::invalid_argument, "no callbacks provided");

  Knots smoothedKnotsData;
  generateSmoothedKnotsData(smoothedKnotsData);

  std::size_t patchesPerKnot = 6U;

  glm::vec3 prevNormal;

  struct CircuitPatchData {
    Vec3Array vertices;
    Vec3Array normals;

    CircuitPatchData() {
      vertices.reserve(512); // pre-allocate
      normals.reserve(512);  // pre-allocate
    }
  };

  Indices indices;
  CircuitPatchData ground;
  CircuitPatchData leftWall;
  CircuitPatchData rightWall;
  Vec3Array circuitPatchColors;

  indices.reserve(512);            // pre-allocate
  circuitPatchColors.reserve(512); // pre-allocate

  const uint32_t startIndex = 1;
  for (std::size_t index = startIndex; index < smoothedKnotsData.size(); index += patchesPerKnot) {
    indices.clear();
    ground.vertices.clear();
    leftWall.vertices.clear();
    rightWall.vertices.clear();
    circuitPatchColors.clear();

    //
    //
    // prepare the vertices

    const auto& tmpKnot = smoothedKnotsData.at(index);
    //

    patchesPerKnot = 6U + std::size_t(tmpKnot.size) * 2U;

    int indicesIndex = 0;

    for (std::size_t stepIndex = index; stepIndex < smoothedKnotsData.size() && stepIndex < index + patchesPerKnot;
         ++stepIndex) {
      const int currIndex = indicesIndex++ * 4;
      indices.push_back(currIndex + 0);
      indices.push_back(currIndex + 1);
      indices.push_back(currIndex + 2);
      indices.push_back(currIndex + 0);
      indices.push_back(currIndex + 3);
      indices.push_back(currIndex + 2);

      const auto& prevKnot = smoothedKnotsData.at(stepIndex - 1);
      const auto& currKnot = smoothedKnotsData.at(stepIndex);

      const glm::vec3& prevLeft = prevKnot.left;
      const glm::vec3& prevRight = prevKnot.right;
      const glm::vec3& currLeft = currKnot.left;
      const glm::vec3& currRight = currKnot.right;

      const glm::vec3& prevColor = prevKnot.color;
      const glm::vec3& currColor = currKnot.color;

      const glm::vec3 currNormal = gero::math::computeNormal(prevLeft, prevRight, currRight);

      // for the first time
      if (stepIndex == startIndex)
        prevNormal = currNormal;

      const glm::vec3 prevNormalLeft(prevNormal.x, prevNormal.z, prevNormal.y);
      const glm::vec3 currNormalLeft(currNormal.x, currNormal.z, currNormal.y);
      const glm::vec3 prevNormalRight = -prevNormalLeft;
      const glm::vec3 currNormalRight = -currNormalLeft;

      //

      ground.vertices.push_back(prevLeft);
      ground.vertices.push_back(prevRight);
      ground.vertices.push_back(currRight);
      ground.vertices.push_back(currLeft);

      ground.normals.push_back(prevNormal);
      ground.normals.push_back(prevNormal);
      ground.normals.push_back(currNormal);
      ground.normals.push_back(currNormal);

      circuitPatchColors.push_back(prevColor);
      circuitPatchColors.push_back(prevColor);
      circuitPatchColors.push_back(currColor);
      circuitPatchColors.push_back(currColor);

      //

      const glm::vec3 prevHeight = prevNormal * 10.0f;
      const glm::vec3 currHeight = currNormal * 10.0f;

      //

      const glm::vec3 prevTopLeft = prevLeft + prevHeight;
      const glm::vec3 currTopLeft = currLeft + currHeight;

      leftWall.vertices.push_back(prevLeft);
      leftWall.vertices.push_back(prevTopLeft);
      leftWall.vertices.push_back(currTopLeft);
      leftWall.vertices.push_back(currLeft);

      leftWall.normals.push_back(prevNormalLeft);
      leftWall.normals.push_back(prevNormalLeft);
      leftWall.normals.push_back(currNormalLeft);
      leftWall.normals.push_back(currNormalLeft);

      //

      const glm::vec3 prevTopRight = prevRight + prevHeight;
      const glm::vec3 currTopRight = currRight + currHeight;

      rightWall.vertices.push_back(prevRight);
      rightWall.vertices.push_back(prevTopRight);
      rightWall.vertices.push_back(currTopRight);
      rightWall.vertices.push_back(currRight);

      rightWall.normals.push_back(prevNormalRight);
      rightWall.normals.push_back(prevNormalRight);
      rightWall.normals.push_back(currNormalRight);
      rightWall.normals.push_back(currNormalRight);

      //

      prevNormal = currNormal;
    }

    if (onNewGroundPatch)
      onNewGroundPatch(ground.vertices, circuitPatchColors, ground.normals, indices);

    if (onNewLeftWallPatch)
      onNewLeftWallPatch(leftWall.vertices, circuitPatchColors, leftWall.normals, indices);
    if (onNewRightWallPatch)
      onNewRightWallPatch(rightWall.vertices, circuitPatchColors, rightWall.normals, indices);
  }
}

const CircuitBuilder::StartTransform&
CircuitBuilder::getStartTransform() const {
  return _startTransform;
}

const CircuitBuilder::Knots&
CircuitBuilder::getKnots() const {
  return _knots;
}
