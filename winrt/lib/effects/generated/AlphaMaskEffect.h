// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class AlphaMaskEffect : public RuntimeClass<
        IAlphaMaskEffect,
        MixIn<AlphaMaskEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_AlphaMaskEffect, BaseTrust);

    public:
        AlphaMaskEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1AlphaMask; }

        EFFECT_PROPERTY(Source, IGraphicsEffectSource*);
        EFFECT_PROPERTY(AlphaMask, IGraphicsEffectSource*);
    };

    class AlphaMaskEffectFactory
        : public AgileActivationFactory<IAlphaMaskEffectStatics>
        , private LifespanTracker<AlphaMaskEffectFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_AlphaMaskEffect, BaseTrust);

    public:
        IFACEMETHODIMP ActivateInstance(IInspectable**) override;
        IFACEMETHOD(get_IsSupported)(boolean* value) override;
    };
}}}}}
