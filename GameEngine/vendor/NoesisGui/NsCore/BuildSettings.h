////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __CORE_BUILDSETTINGS_H__
#define __CORE_BUILDSETTINGS_H__


// Give the chance to customize default values
#ifdef NS_CUSTOM_BUILD_SETTINGS_HEADER
    #include NS_CUSTOM_BUILD_SETTINGS_HEADER
#endif

// The width of the texture used to store color ramps
#ifndef RAMP_RESOLUTION
    #define RAMP_RESOLUTION 512
#endif

// The size of the LRU cache for color ramps
#ifndef RAMP_CACHE_SIZE
    #define RAMP_CACHE_SIZE 512
#endif

// PPAA extrudes the contours of the path to approximate antialiasing by using alpha blending
// PPA_EXTRUDE_IN defines the contraction, in pixels, of each vertex along the normal
#ifndef PPAA_EXTRUDE_IN
    #define PPAA_EXTRUDE_IN 0.25f
#endif

// PPAA extrudes the contours of the path to approximate antialiasing by using alpha blending
// PPAA_EXTRUDE_OUT defines how many pixels to extrude a vertex out
#ifndef PPAA_EXTRUDE_OUT
    #define PPAA_EXTRUDE_OUT 0.50f
#endif

// Horizontal lines of text are subdivided into chunks. Each chunk is composed by a fixed number of
// glyphs that can be send to GPU or rejected using its bounding box
#ifndef HLINE_CHUNK_SIZE
    #define HLINE_CHUNK_SIZE 255U
#endif

// Support for color fonts (code size ~ 5KB)
#ifndef FREETYPE_EMOJIS
    #define FREETYPE_EMOJIS 1
#endif

// Support for system fonts (code size ~ 3KB)
#ifndef USE_SYSTEM_FONTS
    #define USE_SYSTEM_FONTS 1
#endif

// Font differentiation is described in the 'WPF font selection model' document (ticket #1868).
// When this is enabled, after extracting family name, face name, style, weight and stretch from
// an OpenType font file, an algorithm resolves potential conflicts between these values.
// For example, it is common in fonts to incorrectly add weight or stretch terms in the family
// name, like 'Absolut Reduced Condensed'. When this flag is enabled the family is fixed to be
// 'Absolut Reduced'. Font differentiation is needed for strict compatibility with WPF and Blend.
// You probably can disable this if using a curated set of fonts (code size ~ 74KB)
#ifndef NS_USE_FONT_DIFFERENTIATION
    #define NS_USE_FONT_DIFFERENTIATION 1
#endif

// Text shaping is needed for complex scripts and right to left languages like Arabic. It is also
// needed for rendering Emoji sequences and for OpenType features like ligature substitution, mark
// positioning and GPOS kerning (code size ~ 200KB)
#ifndef NS_TEXT_SHAPING
    #define NS_TEXT_SHAPING 1
#endif

// Enables the implementation of Unicode Line Breaking algorithm (http://unicode.org/reports/tr14/).
// When not enabled, line breaks will only happen at spaces (' ') and hyphens ('-').
// Text shaping must be active for the Unicode line breaking algorithm (code size ~ 10KB)
#ifndef NS_UNICODE_LINE_BREAKING
    #define NS_UNICODE_LINE_BREAKING 1
#endif

// Enables touches and manipulations (code size ~ 13KB)
#ifndef TOUCH_ENABLED
    #define TOUCH_ENABLED 1
#endif

// Thread-safety is enabled by default but if you don't need it (eg. you always call into Noesis
// from the same thread) it can be disabled (code size ~ 12KB)
#ifndef NS_MULTITHREADING
    #define NS_MULTITHREADING 1
#endif

#if NS_MULTITHREADING
    #define NS_THREAD_LOCAL thread_local
#else
    #define NS_THREAD_LOCAL
#endif

