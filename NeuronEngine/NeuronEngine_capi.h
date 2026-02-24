#pragma once

// Plain C export layer so any language / runtime can call the engine
// via P/Invoke / FFI without requiring C++/CLI.
//
// Convention:
//   - void* handle  is always a NeuronEngine::NeuronArrayBase*
//   - Functions that return variable-length data write into a caller-
//     supplied buffer and return the number of bytes written.
//     Return value -1 means the buffer was too small.
//   - Synapse wire format: 12 bytes each = int32 target | float weight | int32 model
//   - Labels are exchanged as UTF-8 (null-terminated).

#include "platform.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ── Lifecycle ────────────────────────────────────────────────────────────────
NEURO_EXPORT void*     NeuronArray_Create(void);
NEURO_EXPORT void      NeuronArray_Destroy(void* handle);
NEURO_EXPORT void      NeuronArray_Initialize(void* handle, int size);

// ── Engine control ───────────────────────────────────────────────────────────
NEURO_EXPORT void      NeuronArray_Fire(void* handle);
NEURO_EXPORT int       NeuronArray_GetArraySize(void* handle);
NEURO_EXPORT int       NeuronArray_GetThreadCount(void* handle);
NEURO_EXPORT void      NeuronArray_SetThreadCount(void* handle, int i);
NEURO_EXPORT int       NeuronArray_GetRefractoryDelay(void* handle);
NEURO_EXPORT void      NeuronArray_SetRefractoryDelay(void* handle, int i);
NEURO_EXPORT long long NeuronArray_GetGeneration(void* handle);
NEURO_EXPORT void      NeuronArray_SetGeneration(void* handle, long long i);
NEURO_EXPORT int       NeuronArray_GetFiredCount(void* handle);
NEURO_EXPORT long long NeuronArray_GetTotalSynapses(void* handle);
NEURO_EXPORT long      NeuronArray_GetTotalNeuronsInUse(void* handle);

// ── Per-neuron accessors ─────────────────────────────────────────────────────
NEURO_EXPORT float     NeuronArray_GetNeuronLastCharge(void* handle, int i);
NEURO_EXPORT void      NeuronArray_SetNeuronLastCharge(void* handle, int i, float v);
NEURO_EXPORT void      NeuronArray_SetNeuronCurrentCharge(void* handle, int i, float v);
NEURO_EXPORT void      NeuronArray_AddToNeuronCurrentCharge(void* handle, int i, float v);
NEURO_EXPORT int       NeuronArray_GetNeuronInUse(void* handle, int i);   // 0 or 1
NEURO_EXPORT int       NeuronArray_GetNeuronModel(void* handle, int i);
NEURO_EXPORT void      NeuronArray_SetNeuronModel(void* handle, int i, int model);
NEURO_EXPORT float     NeuronArray_GetNeuronLeakRate(void* handle, int i);
NEURO_EXPORT void      NeuronArray_SetNeuronLeakRate(void* handle, int i, float v);
NEURO_EXPORT int       NeuronArray_GetNeuronAxonDelay(void* handle, int i);
NEURO_EXPORT void      NeuronArray_SetNeuronAxonDelay(void* handle, int i, int v);
NEURO_EXPORT long long NeuronArray_GetNeuronLastFired(void* handle, int i);

// Label as UTF-8.  Returns bytes written (excl. null), -1 if buf too small.
NEURO_EXPORT int       NeuronArray_GetNeuronLabel(void* handle, int i, char* buf, int bufLen);
NEURO_EXPORT void      NeuronArray_SetNeuronLabel(void* handle, int i, const char* utf8);

// ── Synapse byte-array accessors ─────────────────────────────────────────────
// Each synapse is 12 bytes: int32 target, float weight, int32 model.
// Returns bytes written, -1 if buf too small (use 12 * neuron_count as upper bound).
NEURO_EXPORT int       NeuronArray_GetSynapses(void* handle, int src, uint8_t* buf, int bufLen);
NEURO_EXPORT int       NeuronArray_GetSynapsesFrom(void* handle, int src, uint8_t* buf, int bufLen);

// ── Remote firing ────────────────────────────────────────────────────────────
// GetRemoteFiring: fills buf with a null-terminated ASCII string.
NEURO_EXPORT int       NeuronArray_GetRemoteFiring(void* handle, char* buf, int bufLen);
// GetRemoteFiringSynapses: same 12-byte-per-synapse format.
NEURO_EXPORT int       NeuronArray_GetRemoteFiringSynapses(void* handle, uint8_t* buf, int bufLen);

// ── Synapse mutation ─────────────────────────────────────────────────────────
NEURO_EXPORT void      NeuronArray_AddSynapse(void* handle, int src, int dest, float weight, int model, int noBackPtr);
NEURO_EXPORT void      NeuronArray_AddSynapseFrom(void* handle, int src, int dest, float weight, int model);
NEURO_EXPORT void      NeuronArray_DeleteSynapse(void* handle, int src, int dest);
NEURO_EXPORT void      NeuronArray_DeleteSynapseFrom(void* handle, int src, int dest);

#ifdef __cplusplus
}
#endif
