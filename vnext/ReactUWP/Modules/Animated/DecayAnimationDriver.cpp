// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <math.h>
#include "DecayAnimationDriver.h"

namespace react {
namespace uwp {
DecayAnimationDriver::DecayAnimationDriver(
    int64_t id,
    int64_t animatedValueTag,
    const Callback &endCallback,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimationDriver(id, animatedValueTag, endCallback, config, manager) {
  m_deceleration =
      config.find(s_decelerationName).dereference().second.asDouble();
  assert(m_deceleration > 0);
  m_velocity = config.find(s_velocityName).dereference().second.asDouble();
}

std::tuple<winrt::CompositionAnimation, winrt::CompositionScopedBatch>
DecayAnimationDriver::MakeAnimation(const folly::dynamic &config) {
  const auto [scopedBatch, animation] = []() {
    const auto compositor = winrt::Window::Current().Compositor();
    return std::make_tuple(
        compositor.CreateScopedBatch(
            winrt::CompositionBatchTypes::AllAnimations),
        compositor.CreateScalarKeyFrameAnimation());
  }();

  std::chrono::milliseconds duration(
      static_cast<int>(m_velocity / -m_deceleration * 1000));
  animation.Duration(duration);

  const auto compositor = winrt::Window::Current().Compositor();
  animation.SetScalarParameter(
      s_velocityParameterName, static_cast<float>(m_velocity));
  animation.SetScalarParameter(
      s_decelerationParameterName, static_cast<float>(m_deceleration));
  animation.SetScalarParameter(
      s_durationName, static_cast<float>(m_velocity / -m_deceleration) * 1000);
  // Offset = (Velocity*time) + (0.5*Acceleration*Time^2)
  animation.InsertExpressionKeyFrame(
      1.0f,
      static_cast<winrt::hstring>(L"(") + s_durationName + L" * " +
          s_velocityParameterName + L") + (0.5 * " +
          s_decelerationParameterName + L" * " + s_durationName + L" * " +
          s_durationName + L")",
      compositor.CreateCubicBezierEasingFunction({0, 1}, {0, 1}));
  animation.IterationCount(static_cast<int32_t>(m_iterations));
  animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);

  return std::make_tuple(animation, scopedBatch);
}
} // namespace uwp
} // namespace react
