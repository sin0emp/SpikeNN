#include "Synapse.h"
#include "Network.h"
#include "Neuron.h"
#include <cmath>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

Synapse::Synapse(Layer* layer, Neuron* pre, Neuron* post, ChannelType type, float weight, int delay)
{
   //srand((int)time(0));
   
   if (weight == -1)
   {
      if (type == EXCITATORY)
         weight = layer->getExcitatoryMinRandWeight() + ((float)rand()/RAND_MAX) * 
                  (layer->getExcitatoryMaxRandWeight()-layer->getExcitatoryMinRandWeight());
      else
         weight = layer->getInhibitoryMinRandWeight() + ((float)rand()/RAND_MAX) *
                  (layer->getInhibitoryMaxRandWeight()-layer->getInhibitoryMinRandWeight());
   }

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
   wakeup();
   //mLogger.set(mLayer->getAddress(pre->getLayerID(), pre->getID(), post->getLayerID(), post->getID()));
}

void Synapse::wakeup()
{
   mTime = mLayer->getPointerToTime();
}

void Synapse::initialize()
{
   mLogWeightFlag = false;
   mLastPostSpikeTime = mLastPreSpikeTime = -1000;
   mC = 0;
}

void Synapse::addSpike()
{
   mLastPreSpikeTime = *mTime;

   if (mType == EXCITATORY)
      mPostNeuron->addInputCurrent(mLastPreSpikeTime + mDelay, mWeight);
   else if (mType == INHIBITORY)
      mPostNeuron->addInputCurrent(mLastPreSpikeTime + mDelay, -mWeight);
   else if (mType == RESET)
      mPostNeuron->rest(); //should delay be takan into account?

   if ((mType == EXCITATORY && mLayer->shouldExcitatoryLearn())||
       (mType == INHIBITORY && mLayer->shouldInhibitoryLearn())) stepIncreaseSTDP();
   mLastPostSpikeTime = -1000;
}

void Synapse::setPostSpikeTime()
{
   mLastPostSpikeTime = *mTime;
   if ((mType == EXCITATORY && mLayer->shouldExcitatoryLearn())||
       (mType == INHIBITORY && mLayer->shouldInhibitoryLearn())) stepIncreaseSTDP();
   mLastPreSpikeTime = -1000;
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
   float maxWeight = (mType == EXCITATORY) ? 
      mLayer->getExcitatoryMaxWeight() : mLayer->getInhibitoryMaxWeight();
   if (mWeight > maxWeight) mWeight = maxWeight;
   if (mWeight < 0) mWeight = 0;
   mC *= mLayer->getCMultiplier();
   mWeight *= mLayer->getDecayMultiplier();

   if (*mTime % 1000 == 0 && mLogWeightFlag)
      mLogger.writeLine(Logger::toString((float)*mTime) + " " + Logger::toString(mWeight));

}

void Synapse::addWeightLog(std::string directory)
{
   mLogWeightFlag = true;
   mLogger.set(mLayer->getAddress(mPreNeuron->getLayerID(), mPreNeuron->getID(), mPostNeuron->getLayerID(),
      mPostNeuron->getID()), directory);
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

bool Synapse::isFromLayer(int layerIndex) 
{
   return layerIndex == mPreNeuron->getLayerID();
}

template <class Archive>
void Synapse::serialize(Archive &ar, const unsigned int version)
{
   ar & mLayer & mID & mPreNeuron
      & mPostNeuron & mWeight & mDelay
      & mType;
}

template void Synapse::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void Synapse::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);
