// Cross-platform P/Invoke replacement for the C++/CLI NeuronEngineWrapper.vcxproj.
// Exposes the same namespace + class as the C++/CLI assembly so Program.cs is unchanged.
//
// The native library is loaded as:
//   Windows  → NeuronEngine.dll
//   Linux    → libNeuronEngine.so
//   macOS    → libNeuronEngine.dylib
// All three are resolved automatically from [DllImport("NeuronEngine")].

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace NeuronEngine.CLI
{
    public class NeuronArrayBase : IDisposable
    {
        // ── Native imports ───────────────────────────────────────────────────
        private const string Lib = "NeuronEngine";

        [DllImport(Lib)] static extern IntPtr NeuronArray_Create();
        [DllImport(Lib)] static extern void   NeuronArray_Destroy(IntPtr h);
        [DllImport(Lib)] static extern void   NeuronArray_Initialize(IntPtr h, int size);

        [DllImport(Lib)] static extern void   NeuronArray_Fire(IntPtr h);
        [DllImport(Lib)] static extern int    NeuronArray_GetArraySize(IntPtr h);
        [DllImport(Lib)] static extern int    NeuronArray_GetThreadCount(IntPtr h);
        [DllImport(Lib)] static extern void   NeuronArray_SetThreadCount(IntPtr h, int i);
        [DllImport(Lib)] static extern int    NeuronArray_GetRefractoryDelay(IntPtr h);
        [DllImport(Lib)] static extern void   NeuronArray_SetRefractoryDelay(IntPtr h, int i);
        [DllImport(Lib)] static extern long   NeuronArray_GetGeneration(IntPtr h);
        [DllImport(Lib)] static extern void   NeuronArray_SetGeneration(IntPtr h, long i);
        [DllImport(Lib)] static extern int    NeuronArray_GetFiredCount(IntPtr h);
        [DllImport(Lib)] static extern long   NeuronArray_GetTotalSynapses(IntPtr h);
        [DllImport(Lib)] static extern long   NeuronArray_GetTotalNeuronsInUse(IntPtr h);

        [DllImport(Lib)] static extern float  NeuronArray_GetNeuronLastCharge(IntPtr h, int i);
        [DllImport(Lib)] static extern void   NeuronArray_SetNeuronLastCharge(IntPtr h, int i, float v);
        [DllImport(Lib)] static extern void   NeuronArray_SetNeuronCurrentCharge(IntPtr h, int i, float v);
        [DllImport(Lib)] static extern void   NeuronArray_AddToNeuronCurrentCharge(IntPtr h, int i, float v);
        [DllImport(Lib)] static extern int    NeuronArray_GetNeuronInUse(IntPtr h, int i);
        [DllImport(Lib)] static extern int    NeuronArray_GetNeuronModel(IntPtr h, int i);
        [DllImport(Lib)] static extern void   NeuronArray_SetNeuronModel(IntPtr h, int i, int model);
        [DllImport(Lib)] static extern float  NeuronArray_GetNeuronLeakRate(IntPtr h, int i);
        [DllImport(Lib)] static extern void   NeuronArray_SetNeuronLeakRate(IntPtr h, int i, float v);
        [DllImport(Lib)] static extern int    NeuronArray_GetNeuronAxonDelay(IntPtr h, int i);
        [DllImport(Lib)] static extern void   NeuronArray_SetNeuronAxonDelay(IntPtr h, int i, int v);
        [DllImport(Lib)] static extern long   NeuronArray_GetNeuronLastFired(IntPtr h, int i);
        [DllImport(Lib)] static extern int    NeuronArray_GetNeuronLabel(IntPtr h, int i, byte[] buf, int bufLen);
        [DllImport(Lib)] static extern void   NeuronArray_SetNeuronLabel(IntPtr h, int i,
                                                  [MarshalAs(UnmanagedType.LPUTF8Str)] string utf8);

        [DllImport(Lib)] static extern int    NeuronArray_GetSynapses(IntPtr h, int src, byte[] buf, int bufLen);
        [DllImport(Lib)] static extern int    NeuronArray_GetSynapsesFrom(IntPtr h, int src, byte[] buf, int bufLen);
        [DllImport(Lib)] static extern int    NeuronArray_GetRemoteFiring(IntPtr h, byte[] buf, int bufLen);
        [DllImport(Lib)] static extern int    NeuronArray_GetRemoteFiringSynapses(IntPtr h, byte[] buf, int bufLen);

        [DllImport(Lib)] static extern void   NeuronArray_AddSynapse(IntPtr h, int src, int dest,
                                                  float weight, int model, int noBackPtr);
        [DllImport(Lib)] static extern void   NeuronArray_AddSynapseFrom(IntPtr h, int src, int dest,
                                                  float weight, int model);
        [DllImport(Lib)] static extern void   NeuronArray_DeleteSynapse(IntPtr h, int src, int dest);
        [DllImport(Lib)] static extern void   NeuronArray_DeleteSynapseFrom(IntPtr h, int src, int dest);

        // ── Handle ───────────────────────────────────────────────────────────
        private IntPtr _handle;

        public NeuronArrayBase() => _handle = NeuronArray_Create();
        ~NeuronArrayBase() => Dispose(false);
        public void Dispose() { Dispose(true); GC.SuppressFinalize(this); }
        private void Dispose(bool _) {
            if (_handle != IntPtr.Zero) { NeuronArray_Destroy(_handle); _handle = IntPtr.Zero; }
        }

        // ── Engine control ───────────────────────────────────────────────────
        public void Initialize(int size)          => NeuronArray_Initialize(_handle, size);
        public void Fire()                         => NeuronArray_Fire(_handle);
        public int  GetArraySize()                 => NeuronArray_GetArraySize(_handle);
        public int  GetThreadCount()               => NeuronArray_GetThreadCount(_handle);
        public void SetThreadCount(int i)          => NeuronArray_SetThreadCount(_handle, i);
        public int  GetRefractoryDelay()           => NeuronArray_GetRefractoryDelay(_handle);
        public void SetRefractoryDelay(int i)      => NeuronArray_SetRefractoryDelay(_handle, i);
        public long GetGeneration()                => NeuronArray_GetGeneration(_handle);
        public void SetGeneration(long i)          => NeuronArray_SetGeneration(_handle, i);
        public int  GetFiredCount()                => NeuronArray_GetFiredCount(_handle);
        public long GetTotalSynapses()             => NeuronArray_GetTotalSynapses(_handle);
        public long GetTotalNeuronsInUse()         => NeuronArray_GetTotalNeuronsInUse(_handle);

        // ── Per-neuron accessors ─────────────────────────────────────────────
        public float GetNeuronLastCharge(int i)        => NeuronArray_GetNeuronLastCharge(_handle, i);
        public void  SetNeuronLastCharge(int i, float v) => NeuronArray_SetNeuronLastCharge(_handle, i, v);
        public void  SetNeuronCurrentCharge(int i, float v) => NeuronArray_SetNeuronCurrentCharge(_handle, i, v);
        public void  AddToNeuronCurrentCharge(int i, float v) => NeuronArray_AddToNeuronCurrentCharge(_handle, i, v);
        public bool  GetNeuronInUse(int i)             => NeuronArray_GetNeuronInUse(_handle, i) != 0;
        public int   GetNeuronModel(int i)             => NeuronArray_GetNeuronModel(_handle, i);
        public void  SetNeuronModel(int i, int model)  => NeuronArray_SetNeuronModel(_handle, i, model);
        public float GetNeuronLeakRate(int i)          => NeuronArray_GetNeuronLeakRate(_handle, i);
        public void  SetNeuronLeakRate(int i, float v) => NeuronArray_SetNeuronLeakRate(_handle, i, v);
        public int   GetNeuronAxonDelay(int i)         => NeuronArray_GetNeuronAxonDelay(_handle, i);
        public void  SetNeuronAxonDelay(int i, int v)  => NeuronArray_SetNeuronAxonDelay(_handle, i, v);
        public long  GetNeuronLastFired(int i)         => NeuronArray_GetNeuronLastFired(_handle, i);

        public string GetNeuronLabel(int i)
        {
            byte[] buf = new byte[256];
            int n = NeuronArray_GetNeuronLabel(_handle, i, buf, buf.Length);
            return n <= 0 ? "" : Encoding.UTF8.GetString(buf, 0, n);
        }
        public void SetNeuronLabel(int i, string label) => NeuronArray_SetNeuronLabel(_handle, i, label);

        // ── Synapse byte-array accessors ─────────────────────────────────────
        // Wire format: 12 bytes per synapse = int32 target | float weight | int32 model
        // (identical to the C++/CLI wrapper's byte[] return values)

        private byte[] GetSynapseBytes(int src, bool from)
        {
            byte[] buf = new byte[12 * 64]; // initial capacity: 64 synapses
            while (true)
            {
                int n = from
                    ? NeuronArray_GetSynapsesFrom(_handle, src, buf, buf.Length)
                    : NeuronArray_GetSynapses(_handle, src, buf, buf.Length);
                if (n == -1) { buf = new byte[buf.Length * 2]; continue; }
                byte[] result = new byte[n];
                Buffer.BlockCopy(buf, 0, result, 0, n);
                return result;
            }
        }

        public byte[] GetSynapses(int src)     => GetSynapseBytes(src, false);
        public byte[] GetSynapsesFrom(int src) => GetSynapseBytes(src, true);

        public string GetRemoteFiring()
        {
            byte[] buf = new byte[4096];
            while (true)
            {
                int n = NeuronArray_GetRemoteFiring(_handle, buf, buf.Length);
                if (n == -1) { buf = new byte[buf.Length * 2]; continue; }
                return Encoding.ASCII.GetString(buf, 0, n);
            }
        }

        public byte[] GetRemoteFiringSynapses()
        {
            byte[] buf = new byte[12 * 256];
            while (true)
            {
                int n = NeuronArray_GetRemoteFiringSynapses(_handle, buf, buf.Length);
                if (n == -1) { buf = new byte[buf.Length * 2]; continue; }
                byte[] result = new byte[n];
                Buffer.BlockCopy(buf, 0, result, 0, n);
                return result;
            }
        }

        // ── Synapse mutation ─────────────────────────────────────────────────
        public void AddSynapse(int src, int dest, float weight, int model, bool noBackPtr)
            => NeuronArray_AddSynapse(_handle, src, dest, weight, model, noBackPtr ? 1 : 0);
        public void AddSynapseFrom(int src, int dest, float weight, int model)
            => NeuronArray_AddSynapseFrom(_handle, src, dest, weight, model);
        public void DeleteSynapse(int src, int dest)
            => NeuronArray_DeleteSynapse(_handle, src, dest);
        public void DeleteSynapseFrom(int src, int dest)
            => NeuronArray_DeleteSynapseFrom(_handle, src, dest);
    }
}
