#include "pch.h"

#include "NeuronEngine_capi.h"
#include "NeuronArrayBase.h"
#include "NeuronBase.h"
#include "SynapseBase.h"

#include <cstring>
#include <string>
#include <vector>

using namespace NeuronEngine;

// ── helpers ──────────────────────────────────────────────────────────────────

// 12-byte synapse wire struct  (matches C++/CLI NeuronEngineWrapper::Synapse)
#pragma pack(push, 1)
struct WireSynapse { int target; float weight; int model; };
#pragma pack(pop)

// Serialise a vector of SynapseBase into buf. Returns bytes written or -1.
static int SerializeSynapses(const std::vector<SynapseBase>& vec,
                             uint8_t* buf, int bufLen,
                             int firstNeuronOffset)
{
    int needed = (int)(vec.size() * sizeof(WireSynapse));
    if (needed > bufLen) return -1;

    uint8_t* p = buf;
    for (const SynapseBase& s : vec)
    {
        WireSynapse ws;
        ws.weight = s.GetWeight();
        ws.model  = (int)s.GetModel();

        NeuronBase* t = s.GetTarget();
        if (t == nullptr || ((long long)t >> 63) != 0)
            ws.target = (int)(long long)t;   // remote / negative id
        else
            ws.target = t->GetId();          // local neuron id

        memcpy(p, &ws, sizeof(WireSynapse));
        p += sizeof(WireSynapse);
    }
    return needed;
}

