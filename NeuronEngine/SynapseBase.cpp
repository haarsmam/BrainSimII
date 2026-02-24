#include "pch.h"
#include "SynapseBase.h"
#include "NeuronBase.h"

namespace NeuronEngine
{
	NeuronBase* SynapseBase::GetTarget() const
	{
		return targetNeuron;
	}
	void SynapseBase::SetTarget(NeuronBase* target)
	{
		targetNeuron = target;
	}
	float SynapseBase::GetWeight() const
	{
		return weight;
	}
	void SynapseBase::SetWeight(float value)
	{
		weight = value;
	}

	SynapseBase::modelType SynapseBase::GetModel() const
	{
		return model;
	}
	void SynapseBase::SetModel(SynapseBase::modelType value)
	{
		model = value;
	}
}
