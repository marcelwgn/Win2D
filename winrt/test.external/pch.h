// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "targetver.h"

#include <memory>
#include <functional>
#include <iterator>

#include <wrl.h>
#include <strsafe.h>
#include <CppUnitTest.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <d2d1_2.h>
#include <d2d1_3.h>

#include <dwrite_2.h>
#include <dwrite_3.h>

#include <DirectXMath.h>
#include <Combaseapi.h>
#include <wincodec.h>

#include <ErrorHandling.h>
#include <Utilities.h>
#include <Constants.h>
#include <ComArray.h>
#include <collection.h>

#undef GetCurrentTime // Fixes a conflict in a WinRT header (caused by Win32 APIs from Windows.h)
#undef GetGlyphIndices
#include <windows.foundation.h>

#include <windowsnumerics.h>

#include <Microsoft.Graphics.Canvas.native.h>

// Generated from local IDLs
#include <Microsoft.Graphics.Canvas.h>

// Pick up the inbox or local WinRT DirectX types as appropriate
#include <UapApis.h>

#pragma warning(disable: 4100)  // "unreferenced formal parameter"
#pragma warning(disable: 4351)  // "elements of {array} will be default initialized"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Microsoft::WRL;

#include "Helpers.h"
#include "DebugLayer.h"
#include "MockDxgiDevice.h"
#include "MockDxgiSurface.h"
