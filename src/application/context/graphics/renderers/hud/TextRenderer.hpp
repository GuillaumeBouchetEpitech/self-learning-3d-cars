
#pragma once

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/Texture.hpp"

#include "geronimo/graphics/camera/Camera.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <regex>

class Shader;

struct TextRenderer {

public:
  enum class TextAlign {
    left = 0,
    center,
    right,
  };

public:
  struct State
  {
    std::optional<glm::vec4> color;
    std::optional<glm::vec4> outline;

    State(
      std::optional<glm::vec4> inColor = std::nullopt,
      std::optional<glm::vec4> inOutline = std::nullopt)
      : color(inColor)
      , outline(inOutline)
    {}
  };

public:
  struct MessageRectangle {
    glm::vec2 pos;
    glm::vec2 size;
  };

private:
  struct LetterOffset {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec4 color;
    float scale;
  };

private:

  struct Graphic {
    std::vector<LetterOffset> vertices;
    std::shared_ptr<gero::graphics::ShaderProgram> shader;
    std::shared_ptr<gero::graphics::Texture> texture;
    gero::graphics::Geometry geometry;
    gero::graphics::Camera::MatricesData matricesData;
  }
  _graphic;

  struct Logic {
    std::unordered_map<char, glm::vec2> charactersTexCoordMap;
    std::vector<uint32_t> allLinesWidth;
    std::vector<MessageRectangle> latestMessageRectangles;
    std::regex stateRegexp;
  }
  _logic;

public:
  TextRenderer() = default;
  ~TextRenderer() = default;

public:
  void initialise();
  void setMatricesData(const gero::graphics::Camera::MatricesData& matricesData);

public:
  void pushText(
    const glm::vec2& inPosition,
    const std::string_view inMessage,
    const glm::vec4& inColor,
    float inScale = 1.0f,
    float inDepth = 0.0f,
    const glm::vec4& inOutlineColor = {0, 0, 0, 1},
    TextAlign inTextAlign = TextAlign::left
  );

  template<typename ... Args>
  void pushText(
    const glm::vec2& inPosition,
    const std::string_view inMessage,
    const glm::vec4& inColor,
    float inScale,
    float inDepth,
    const glm::vec4& inOutlineColor,
    TextAlign inTextAlign,
    Args ... args
  ) {

    std::array<State, sizeof...(args)> allStates{{ args... }};

    _pushText(
      inPosition,
      inMessage,
      inColor,
      inScale,
      inDepth,
      inOutlineColor,
      inTextAlign,
      allStates.data(),
      allStates.size()
    );
  }

private:
  void _pushText(
    const glm::vec2& inPosition,
    const std::string_view inMessage,
    const glm::vec4& inColor,
    float inScale,
    float inDepth,
    const glm::vec4& inOutlineColor,
    TextAlign inTextAlign,
    const State* pStates,
    std::size_t totalStates
  );

private:
  void _pushSingleCharacter(
    const glm::vec3& inPosition,
    const glm::vec2& inTexCoord,
    float inScale,
    const glm::vec4& inColor,
    const glm::vec4& inOutlineColor
  );

public:
  const std::vector<MessageRectangle>& getLatestTextRectangles() const;

public:
  void clear();
  void render();
};
