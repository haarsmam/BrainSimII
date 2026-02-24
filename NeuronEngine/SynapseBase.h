#pragma once

#include "platform.h"

namespace NeuronEngine { class NeuronBase; }

namespace NeuronEngine
{
	class NEURO_EXPORT SynapseBase
	{
	public:
		enum class modelType { Fixed, Binary, Hebbian1, Hebbian2,Hebbian3,Gate,Learn};

		void SetTarget(NeuronBase * target);
		NeuronBase* GetTarget() const;
		float GetWeight() const;
		void SetWeight(float value);
		void SetModel(modelType value);
		modelType GetModel() const;

	private:
		NeuronBase* targetNeuron = 0; //pointer to the target neuron
		float weight = 0; //weight of the synapse
		modelType model = modelType::Fixed;
	};
}
