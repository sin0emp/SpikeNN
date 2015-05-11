#include "Synapse.h"
#include "Network.h"
#include "Neuron.h"
#include <cmath>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

SynapseBase::SynapseBase(Layer* layer, float weight, int delay, ChannelType type)
{
   initialize();
   mLayer = layer;
   mWeight = weight;
   mDelay = delay;
   mType = type;
}

void SynapseBase::initialize()
{
   mC = 0;
}

void SynapseBase::stepIncreaseSTDP(float dt)
{
   if ((mType == EXCITATORY && mLayer->shouldExcitatoryLearn())||
       (mType == INHIBITORY && mLayer->shouldInhibitoryLearn()))
   {
      float t = dt-mDelay;
      if(t >= 0)
         mC += (mLayer->getAP() * std::exp(-t / mLayer->getTaoP()));
      else
         mC -= (mLayer->getAN() * std::exp(t / mLayer->getTaoN()));
   }
}

void SynapseBase::updateWeight()
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
}

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

   initialize();
   mPreNeuron = pre;
   mPostNeuron = post;
   mBase = new SynapseBase(layer, weight, delay, type);
   mID = layer->getNextSynapseID();
   wakeup();
}

Synapse::~Synapse()
{
   if (!mBase->mLayer->getSharedConnectionFlag())
      delete mBase;
}

void Synapse::wakeup()
{
   mTime = mBase->mLayer->getPointerToTime();
   mLogger.set(mBase->mLayer->getAddress(mPreNeuron->getLayerID(), mPreNeuron->getID(),
               mPostNeuron->getLayerID(), mPostNeuron->getID()));
}

void Synapse::initialize()
{
   mLogWeightFlag = false;
   mLastPostSpikeTime = mLastPreSpikeTime = -1000;  
}

void Synapse::addSpike()
{
   mLastPreSpikeTime = *mTime;

   if (mBase->mType == EXCITATORY)
      mPostNeuron->addInputCurrent(mLastPreSpikeTime + mBase->mDelay, mBase->mWeight);
   else if (mBase->mType == INHIBITORY)
      mPostNeuron->addInputCurrent(mLastPreSpikeTime + mBase->mDelay, -mBase->mWeight);
   else if (mBase->mType == RESET)
      mPostNeuron->rest(); //should delay be takan into account?


   //TODO: don't give request when learning is turned off!
   if ((mBase->mType == EXCITATORY && mBase->mLayer->shouldExcitatoryLearn()) ||
       (mBase->mType == INHIBITORY && mBase->mLayer->shouldInhibitoryLearn()))
   {
      if (!mBase->mLayer->getSharedConnectionFlag())
      {
         //if (abs(mLastPostSpikeTime - mLastPreSpikeTime) < 20)
         mBase->stepIncreaseSTDP(mLastPostSpikeTime - mLastPreSpikeTime);
         //mLastPreSpikeTime = mLastPostSpikeTime = -1000;
      }
      else /*if (mLastPostSpikeTime > 0)*/
      {
         int postID = mPostNeuron->getID();
         if (mBase->mLayer->mSharedWinnerID == -1 || mBase->mLayer->mSharedWinnerID == postID)
         {
            mBase->stepIncreaseSTDP(mLastPostSpikeTime - mLastPreSpikeTime);
            mBase->mLayer->mSharedWinnerID = postID;
         }
         //mLastPreSpikeTime = mLastPostSpikeTime = -1000;
      }
   }
}

void Synapse::setPostSpikeTime()
{
   mLastPostSpikeTime = *mTime;

   if ((mBase->mType == EXCITATORY && mBase->mLayer->shouldExcitatoryLearn()) ||
       (mBase->mType == INHIBITORY && mBase->mLayer->shouldInhibitoryLearn()))
   {
      if (!mBase->mLayer->getSharedConnectionFlag())
      {
         //if (abs(mLastPostSpikeTime - mLastPreSpikeTime) < 20)
         mBase->stepIncreaseSTDP(mLastPostSpikeTime - mLastPreSpikeTime);
         //mLastPreSpikeTime = mLastPostSpikeTime = -1000;
      }
      else /*if (mLastPreSpikeTime > 0)*/
      {
         int postID = mPostNeuron->getID();
         if (mBase->mLayer->mSharedWinnerID == -1 || mBase->mLayer->mSharedWinnerID == postID)
         {
            mBase->stepIncreaseSTDP(mLastPostSpikeTime - mLastPreSpikeTime);
            mBase->mLayer->mSharedWinnerID = postID;
         }
         //mLastPreSpikeTime = mLastPostSpikeTime = -1000;
      }
   }
}

int Synapse::getPreNeuronID() const
{
   return mPreNeuron->getID();
}

int Synapse::getPostNeuronID() const
{
   return mPostNeuron->getID();
}

//void Synapse::addWeightLog(std::string directory)
//{
//   mLogWeightFlag = true;
//   mLogger.set(mBase->mLayer->getAddress(mPreNeuron->getLayerID(), mPreNeuron->getID(), mPostNeuron->getLayerID(),
//      mPostNeuron->getID()), directory);
//}

//void Synapse::logWeight(bool (*pattern)(int))
//{
//   if ((*pattern)(mID))
//      mLogWeightFlag = true;
//}
//
//void Synapse::logWeight(bool (*pattern)(int, int, int, int))
//{
//   if ((*pattern)(mPreNeuron->getLayerID(), mPreNeuron->getID(), mPostNeuron->getLayerID(), mPostNeuron->getID()))
//      mLogWeightFlag = true;
//}

bool Synapse::isFromLayer(int layerIndex) 
{
   return layerIndex == mPreNeuron->getLayerID();
}

template <class Archive>
void Synapse::serialize(Archive &ar, const unsigned int version)
{
   ar & mID & mPreNeuron
      & mPostNeuron & mBase;
}

template <class Archive>
void SynapseBase::serialize(Archive &ar, const unsigned int version)
{
   ar & mLayer & mWeight & mDelay
      & mType;
}

template void Synapse::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void Synapse::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);
template void SynapseBase::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void SynapseBase::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);
