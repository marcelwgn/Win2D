// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasFontSet.h"
#include "CanvasFontFace.h"
#include "TextUtilities.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

//
// CanvasFontSetFactory implementation
//
IFACEMETHODIMP CanvasFontSetFactory::Create(
    IUriRuntimeClass* uri,
    ICanvasFontSet** fontSet)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(uri);
            CheckAndClearOutPointer(fontSet);

            auto customFontManager = CustomFontManager::GetInstance();

            auto fontCollection = customFontManager->GetFontCollectionFromUri(uri);

            if (!fontCollection)
                ThrowHR(E_INVALIDARG);

            ComPtr<DWriteFontSetType> fontResource;
            ThrowIfFailed(As<IDWriteFontCollection1>(fontCollection)->GetFontSet(&fontResource));

            auto newFontSet = Make<CanvasFontSet>(fontResource.Get());
            CheckMakeResult(newFontSet);

            ThrowIfFailed(newFontSet.CopyTo(fontSet));
        });
}


ComPtr<IDWriteFontSet> GetLocalFonts(ComPtr<IDWriteFontSet> const& fonts)
{
    auto factory = As<IDWriteFactory3>(CustomFontManager::GetInstance()->GetSharedFactory());

    ComPtr<IDWriteFontSetBuilder> fontSetBuilder;
    ThrowIfFailed(factory->CreateFontSetBuilder(&fontSetBuilder));

    const uint32_t fontCount = fonts->GetFontCount();
    for (uint32_t i = 0; i < fontCount; ++i)
    {
        ComPtr<IDWriteFontFaceReference> fontFaceReference;
        ThrowIfFailed(fonts->GetFontFaceReference(i, &fontFaceReference));

        if (fontFaceReference->GetLocality() == DWRITE_LOCALITY_LOCAL)
        {
            ThrowIfFailed(fontSetBuilder->AddFontFaceReference(fontFaceReference.Get()));
        }
    }

    ComPtr<IDWriteFontSet> localFonts;
    ThrowIfFailed(fontSetBuilder->CreateFontSet(&localFonts));

    return localFonts;
}

IFACEMETHODIMP CanvasFontSetFactory::GetSystemFontSet(
    ICanvasFontSet** fontSet)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(fontSet);
            
            auto factory = CustomFontManager::GetInstance()->GetSharedFactory();

            ComPtr<DWriteFontSetType> systemFonts;

            ComPtr<DWriteFontSetType> systemFontsIncludingRemoteFonts;

            ThrowIfFailed(As<IDWriteFactory3>(factory)->GetSystemFontSet(&systemFontsIncludingRemoteFonts));
            systemFonts = GetLocalFonts(systemFontsIncludingRemoteFonts);

            auto canvasFontSet = ResourceManager::GetOrCreate<ICanvasFontSet>(systemFonts.Get());

            canvasFontSet.CopyTo(fontSet);
        });
}

CanvasFontSet::CanvasFontSet(DWriteFontSetType* dwriteFontSet)
    : ResourceWrapper(dwriteFontSet)
    , m_customFontManager(CustomFontManager::GetInstance())
{
}

IFACEMETHODIMP CanvasFontSet::TryFindFontFace(ICanvasFontFace* fontFace, int* index, boolean* succeeded)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontFace);
            CheckInPointer(index);
            CheckInPointer(succeeded);

            auto& resource = GetResource();

            *index = 0;
            *succeeded = false;

            auto dwriteFontFaceReference = GetWrappedResource<IDWriteFontFaceReference>(fontFace);
            ComPtr<IDWriteFontFace3> dwriteFontFace;
            ThrowIfFailed(dwriteFontFaceReference->CreateFontFace(&dwriteFontFace));

            BOOL exists;
            uint32_t returnedIndex;
            ThrowIfFailed(resource->FindFontFace(dwriteFontFace.Get(), &returnedIndex, &exists));

            if (exists)
            {
                *index = static_cast<int>(returnedIndex);
                *succeeded = true;
            }
        });
}

IFACEMETHODIMP CanvasFontSet::get_Fonts(IVectorView<CanvasFontFace*>** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);

            auto& resource = GetResource();

            const uint32_t fontCount = resource->GetFontCount();

            auto vector = Make<Vector<CanvasFontFace*>>();

            for (uint32_t i = 0; i < fontCount; ++i)
            {
                ComPtr<IDWriteFontFaceReference> fontResource;
                ThrowIfFailed(resource->GetFontFaceReference(i, &fontResource));

                auto canvasFontFace = ResourceManager::GetOrCreate<ICanvasFontFace>(fontResource.Get());

                ThrowIfFailed(vector->Append(canvasFontFace.Get()));
            }

            ThrowIfFailed(vector->GetView(value));
        });
}

DWRITE_FONT_PROPERTY ToDWriteFontProperty(CanvasFontProperty const& value)
{
    DWRITE_FONT_PROPERTY result;
    result.propertyId = ToDWriteFontPropertyId(value.Identifier);
    result.localeName = WindowsGetStringRawBuffer(value.Locale, nullptr);
    result.propertyValue = WindowsGetStringRawBuffer(value.Value, nullptr);
    return result;
}

