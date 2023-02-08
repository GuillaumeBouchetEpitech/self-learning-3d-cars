
#include "TextRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/asValue.hpp"

namespace
{

  const glm::vec2 k_letterSize = {16, 16};
  const float k_letterAspectRatio = k_letterSize.x / k_letterSize.y;
  const glm::vec2 k_letterScale = {k_letterAspectRatio, 1.0f};

} // namespace name


namespace string_view_regexp {

  using match = std::match_results<std::string_view::const_iterator>;
  using sub_match = std::sub_match<std::string_view::const_iterator>;

  inline std::string_view get_string_view(const sub_match& m)
  {
    return std::string_view(m.first, m.length());
  }

  inline bool regex_match(
    std::string_view sv,
    match& m,
    const std::regex& e,
    std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
  {
    return std::regex_match(sv.begin(), sv.end(), m, e, flags);
  }

  inline bool regex_match(
    std::string_view sv,
    const std::regex& e,
    std::regex_constants::match_flag_type flags = std::regex_constants::match_default)
  {
    return std::regex_match(sv.begin(), sv.end(), e, flags);
  }

  inline bool regex_search(
    std::string_view sv,
    match& m,
    std::regex& r)
  {
    return std::regex_search(sv.begin(), sv.end(), m, r);
  }

}


void
TextRenderer::initialise() {

  _logic.allLinesWidth.reserve(64);
  _logic.latestMessageRectangles.reserve(64);

  auto& resourceManager = Context::get().graphic.resourceManager;

  _graphic.shader = resourceManager.getShader(gero::asValue(ShadersAliases::textRenderer));

  _graphic.texture = resourceManager.getTexture(0);

  const glm::vec2 textureSize = glm::vec2(_graphic.texture->getSize());

  const glm::vec2 texCoord = 1.0f / textureSize * k_letterSize;

  {

    const glm::vec2 minPosCoord = k_letterScale * -0.5f;
    const glm::vec2 maxPosCoord = k_letterScale * +0.5f;

    const glm::vec2 minTexCoord = { 0.0f, 0.0f };
    const glm::vec2 maxTexCoord = { texCoord.x, texCoord.y };

    struct VertexData {
      glm::vec2 position;
      glm::vec2 texCoord;
    };

    const std::array<VertexData, 4> vertices{{
      {{maxPosCoord.x, minPosCoord.y}, {maxTexCoord.x, maxTexCoord.y}},
      {{minPosCoord.x, minPosCoord.y}, {minTexCoord.x, maxTexCoord.y}},
      {{maxPosCoord.x, maxPosCoord.y}, {maxTexCoord.x, minTexCoord.y}},
      {{minPosCoord.x, maxPosCoord.y}, {minTexCoord.x, minTexCoord.y}}
    }};

    std::array<int, 6> indices{{1, 0, 2, 1, 3, 2}};

    std::vector<VertexData> letterVertices;
    letterVertices.reserve(indices.size()); // pre-allocate

    for (int index : indices)
      letterVertices.push_back(vertices.at(index));

    auto geoDef = resourceManager.getGeometryDefinition(gero::asValue(GeometriesAliases::textRenderer));
    _graphic.geometry.initialise(*_graphic.shader, geoDef);
    _graphic.geometry.updateBuffer(0, letterVertices);
    _graphic.geometry.setPrimitiveCount(letterVertices.size());
  }

  constexpr std::size_t preAllocatedSize = 1024;

  _graphic.vertices.reserve(preAllocatedSize * 8); // <= pre-allocate

  _logic.charactersTexCoordMap = {
    {' ', {0 * texCoord.x, 0 * texCoord.y}},
    {'!', {1 * texCoord.x, 0 * texCoord.y}},
    {'\"', {2 * texCoord.x, 0 * texCoord.y}},
    {'#', {3 * texCoord.x, 0 * texCoord.y}},
    {'$', {4 * texCoord.x, 0 * texCoord.y}},
    {'%', {5 * texCoord.x, 0 * texCoord.y}},
    {'&', {6 * texCoord.x, 0 * texCoord.y}},
    {'\'', {7 * texCoord.x, 0 * texCoord.y}},
    {'(', {8 * texCoord.x, 0 * texCoord.y}},
    {')', {9 * texCoord.x, 0 * texCoord.y}},
    {'*', {10 * texCoord.x, 0 * texCoord.y}},
    {'+', {11 * texCoord.x, 0 * texCoord.y}},
    {',', {12 * texCoord.x, 0 * texCoord.y}},
    {'-', {13 * texCoord.x, 0 * texCoord.y}},
    {'.', {14 * texCoord.x, 0 * texCoord.y}},
    {'/', {15 * texCoord.x, 0 * texCoord.y}},

    {'0', {0 * texCoord.x, 1 * texCoord.y}},
    {'1', {1 * texCoord.x, 1 * texCoord.y}},
    {'2', {2 * texCoord.x, 1 * texCoord.y}},
    {'3', {3 * texCoord.x, 1 * texCoord.y}},
    {'4', {4 * texCoord.x, 1 * texCoord.y}},
    {'5', {5 * texCoord.x, 1 * texCoord.y}},
    {'6', {6 * texCoord.x, 1 * texCoord.y}},
    {'7', {7 * texCoord.x, 1 * texCoord.y}},
    {'8', {8 * texCoord.x, 1 * texCoord.y}},
    {'9', {9 * texCoord.x, 1 * texCoord.y}},
    {':', {10 * texCoord.x, 1 * texCoord.y}},
    {';', {11 * texCoord.x, 1 * texCoord.y}},
    {'<', {12 * texCoord.x, 1 * texCoord.y}},
    {'=', {13 * texCoord.x, 1 * texCoord.y}},
    {'>', {14 * texCoord.x, 1 * texCoord.y}},
    {'?', {15 * texCoord.x, 1 * texCoord.y}},

    {'@', {0 * texCoord.x, 2 * texCoord.y}},
    {'A', {1 * texCoord.x, 2 * texCoord.y}},
    {'B', {2 * texCoord.x, 2 * texCoord.y}},
    {'C', {3 * texCoord.x, 2 * texCoord.y}},
    {'D', {4 * texCoord.x, 2 * texCoord.y}},
    {'E', {5 * texCoord.x, 2 * texCoord.y}},
    {'F', {6 * texCoord.x, 2 * texCoord.y}},
    {'G', {7 * texCoord.x, 2 * texCoord.y}},
    {'H', {8 * texCoord.x, 2 * texCoord.y}},
    {'I', {9 * texCoord.x, 2 * texCoord.y}},
    {'J', {10 * texCoord.x, 2 * texCoord.y}},
    {'K', {11 * texCoord.x, 2 * texCoord.y}},
    {'L', {12 * texCoord.x, 2 * texCoord.y}},
    {'M', {13 * texCoord.x, 2 * texCoord.y}},
    {'N', {14 * texCoord.x, 2 * texCoord.y}},
    {'O', {15 * texCoord.x, 2 * texCoord.y}},

    {'P', {0 * texCoord.x, 3 * texCoord.y}},
    {'Q', {1 * texCoord.x, 3 * texCoord.y}},
    {'R', {2 * texCoord.x, 3 * texCoord.y}},
    {'S', {3 * texCoord.x, 3 * texCoord.y}},
    {'T', {4 * texCoord.x, 3 * texCoord.y}},
    {'U', {5 * texCoord.x, 3 * texCoord.y}},
    {'V', {6 * texCoord.x, 3 * texCoord.y}},
    {'W', {7 * texCoord.x, 3 * texCoord.y}},
    {'X', {8 * texCoord.x, 3 * texCoord.y}},
    {'Y', {9 * texCoord.x, 3 * texCoord.y}},
    {'Z', {10 * texCoord.x, 3 * texCoord.y}},
    {'[', {11 * texCoord.x, 3 * texCoord.y}},
    {'\\', {12 * texCoord.x, 3 * texCoord.y}},
    {']', {13 * texCoord.x, 3 * texCoord.y}},
    {'^', {14 * texCoord.x, 3 * texCoord.y}},
    {'_', {15 * texCoord.x, 3 * texCoord.y}},

    {'`', {0 * texCoord.x, 4 * texCoord.y}},
    {'a', {1 * texCoord.x, 4 * texCoord.y}},
    {'b', {2 * texCoord.x, 4 * texCoord.y}},
    {'c', {3 * texCoord.x, 4 * texCoord.y}},
    {'d', {4 * texCoord.x, 4 * texCoord.y}},
    {'e', {5 * texCoord.x, 4 * texCoord.y}},
    {'f', {6 * texCoord.x, 4 * texCoord.y}},
    {'g', {7 * texCoord.x, 4 * texCoord.y}},
    {'h', {8 * texCoord.x, 4 * texCoord.y}},
    {'i', {9 * texCoord.x, 4 * texCoord.y}},
    {'j', {10 * texCoord.x, 4 * texCoord.y}},
    {'k', {11 * texCoord.x, 4 * texCoord.y}},
    {'l', {12 * texCoord.x, 4 * texCoord.y}},
    {'m', {13 * texCoord.x, 4 * texCoord.y}},
    {'n', {14 * texCoord.x, 4 * texCoord.y}},
    {'o', {15 * texCoord.x, 4 * texCoord.y}},

    {'p', {0 * texCoord.x, 5 * texCoord.y}},
    {'q', {1 * texCoord.x, 5 * texCoord.y}},
    {'r', {2 * texCoord.x, 5 * texCoord.y}},
    {'s', {3 * texCoord.x, 5 * texCoord.y}},
    {'t', {4 * texCoord.x, 5 * texCoord.y}},
    {'u', {5 * texCoord.x, 5 * texCoord.y}},
    {'v', {6 * texCoord.x, 5 * texCoord.y}},
    {'w', {7 * texCoord.x, 5 * texCoord.y}},
    {'x', {8 * texCoord.x, 5 * texCoord.y}},
    {'y', {9 * texCoord.x, 5 * texCoord.y}},
    {'z', {10 * texCoord.x, 5 * texCoord.y}},
    {'{', {11 * texCoord.x, 5 * texCoord.y}},
    {'|', {12 * texCoord.x, 5 * texCoord.y}},
    {'}', {13 * texCoord.x, 5 * texCoord.y}},
    {'~', {14 * texCoord.x, 5 * texCoord.y}},

  };


  _logic.stateRegexp = std::regex(R"(\$\{(\d+)\})");

}

void
TextRenderer::setMatricesData(
  const gero::graphics::Camera::MatricesData& matricesData) {
  _graphic.matricesData = matricesData;
}

//

void
TextRenderer::pushText(
  const glm::vec2& inPosition,
  const std::string_view inMessage,
  const glm::vec4& inColor, float inScale /* = 1.0f */, float inDepth, /*= 0.0f*/
  const glm::vec4& inOutlineColor, /*= {0,0,0,1}*/
  TextAlign inTextAlign /* = TextAlign::left */
) {

  _pushText(
    inPosition,
    inMessage,
    inColor,
    inScale,
    inDepth,
    inOutlineColor,
    inTextAlign,
    nullptr,
    0
  );

}

void TextRenderer::_pushText(
  const glm::vec2& inPosition,
  const std::string_view inMessage,
  const glm::vec4& inColor,
  float inScale,
  float inDepth,
  const glm::vec4& inOutlineColor,
  TextAlign inTextAlign,
  const State* inPStates,
  std::size_t inTotalStates
) {

  const glm::vec2 charSize = k_letterScale * inScale;
  const glm::vec2 charHSize = charSize * 0.5f;

  _logic.latestMessageRectangles.clear();

  if (inTextAlign != TextAlign::left) {

    _logic.allLinesWidth.clear();
    _logic.allLinesWidth.push_back(0);

    for (std::size_t index = 0; index < inMessage.size(); )
    {
      const char currCharacter = inMessage.at(index);

      if (currCharacter == '$') {

        std::string_view subStr = &inMessage.at(index);

        string_view_regexp::match match;
        if (string_view_regexp::regex_search(subStr.data(), match, _logic.stateRegexp)) {
          index += match.length(0);
          continue;
        }
      }

      if (currCharacter == '\n')
        _logic.allLinesWidth.push_back(0);
      else
        _logic.allLinesWidth.back() += 1;

      ++index;
    }
  }

  std::size_t currentLineIndex = 0;
  const glm::vec2 basePos = inPosition + charHSize;
  glm::vec3 currPos = glm::vec3(basePos, inDepth); // TextAlign::left

  if (inTextAlign != TextAlign::left)
  {
    const uint32_t currenLineSize = _logic.allLinesWidth.at(currentLineIndex);

    if (inTextAlign == TextAlign::center) {
      currPos.x -= float(currenLineSize) * charHSize.x;
    } else if (inTextAlign == TextAlign::right) {
      currPos.x -= float(currenLineSize) * charSize.x;
    }
  }

  MessageRectangle currentRectangle;
  currentRectangle.pos = glm::vec2(currPos) - charHSize;
  currentRectangle.size = {0.0f, charSize.y};

  glm::vec4 currColor = inColor;
  glm::vec4 currOutline = inOutlineColor;

  //

  for (std::size_t index = 0; index < inMessage.size(); )
  {
    const char currCharacter = inMessage.at(index);

    if (currCharacter == '$') {

      std::string_view subStr = &inMessage.at(index);

      string_view_regexp::match match;
      if (string_view_regexp::regex_search(subStr.data(), match, _logic.stateRegexp)) {

        const string_view_regexp::sub_match sub_match = match[1];
        const std::string_view value = string_view_regexp::get_string_view(sub_match);

        const int32_t intVal = std::atoi(value.data());

        if (intVal >= 0 && intVal < int32_t(inTotalStates))
        {
          const State& currState = inPStates[intVal];

          if (currState.color) {
            currColor = currState.color.value();
          }
          if (currState.outline) {
            currOutline = currState.outline.value();
          }

        }

        index += match.length(0);
        continue;
      }
    }

    if (currCharacter == '\n')
    {
      currentLineIndex += 1;

      currPos.x = basePos.x; // TextAlign::left

      if (inTextAlign != TextAlign::left)
      {
        const uint32_t currenLineSize = _logic.allLinesWidth.at(currentLineIndex);

        if (inTextAlign == TextAlign::center) {
          currPos.x -= float(currenLineSize) * charHSize.x;
        } else if (inTextAlign == TextAlign::right) {
          currPos.x -= float(currenLineSize) * charSize.x;
        }
      }

      currPos.y -= charSize.y;

      if (currentRectangle.size.x > 0)
        _logic.latestMessageRectangles.push_back(currentRectangle);

      currentRectangle.pos = glm::vec2(currPos) - charHSize;
      currentRectangle.size = {0.0f, charSize.y};

      ++index;
      continue;
    }

    auto it = _logic.charactersTexCoordMap.find(currCharacter);
    if (it == _logic.charactersTexCoordMap.end())
      D_THROW(std::runtime_error, "fail to find a letter, letter=" << currCharacter);

    const auto& texCoord = it->second;

    _pushSingleCharacter(currPos, texCoord, inScale, currColor, currOutline);

    currPos.x += charSize.x;
    currentRectangle.size.x += inScale;
    ++index;
  }

  if (currentRectangle.size.x > 0)
    _logic.latestMessageRectangles.push_back(currentRectangle);

}

void TextRenderer::_pushSingleCharacter(
  const glm::vec3& inPosition,
  const glm::vec2& inTexCoord,
  float inScale,
  const glm::vec4& inColor,
  const glm::vec4& inOutlineColor
) {

  _graphic.vertices.push_back({inPosition, inTexCoord, inColor, inScale});

  { // background

    const glm::vec2 tmpScale = glm::vec2(inScale) / k_letterSize;

    const glm::vec4 currColor = inOutlineColor;
    constexpr int k_range = 1;
    glm::vec3 offsetPos = glm::vec3(0, 0, -0.01f);

    for (int stepY = -k_range; stepY <= +k_range; stepY += k_range) {

      offsetPos.y = tmpScale.y * stepY;

      for (int stepX = -k_range; stepX <= +k_range; stepX += k_range) {
        if (stepX == 0 && stepY == 0)
          continue;

        offsetPos.x = tmpScale.x * stepX;

        _graphic.vertices.push_back({inPosition + offsetPos, inTexCoord, currColor, inScale});
      }
    }

  } // background

}

const std::vector<TextRenderer::MessageRectangle>&
TextRenderer::getLatestTextRectangles() const
{
  return _logic.latestMessageRectangles;
}

void
TextRenderer::clear() {
  _graphic.vertices.clear();
}

void
TextRenderer::render() {
  if (_graphic.vertices.empty())
    return;

  if (!_graphic.shader)
    D_THROW(std::runtime_error, "shader not setup");

  _graphic.shader->bind();
  _graphic.shader->setUniform("u_composedMatrix", _graphic.matricesData.composed);

  _graphic.texture->bind();

  _graphic.geometry.updateBuffer(1, _graphic.vertices);
  _graphic.geometry.setInstancedCount(_graphic.vertices.size());

  _graphic.geometry.render();
}
