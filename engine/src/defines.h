#pragma once

#include <cstdint>

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define SIREN_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#elif __APPLE__
#define SIREN_PLATFORM_MACOS 1
#endif
#endif

#ifdef SIREN_EXPORT
// Exports
#ifdef _MSC_VER
#define SIREN_API __declspec(dllexport)
#else
#define SIREN_API __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define SIREN_API __declspec(dllimport)
#else
#define SIREN_API
#endif
#endif