IFACEMETHODIMP CanvasFontSet::GetMatchingFontsFromProperties(
    UINT32 propertyCount,
    CanvasFontProperty* propertyElements,
    ICanvasFontSet** matchingFonts)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(matchingFonts);

            auto& resource = GetResource();
            
            std::vector<DWRITE_FONT_PROPERTY> dwriteFontProperties;
            dwriteFontProperties.reserve(propertyCount);
            for (uint32_t i = 0; i < propertyCount; ++i)
            {
                dwriteFontProperties.push_back(ToDWriteFontProperty(propertyElements[i]));
            }

            ComPtr<IDWriteFontSet> dwriteFontSet;
            ThrowIfFailed(resource->GetMatchingFonts(dwriteFontProperties.data(), propertyCount, &dwriteFontSet));

            auto canvasFontSet = Make<CanvasFontSet>(dwriteFontSet.Get());

            ThrowIfFailed(canvasFontSet.CopyTo(matchingFonts));
        });
}


IFACEMETHODIMP CanvasFontSet::GetMatchingFontsFromWwsFamily(
    HSTRING familyName,
    FontWeight weight,
    FontStretch stretch,
    FontStyle style,
    ICanvasFontSet** matchingFonts)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(matchingFonts);

            auto& resource = GetResource();

            ComPtr<IDWriteFontSet> dwriteFontSet;
            ThrowIfFailed(resource->GetMatchingFonts(
                WindowsGetStringRawBuffer(familyName, nullptr), 
                ToFontWeight(weight), 
                ToFontStretch(stretch), 
                ToFontStyle(style), 
                &dwriteFontSet));

            auto canvasFontSet = Make<CanvasFontSet>(dwriteFontSet.Get());
            CheckMakeResult(canvasFontSet);

            ThrowIfFailed(canvasFontSet.CopyTo(matchingFonts));
        });
}


IFACEMETHODIMP CanvasFontSet::CountFontsMatchingProperty(
    CanvasFontProperty property,
    UINT32* count)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(count);

            auto& resource = GetResource();

            auto dwriteProperty = ToDWriteFontProperty(property);
            ThrowIfFailed(resource->GetPropertyOccurrenceCount(&dwriteProperty, count));
        });
}


IFACEMETHODIMP CanvasFontSet::GetPropertyValuesFromIndex(
    UINT32 fontIndex,
    CanvasFontPropertyIdentifier propertyIdentifier,
    IMapView<HSTRING, HSTRING>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            auto& resource = GetResource();

            BOOL unused;
            ComPtr<IDWriteLocalizedStrings> dwriteLocalizedStrings;
            ThrowIfFailed(resource->GetPropertyValues(fontIndex, ToDWriteFontPropertyId(propertyIdentifier), &unused, &dwriteLocalizedStrings));

            CopyLocalizedStringsToMapView(dwriteLocalizedStrings, values);
        });
}

void CopyStringListToArray(ComPtr<IDWriteStringList> const& stringList, CanvasFontPropertyIdentifier propertyIdentifier, UINT32* valueCount, CanvasFontProperty** valueElements)
{
    const uint32_t elementCount = stringList->GetCount();

    ComArray<CanvasFontProperty> output(elementCount);

    for (uint32_t i = 0; i < elementCount; ++i)
    {
        auto name = GetTextFromLocalizedStrings(i, stringList);
        name.CopyTo(&output[i].Value);

        auto locale = GetLocaleFromLocalizedStrings(i, stringList);
        locale.CopyTo(&output[i].Locale);

        output[i].Identifier = propertyIdentifier;
    }

    output.Detach(valueCount, valueElements);
}

IFACEMETHODIMP CanvasFontSet::GetPropertyValuesFromIdentifier(
    CanvasFontPropertyIdentifier propertyIdentifier,
    HSTRING preferredLocaleNames,
    UINT32* valueCount,
    CanvasFontProperty** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            auto& resource = GetResource();

            ComPtr<IDWriteStringList> dwriteStringList;
            ThrowIfFailed(resource->GetPropertyValues(ToDWriteFontPropertyId(propertyIdentifier), WindowsGetStringRawBuffer(preferredLocaleNames, nullptr), &dwriteStringList));

            CopyStringListToArray(dwriteStringList, propertyIdentifier, valueCount, valueElements);
        });
}


IFACEMETHODIMP CanvasFontSet::GetPropertyValues(
    CanvasFontPropertyIdentifier propertyIdentifier,
    UINT32* valueCount,
    CanvasFontProperty** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            auto& resource = GetResource();

            ComPtr<IDWriteStringList> dwriteStringList;
            ThrowIfFailed(resource->GetPropertyValues(ToDWriteFontPropertyId(propertyIdentifier), &dwriteStringList));

            CopyStringListToArray(dwriteStringList, propertyIdentifier, valueCount, valueElements);
        });
}

ActivatableClassWithFactory(CanvasFontSet, CanvasFontSetFactory);
