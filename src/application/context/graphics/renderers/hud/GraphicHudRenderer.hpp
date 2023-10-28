
#pragma once

#include "post-process/PostProcess.hpp"

#include "widgets/CoreUsageRenderer.hpp"
#include "widgets/FitnessDataRenderer.hpp"
#include "widgets/InformationTextRenderer.hpp"
#include "widgets/LeaderEyeRenderer.hpp"
#include "widgets/ScreenTitles.hpp"
#include "widgets/ThirdPersonCamera.hpp"
#include "widgets/TopologyRenderer.hpp"

#include "geronimo/graphics/advanced-concept/stackRenderers/StackRenderers.hpp"
#include "geronimo/graphics/advanced-concept/textRenderer/TextRenderer.hpp"


class IGraphicHudRenderer
{
public:
  struct Widgets {
    TopologyRenderer topologyRenderer;
    ScreenTitles screenTitles;
    CoreUsageRenderer coreUsageRenderer;
    ThirdPersonCamera thirdPersonCamera;
    FitnessDataRenderer fitnessDataRenderer;
    InformationTextRenderer informationTextRenderer;
    LeaderEyeRenderer leaderEyeRenderer;
  };

public:
  virtual gero::graphics::ICamera& getCamera() = 0;
  virtual gero::graphics::IStackRenderers& getStackRenderers() = 0;
  virtual gero::graphics::ITextRenderer& getTextRenderer() = 0;
  virtual PostProcess& getPostProcess() = 0;
  virtual Widgets& getWidgets() = 0;
};

class GraphicHudRenderer : public IGraphicHudRenderer
{
public:
  using Widgets = IGraphicHudRenderer::Widgets;

public:
  GraphicHudRenderer() = default;
  ~GraphicHudRenderer() = default;

public:
  void initialize(uint32_t width, uint32_t height);
  void resize(uint32_t width, uint32_t height);
  void computeMatrices();

public:
  gero::graphics::ICamera& getCamera() override;
  gero::graphics::IStackRenderers& getStackRenderers() override;
  gero::graphics::ITextRenderer& getTextRenderer() override;
  PostProcess& getPostProcess() override;
  Widgets& getWidgets() override;

private:
  gero::graphics::Camera _camera;

  gero::graphics::StackRenderers _stackRenderers;
  gero::graphics::TextRenderer _textRenderer;
  PostProcess _postProcess;

  Widgets _widgets;

};
