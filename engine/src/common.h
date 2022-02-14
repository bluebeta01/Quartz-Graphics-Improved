#pragma once
#include <wrl.h>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

//These will define what percentage of the available threads are
//used by different subsystem of the engine
#define ASSET_THREADS 0.90f
#define RENDER_THREADS 0.10f