// Simple wchar_t -> UTF-8 for ASCII labels (covers the common case).
// Full-range code points are encoded correctly for the BMP (U+0000..U+FFFF).
static int WcharToUtf8(const wchar_t* src, char* dst, int dstLen)
{
    if (!src) { if (dstLen > 0) dst[0] = '\0'; return 0; }
    int written = 0;
    for (; *src; ++src)
    {
        unsigned int cp = (unsigned int)*src;
        if (cp < 0x80)
        {
            if (written + 1 >= dstLen) return -1;
            dst[written++] = (char)cp;
        }
        else if (cp < 0x800)
        {
            if (written + 2 >= dstLen) return -1;
            dst[written++] = (char)(0xC0 | (cp >> 6));
            dst[written++] = (char)(0x80 | (cp & 0x3F));
        }
        else
        {
            if (written + 3 >= dstLen) return -1;
            dst[written++] = (char)(0xE0 | (cp >> 12));
            dst[written++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            dst[written++] = (char)(0x80 | (cp & 0x3F));
        }
    }
    dst[written] = '\0';
    return written;
}

// UTF-8 -> wchar_t  (BMP only, sufficient for neuron labels)
static std::wstring Utf8ToWchar(const char* src)
{
    std::wstring out;
    while (*src)
    {
        unsigned char c = (unsigned char)*src;
        unsigned int cp = 0;
        if (c < 0x80)       { cp = c; src += 1; }
        else if (c < 0xE0)  { cp = (c & 0x1F) << 6  | (src[1] & 0x3F); src += 2; }
        else                { cp = (c & 0x0F) << 12 | ((src[1] & 0x3F) << 6) | (src[2] & 0x3F); src += 3; }
        out += (wchar_t)cp;
    }
    return out;
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────
extern "C"
{

void* NeuronArray_Create(void)
{
    return new NeuronArrayBase();
}

void NeuronArray_Destroy(void* handle)
{
    delete static_cast<NeuronArrayBase*>(handle);
}

void NeuronArray_Initialize(void* handle, int size)
{
    static_cast<NeuronArrayBase*>(handle)->Initialize(size);
}

// ── Engine control ────────────────────────────────────────────────────────────

void NeuronArray_Fire(void* handle)
{
    static_cast<NeuronArrayBase*>(handle)->Fire();
}

int NeuronArray_GetArraySize(void* handle)
{
    return static_cast<NeuronArrayBase*>(handle)->GetArraySize();
}

int NeuronArray_GetThreadCount(void* handle)
{
    return static_cast<NeuronArrayBase*>(handle)->GetThreadCount();
}

void NeuronArray_SetThreadCount(void* handle, int i)
{
    static_cast<NeuronArrayBase*>(handle)->SetThreadCount(i);
}

int NeuronArray_GetRefractoryDelay(void* handle)
{
    return NeuronArrayBase::GetRefractoryDelay();
}

void NeuronArray_SetRefractoryDelay(void* handle, int i)
{
    NeuronArrayBase::SetRefractoryDelay(i);
}

long long NeuronArray_GetGeneration(void* handle)
{
    return static_cast<NeuronArrayBase*>(handle)->GetGeneration();
}

void NeuronArray_SetGeneration(void* handle, long long i)
{
    static_cast<NeuronArrayBase*>(handle)->SetGeneration(i);
}

int NeuronArray_GetFiredCount(void* handle)
{
    return static_cast<NeuronArrayBase*>(handle)->GetFiredCount();
}

long long NeuronArray_GetTotalSynapses(void* handle)
{
    return static_cast<NeuronArrayBase*>(handle)->GetTotalSynapseCount();
}

long NeuronArray_GetTotalNeuronsInUse(void* handle)
{
    return static_cast<NeuronArrayBase*>(handle)->GetNeuronsInUseCount();
}

// ── Per-neuron accessors ──────────────────────────────────────────────────────

float NeuronArray_GetNeuronLastCharge(void* handle, int i)
{
    return static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->GetLastCharge();
}

void NeuronArray_SetNeuronLastCharge(void* handle, int i, float v)
{
    static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->SetLastCharge(v);
}

void NeuronArray_SetNeuronCurrentCharge(void* handle, int i, float v)
{
    static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->SetCurrentCharge(v);
}

void NeuronArray_AddToNeuronCurrentCharge(void* handle, int i, float v)
{
    static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->AddToCurrentValue(v);
}

int NeuronArray_GetNeuronInUse(void* handle, int i)
{
    return static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->GetInUse() ? 1 : 0;
}

int NeuronArray_GetNeuronModel(void* handle, int i)
{
    return (int)static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->GetModel();
}

void NeuronArray_SetNeuronModel(void* handle, int i, int model)
{
    static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->SetModel((NeuronBase::modelType)model);
}

float NeuronArray_GetNeuronLeakRate(void* handle, int i)
{
    return static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->GetLeakRate();
}

void NeuronArray_SetNeuronLeakRate(void* handle, int i, float v)
{
    static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->SetLeakRate(v);
}

int NeuronArray_GetNeuronAxonDelay(void* handle, int i)
{
    return static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->GetAxonDelay();
}

void NeuronArray_SetNeuronAxonDelay(void* handle, int i, int v)
{
    static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->SetAxonDelay(v);
}

long long NeuronArray_GetNeuronLastFired(void* handle, int i)
{
    return static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->GetLastFired();
}

int NeuronArray_GetNeuronLabel(void* handle, int i, char* buf, int bufLen)
{
    wchar_t* label = static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->GetLabel();
    if (!label) { if (bufLen > 0) buf[0] = '\0'; return 0; }
    return WcharToUtf8(label, buf, bufLen);
}

void NeuronArray_SetNeuronLabel(void* handle, int i, const char* utf8)
{
    std::wstring w = Utf8ToWchar(utf8);
    static_cast<NeuronArrayBase*>(handle)->GetNeuron(i)->SetLabel(w.c_str());
}

// ── Synapse byte-array accessors ──────────────────────────────────────────────

int NeuronArray_GetSynapses(void* handle, int src, uint8_t* buf, int bufLen)
{
    NeuronBase* n = static_cast<NeuronArrayBase*>(handle)->GetNeuron(src);
    n->GetLock();
    std::vector<SynapseBase> vec = n->GetSynapses();
    n->ClearLock();
    return SerializeSynapses(vec, buf, bufLen, 0);
}

int NeuronArray_GetSynapsesFrom(void* handle, int src, uint8_t* buf, int bufLen)
{
    NeuronBase* n = static_cast<NeuronArrayBase*>(handle)->GetNeuron(src);
    n->GetLock();
    std::vector<SynapseBase> vec = n->GetSynapsesFrom();
    n->ClearLock();
    return SerializeSynapses(vec, buf, bufLen, 0);
}

// ── Remote firing ─────────────────────────────────────────────────────────────

int NeuronArray_GetRemoteFiring(void* handle, char* buf, int bufLen)
{
    std::string s = static_cast<NeuronArrayBase*>(handle)->GetRemoteFiringString();
    int len = (int)s.size();
    if (len >= bufLen) return -1;
    memcpy(buf, s.c_str(), len + 1);
    return len;
}

int NeuronArray_GetRemoteFiringSynapses(void* handle, uint8_t* buf, int bufLen)
{
    std::vector<SynapseBase> vec;
    SynapseBase s = static_cast<NeuronArrayBase*>(handle)->GetRemoteFiringSynapse();
    while (s.GetTarget() != nullptr)
    {
        vec.push_back(s);
        s = static_cast<NeuronArrayBase*>(handle)->GetRemoteFiringSynapse();
    }
    return SerializeSynapses(vec, buf, bufLen, 0);
}

// ── Synapse mutation ──────────────────────────────────────────────────────────

void NeuronArray_AddSynapse(void* handle, int src, int dest, float weight, int model, int noBackPtr)
{
    NeuronArrayBase* na = static_cast<NeuronArrayBase*>(handle);
    NeuronBase* n = na->GetNeuron(src);
    NeuronBase* target = (dest < 0)
        ? (NeuronBase*)(long long)dest
        : na->GetNeuron(dest);
    n->AddSynapse(target, weight, (SynapseBase::modelType)model, noBackPtr != 0);
}

void NeuronArray_AddSynapseFrom(void* handle, int src, int dest, float weight, int model)
{
    NeuronArrayBase* na = static_cast<NeuronArrayBase*>(handle);
    NeuronBase* n = na->GetNeuron(dest);
    NeuronBase* source = (src < 0)
        ? (NeuronBase*)(long long)src
        : na->GetNeuron(src);
    n->AddSynapseFrom(source, weight, (SynapseBase::modelType)model);
}

void NeuronArray_DeleteSynapse(void* handle, int src, int dest)
{
    NeuronArrayBase* na = static_cast<NeuronArrayBase*>(handle);
    if (src < 0) return;
    NeuronBase* n = na->GetNeuron(src);
    NeuronBase* target = (dest < 0) ? (NeuronBase*)(long long)dest : na->GetNeuron(dest);
    n->DeleteSynapse(target);
}

void NeuronArray_DeleteSynapseFrom(void* handle, int src, int dest)
{
    NeuronArrayBase* na = static_cast<NeuronArrayBase*>(handle);
    if (dest < 0) return;
    NeuronBase* n = na->GetNeuron(dest);
    NeuronBase* source = (src < 0) ? (NeuronBase*)(long long)src : na->GetNeuron(src);
    n->DeleteSynapse(source);
}

} // extern "C"
