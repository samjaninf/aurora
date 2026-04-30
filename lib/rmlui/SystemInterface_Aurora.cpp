#include "SystemInterface_Aurora.h"

#include <SDL3/SDL_keyboard.h>
#include <algorithm>

#include "../logging.hpp"
#include "../window.hpp"

namespace aurora::rmlui {
static Module Log("aurora::rmlui");

SystemInterface_Aurora::SystemInterface_Aurora() { SetWindow(window::get_sdl_window()); }

void SystemInterface_Aurora::ActivateKeyboard(Rml::Vector2f caret_position, float line_height) {
  SDL_Window* sdlWindow = window::get_sdl_window();
  if (sdlWindow == nullptr) {
    return;
  }

  const AuroraWindowSize size = window::get_window_size();
  if (size.native_fb_width == 0 || size.native_fb_height == 0 || size.width == 0 || size.height == 0) {
    return;
  }

  const float scaleX = static_cast<float>(size.width) / static_cast<float>(size.native_fb_width);
  const float scaleY = static_cast<float>(size.height) / static_cast<float>(size.native_fb_height);
  const float inputLineHeight = std::max(line_height * scaleY, 1.0f);
  const SDL_Rect rect = {
      .x = static_cast<int>(std::lround(caret_position.x * scaleX)),
      .y = static_cast<int>(std::lround((caret_position.y * scaleY) + inputLineHeight)),
      .w = 1,
      .h = static_cast<int>(std::lround(inputLineHeight)),
  };

  SDL_SetTextInputArea(sdlWindow, &rect, 0);
  if (!SDL_TextInputActive(sdlWindow)) {
    SDL_StartTextInput(sdlWindow);
  }
}

void SystemInterface_Aurora::DeactivateKeyboard() {
  SDL_Window* sdlWindow = window::get_sdl_window();
  if (sdlWindow != nullptr && SDL_TextInputActive(sdlWindow)) {
    SDL_StopTextInput(sdlWindow);
  }
}

bool SystemInterface_Aurora::LogMessage(Rml::Log::Type type, const Rml::String& message) {
  switch (type) {
  case Rml::Log::Type::LT_ERROR:
    Log.error("{}", message);
    return false;
  case Rml::Log::Type::LT_ASSERT:
    Log.fatal("{}", message);
    return false;
  case Rml::Log::Type::LT_WARNING:
    Log.warn("{}", message);
    return true;
  case Rml::Log::Type::LT_INFO:
    Log.info("{}", message);
    return true;
  case Rml::Log::Type::LT_DEBUG:
    Log.debug("{}", message);
    return true;
  default:
    Log.info("{}", message);
    return true;
  }
}
} // namespace aurora::rmlui
