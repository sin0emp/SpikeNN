#include "Neuron.h"
#include "Network.h"
#include "Layer.h"
#include "Synapse.h"
#include <string>

Neuron::Neuron(Layer* layer, int ID, ChannelType type)
{
   mLayer = layer;
   mType = type;
   mID = ID;
   mInputCurrent = 0;
   mPreSynapsesNum = mPostSynapsesNum = 0;
   mInputCurrentNum = 0;
   mLogPotentialFlag = false;

   mLogger.set(mLayer->getAddress(mLayer->getID(), mID));
}

Neuron::~Neuron()
{
   for (std::size_t i = 0; i < mPostSynapses.size(); ++i)
      delete mPostSynapses[i];
}

void Neuron::propagateSpike()
{
   if (mLayer->getTime() % 1000 > 500)
      int a = 1;

   mLayer->recordSpike(mID);
   
   for (std::size_t i = 0; i < mPostSynapsesNum; ++i)
      mPostSynapses[i]->addSpike();

   for (std::size_t i = 0; i < mPreSynapsesNum; ++i)
      mPreSynapses[i]->setPostSpikeTime();
}

void Neuron::addInputCurrent(int time, float current)
{
   //sort calls according to their time using insertion sort
   //begin checking from the end of vector because the call 
   //probably will take place at the end of vector
   int i = mInputCurrentNum - 1;

   for (; i >= 0; --i)
      if (time > mInputCurrentQueue[i].fireTime)
         break;

   mInputCurrentQueue.insert(mInputCurrentQueue.begin() + (i + 1), CurrentInfo(time, current));
   ++mInputCurrentNum;
}

Synapse* Neuron::makeConnection(Neuron* source, Neuron* dest, float weight, int delay, ChannelType type)
{
   //TODO: throw error if the two neurons dosn't belong to a same network

   Synapse* syn = new Synapse(source->mLayer, source, dest, (type == DEPENDENT)? source->mType : type,  weight, delay);
   source->mPostSynapses.push_back(syn);
   ++source->mPostSynapsesNum;
   dest->mPreSynapses.push_back(syn);
   ++dest->mPreSynapsesNum;
   return syn;
}

void Neuron::update()
{
   int t = mLayer->getTime();

   while (mInputCurrentNum > 0)
   {
      //because of the zero delays, when network arrives here the firing time
      //can be less than the actual network time, hence <= has been put here
      if (mInputCurrentQueue[0].fireTime <= t)
      {
         mInputCurrent += mInputCurrentQueue[0].currentToAdd;
         mInputCurrentQueue.erase(mInputCurrentQueue.begin());
         --mInputCurrentNum;
      }
      else
         break;
   }

   //call the overriden function to update the potential
   float potential = updatePotential();
   mInputCurrent = 0; //reset input current for the next cycle

   if (mLogPotentialFlag)
      mLogger.writeLine(Logger::toString(mLayer->getTime()) + " " + Logger::toString(potential));
}

bool Neuron::isConnectedTo(Neuron* n)
{
   for(size_t i = 0; i < mPostSynapsesNum; ++i)
      if (mPostSynapses[i]->isConnectedTo(n))
         return true;

   return false;
}

std::string Neuron::getSpikeTimes()
{
   std::string s;
   
   for (size_t i = 0; i < mSpikeTimes.size(); ++i)
      s += (Logger::toString((float)(mSpikeTimes[i] % 60000)) + " " + Logger::toString((float)mID) + "\n");

   mSpikeTimes.clear();

   return s;
}

void Neuron::logPotential()
{
   mLogPotentialFlag = true;
}

void Neuron::logPostSynapseWeight(std::string directory)
{
   for(size_t i = 0; i < mPostSynapses.size(); ++i)
      mPostSynapses[i]->addWeightLog(directory);
}

void Neuron::logPreSynapseWeight(std::string directory)
{
   for(size_t i = 0; i < mPreSynapses.size(); ++i)
      mPreSynapses[i]->addWeightLog(directory);
}

int Neuron::getLayerID()
{
   return mLayer->getID(); 
}

void Neuron::rest()
{
   mInputCurrent = 0;
   mSpikeTimes.clear();
}