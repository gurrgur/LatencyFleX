// Copyright 2021 Tatsuyuki Ishi
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>

#include <funchook.h>

#include "latencyflex_layer.h"

namespace {
funchook_t *tick_hook;
typedef void (*tick_func)(void *self);
tick_func real_tick;

void lfx_FEngineLoop_Tick(void *self) {
  lfx_WaitAndBeginFrame();

  real_tick(self);
}

void hook_init() {
  if (getenv("LFX_HOOK")) {
    real_tick = reinterpret_cast<tick_func>(std::stoul(getenv("LFX_HOOK"), nullptr, 16));
  } else {
    return;
  }

  int err;
  err = funchook_set_debug_file("_funchook_debug.log");

  tick_hook = funchook_create();
  
  err = funchook_prepare(tick_hook, (void **)&real_tick, (void *)lfx_FEngineLoop_Tick);
  if (err != 0)
  {
    std::cerr << "LatencyFleX: Error: funchook_prepare failure with err=" << err << std::endl;
    return;
  }

  err = funchook_install(tick_hook, 0);
  if (err != 0)
  { 
    std::cerr << "LatencyFleX: Error: funchook_install failure with err=" << err << std::endl;
    return;
  }
  std::cerr << "LatencyFleX: Successfully initialized hook" << std::endl;
}

class OnLoad {
public:
  OnLoad() { hook_init(); }
};

[[maybe_unused]] OnLoad on_load;
} // namespace