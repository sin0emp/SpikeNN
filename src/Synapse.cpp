#include "Synapse.h"
#include "Network.h"
#include "Neuron.h"
#include <cmath>
#include <iostream>

Synapse::Synapse(Layer* layer, Neuron* pre, Neuron* post, ChannelType type, float weight, int delay)
{
   //srand((int)time(0));

   if (weight == -1)
      weight = layer->getMinRandWeight() + ((float)rand()/RAND_MAX) * (layer->getMaxRandWeight()-layer->getMinRandWeight());

   if (delay == -1)
      delay = (int)std::floor(((float)rand()/RAND_MAX) * (layer->getMaxRandDelay()-layer->getMinRandDelay()) +
         layer->getMinRandDelay() + 0.5);

   mLayer = layer;
   mPreNeuron = pre;
   mPostNeuron = post;
   mWeight = weight;
   mDelay = delay;
   mType = type;
   mID = layer->getNextSynapseID();
   mLogger.set(mLayer->getAddress(pre->getLayerID(), pre->getID(), post->getLayerID(), post->getID()));
   //mLogger.set(Logger::toString((float)mID))
   mLogWeightFlag = false;
   mLastPostSpikeTime = mLastPreSpikeTime = -1000;
   mC = 0;
}

void Synapse::addSpike()
{
   mLastPreSpikeTime = mLayer->getTime();
   mPostNeuron->addInputCurrent(mLastPreSpikeTime + mDelay, (mType == EXCITATORY) ? mWeight : -mWeight);
   if (mLayer->getLearningFlag() && mType == EXCITATORY) stepIncreaseSTDP();
}

void Synapse::setPostSpikeTime()
{
   mLastPostSpikeTime = mLayer->getTime();
   if (mLayer->getLearningFlag() && mType == EXCITATORY) stepIncreaseSTDP();
}

void Synapse::stepIncreaseSTDP()
{
   int t = mLastPostSpikeTime - mLastPreSpikeTime;
   if(t >= 0)
      mC += (mLayer->getAP() * std::exp(-t / mLayer->getTaoP()));
   else
      mC -= (mLayer->getAN() * std::exp(t / mLayer->getTaoN()));
}

void Synapse::updateWeight()
{
   float d = mLayer->getDAConcentraion();
   //mWeight += (d == -1) ? (0.01f + mC) : mC * (0.002f + d);  //this is izhikevich version but why add constants???
   mWeight += (d == -1) ? mC : mC * d;
   if (mWeight > mLayer->getMaxWeight()) mWeight = mLayer->getMaxWeight();
   if (mWeight < 0) mWeight = 0;
   mC *= mLayer->getCMultiplier();
}

void Synapse::addWeightLog()
{
   mLogWeightFlag = true;
}

void Synapse::logWeight(bool (*pattern)(int))
{
   if ((*pattern)(mID))
      mLogWeightFlag = true;
}

void Synapse::logWeight(bool (*pattern)(int, int, int, int))
{
   if ((*pattern)(mPreNeuron->getLayerID(), mPreNeuron->getID(), mPostNeuron->getLayerID(), mPostNeuron->getID()))
      mLogWeightFlag = true;
}