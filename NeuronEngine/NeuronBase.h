#pragma once

#include "platform.h"
#include <string>
#include <vector>
#include <atomic>
#include "SynapseBase.h"


namespace NeuronEngine { class SynapseBase; }
namespace NeuronEngine { class NeuronArrayBase; }

namespace NeuronEngine
{
	class NeuronBase
	{
	public:
		enum class modelType {Std,Color,FloatValue,LIF,Random,Burst,Always};

		//the ending value of a neuron 
		float lastCharge = 0;

		//an empty vector takes up memory so this is a pointer to the vector which is allocated only if needed
		std::vector<SynapseBase>* synapses = NULL;

	private:
		//the accumulating value of a neuron
		std::atomic<float> currentCharge = 0;

		modelType model = modelType::Std;
		
		float leakRate = 0.1f; //used only by LIF model
		int nextFiring = 0; //used only by Random model && continuous model
		long long lastFired = 0; //timestamp of last firing
		int id = -1; //an illegal value which will trap
		wchar_t* label = NULL;
		int axonDelay = 0;
		int axonCounter = 0;
		
		std::vector<SynapseBase>* synapsesFrom = NULL;

		//this is a roll-your-own mutex because mutex doesn't exist in CLI code and causes compile fails
		std::atomic<int> vectorLock = 0;
		//std::mutex aLock;
		

	private:
		const float  threshold = 1.0f;


	public:
		NEURO_EXPORT  NeuronBase(int ID);
		NEURO_EXPORT  ~NeuronBase();

		NEURO_EXPORT  int GetId();
		NEURO_EXPORT  modelType GetModel();
		NEURO_EXPORT  void SetModel(modelType value);
		NEURO_EXPORT  float GetLastCharge();
		NEURO_EXPORT  void SetLastCharge(float value);
		NEURO_EXPORT  float GetCurrentCharge();
		NEURO_EXPORT  void SetCurrentCharge(float value);

		NEURO_EXPORT  void AddSynapse(NeuronBase* n, float weight, SynapseBase::modelType model = SynapseBase::modelType::Fixed, bool noBackPtr = true);
		NEURO_EXPORT  void AddSynapseFrom(NeuronBase* n, float weight, SynapseBase::modelType model = SynapseBase::modelType::Fixed);
		NEURO_EXPORT  void DeleteSynapse(NeuronBase* n);
		NEURO_EXPORT  void GetLock();
		NEURO_EXPORT  void ClearLock();
		NEURO_EXPORT  std::vector<SynapseBase> GetSynapses();
		NEURO_EXPORT  std::vector<SynapseBase> GetSynapsesFrom();
		NEURO_EXPORT  int GetSynapseCount();

		NEURO_EXPORT  bool GetInUse();
		NEURO_EXPORT  wchar_t* GetLabel();
		NEURO_EXPORT  void SetLabel(const wchar_t*);


		NEURO_EXPORT  float GetLeakRate();
		NEURO_EXPORT  void SetLeakRate(float value);
		NEURO_EXPORT  int GetAxonDelay();
		NEURO_EXPORT  void SetAxonDelay(int value);
		NEURO_EXPORT  long long GetLastFired();

		NEURO_EXPORT  void AddToCurrentValue(float weight);

		NEURO_EXPORT  bool Fire1(long long generation);
		void Fire2(long long cycle);
		bool IsGated(long long cycle);
		void HandleHebbian2Synapses(long long cycle);
		void Fire3(long long cycle);

		float NewHebbianWeight(float y, float offset, SynapseBase::modelType model, int numberOfSynapses);

		NeuronBase(const NeuronBase& t)
		{
			model = t.model;
			id = t.id;
			leakRate = t.leakRate;
		}
		NeuronBase& operator = (const NeuronBase& t)
		{
			return *this;
		}

	};
}