// Providers can perform hot reloading of XAML resources by enabling this macro.
// This requires internal tables that are used even if XAMLs are not reloaded.
// We recommend disabling this on release builds (code size ~ 20 KB).
// This feature can also be disabled at runtime by using Noesis::GUI::DisableHotReload()
#ifndef NS_XAML_RELOAD_ENABLED
  #ifdef NS_PROFILE
    #define NS_XAML_RELOAD_ENABLED 1
  #else
    #define NS_XAML_RELOAD_ENABLED 0
  #endif
#endif

// Providers can perform hot reloading of fonts resources by enabling this macro.
// The impact in the runtime is miminal (code size ~ 5 KB)
#ifndef NS_FONT_RELOAD_ENABLED
  #ifdef NS_PROFILE
    #define NS_FONT_RELOAD_ENABLED 1
  #else
    #define NS_FONT_RELOAD_ENABLED 0
  #endif
#endif

// Providers can perform hot reloading of texture resources by enabling this macro.
// The impact in the runtime is miminal and can be always enabled if this feature is needed
// for asynchronous loading of textures (code size ~ 4 KB)
#ifndef NS_TEXTURE_RELOAD_ENABLED
  #ifdef NS_PROFILE
    #define NS_TEXTURE_RELOAD_ENABLED 1
  #else
    #define NS_TEXTURE_RELOAD_ENABLED 1
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// The macros below control what classes are registered in the factory. If your XAMLs don't
// use <Button/> for example, you can disable registering it in the factory and save a few KB.
// Note that this optimization only makes sense when compiling static libraries because when
// generating dynamic libraries, these clases have dll_export and can't be optimized out
////////////////////////////////////////////////////////////////////////////////////////////////////

// Enables Double animations (code size ~ 20KB)
#ifndef USE_DOUBLE_ANIMATIONS
    #define USE_DOUBLE_ANIMATIONS 1
#endif

// Enables Int16 animations (code size ~ 20KB)
#ifndef USE_INT16_ANIMATIONS
    #define USE_INT16_ANIMATIONS 1
#endif

// Enables Int32 animations (code size ~ 20KB)
#ifndef USE_INT32_ANIMATIONS
    #define USE_INT32_ANIMATIONS 1
#endif

// Enables Int64 animations (code size ~ 20KB)
#ifndef USE_INT64_ANIMATIONS
    #define USE_INT64_ANIMATIONS 1
#endif

// Enables Color animations (code size ~ 20KB)
#ifndef USE_COLOR_ANIMATIONS
    #define USE_COLOR_ANIMATIONS 1
#endif

// Enables Point animations (code size ~ 20KB)
#ifndef USE_POINT_ANIMATIONS
    #define USE_POINT_ANIMATIONS 1
#endif

// Enables Point3D animations (code size ~ 20KB)
#ifndef USE_POINT3D_ANIMATIONS
    #define USE_POINT3D_ANIMATIONS 1
#endif

// Enables Rect animations (code size ~ 20KB)
#ifndef USE_RECT_ANIMATIONS
    #define USE_RECT_ANIMATIONS 1
#endif

// Enables Size animations (code size ~ 20KB)
#ifndef USE_SIZE_ANIMATIONS
    #define USE_SIZE_ANIMATIONS 1
#endif

// Enables Thickness animations (code size ~ 20KB)
#ifndef USE_THICKNESS_ANIMATIONS
    #define USE_THICKNESS_ANIMATIONS 1
#endif

// Enables Boolean animations (code size ~ 10KB)
#ifndef USE_BOOLEAN_ANIMATIONS
    #define USE_BOOLEAN_ANIMATIONS 1
#endif

// Enables Object animations (code size ~ 10KB)
#ifndef USE_OBJECT_ANIMATIONS
    #define USE_OBJECT_ANIMATIONS 1
#endif

// Enables String animations (code size ~ 10KB)
#ifndef USE_STRING_ANIMATIONS
    #define USE_STRING_ANIMATIONS 1
#endif

// Enables Matrix animations (code size ~ 10KB)
#ifndef USE_MATRIX_ANIMATIONS
    #define USE_MATRIX_ANIMATIONS 1
#endif

#endif
