#pragma once

#include "platform.h"

#include "NeuronBase.h"
#include "SynapseBase.h"
#include <vector>
#include <atomic>


#ifndef CompilingNeuronWrapper
#  if NEURO_USE_TBB
#    include <tbb/concurrent_queue.h>
     namespace NeuronParallel = tbb;
#  else
#    include <concurrent_queue.h>
     namespace NeuronParallel = concurrency;
#  endif
#endif
#include <string>
#define NeuronWrapper _

namespace NeuronEngine
{
	class NeuronArrayBase
	{
	public:
		NEURO_EXPORT NeuronArrayBase();
		NEURO_EXPORT ~NeuronArrayBase();
		NEURO_EXPORT void Initialize(int theSize, NeuronBase::modelType t = NeuronBase::modelType::Std);
		NEURO_EXPORT NeuronBase* GetNeuron(int i);
		NEURO_EXPORT int GetArraySize();
		NEURO_EXPORT long long GetTotalSynapseCount();
		NEURO_EXPORT long GetNeuronsInUseCount();
		NEURO_EXPORT void Fire();
		NEURO_EXPORT long long GetGeneration();
		NEURO_EXPORT void SetGeneration(long long i);
		NEURO_EXPORT int GetFiredCount();
		NEURO_EXPORT int GetThreadCount();
		NEURO_EXPORT void SetThreadCount(int i);
		NEURO_EXPORT void GetBounds(int taskID, int& start, int& end);
		NEURO_EXPORT std::string GetRemoteFiringString();
		NEURO_EXPORT SynapseBase GetRemoteFiringSynapse();
		NEURO_EXPORT static int GetRefractoryDelay();
		NEURO_EXPORT static void SetRefractoryDelay(int i);


	private:
		int arraySize = 0;
		int threadCount = 124;
		std::vector<NeuronBase> neuronArray;
		std::atomic<long> firedCount = 0;
		long long cycle = 0;
		static int refractoryDelay;

		static std::vector<unsigned long long> fireList1;
		//static std::vector<std::atomic<unsigned long long>> fireList1;

		static std::vector<unsigned long long> fireList2;

	private:
		NEURO_NOINLINE void ProcessNeurons1(int taskID); //these are noinlined so the profiler makes more sense
		NEURO_NOINLINE void ProcessNeurons2(int taskID);
		NEURO_NOINLINE void ProcessNeurons3(int taskID);
		void GetBounds64(int taskID, int& start, int& end);

	public:
		static void AddNeuronToFireList1(int id);
		static bool clearFireListNeeded;
		static void ClearFireLists();

	public:
#ifndef CompilingNeuronWrapper
		static NeuronParallel::concurrent_queue<SynapseBase> remoteQueue;
		static NeuronParallel::concurrent_queue<NeuronBase *> fire2Queue;
#endif // !NeuronWrapper
	};
}