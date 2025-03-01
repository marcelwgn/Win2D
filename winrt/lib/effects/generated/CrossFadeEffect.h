// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas;

    class CrossFadeEffect : public RuntimeClass<
        ICrossFadeEffect,
        MixIn<CrossFadeEffect, CanvasEffect>>,
        public CanvasEffect
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_CrossFadeEffect, BaseTrust);

    public:
        CrossFadeEffect(ICanvasDevice* device = nullptr, ID2D1Effect* effect = nullptr);

        static IID const& EffectId() { return CLSID_D2D1CrossFade; }

        EFFECT_PROPERTY(CrossFade, float);
        EFFECT_PROPERTY(Source2, IGraphicsEffectSource*);
        EFFECT_PROPERTY(Source1, IGraphicsEffectSource*);

        EFFECT_PROPERTY_MAPPING();
    };

    class CrossFadeEffectFactory
        : public AgileActivationFactory<ICrossFadeEffectStatics>
        , private LifespanTracker<CrossFadeEffectFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Effects_CrossFadeEffect, BaseTrust);

    public:
        IFACEMETHODIMP ActivateInstance(IInspectable**) override;
        IFACEMETHOD(get_IsSupported)(boolean* value) override;
    };
}}}}}
