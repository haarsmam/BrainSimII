#pragma once

// ── Export / visibility macros ────────────────────────────────────────────────
#ifdef _WIN32
#  define NEURO_EXPORT   __declspec(dllexport)
#  define NEURO_NOINLINE __declspec(noinline)
#else
#  define NEURO_EXPORT   __attribute__((visibility("default")))
#  define NEURO_NOINLINE __attribute__((noinline))
#endif

// ── Parallel backend selector ─────────────────────────────────────────────────
// On Windows we use Microsoft PPL (ships with MSVC, zero extra dependency).
// On every other platform we use Intel oneTBB.
#ifdef _WIN32
#  define NEURO_USE_TBB 0
#else
#  define NEURO_USE_TBB 1
#endif
