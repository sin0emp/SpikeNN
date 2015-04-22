#ifndef SYNAPSE_H
#define SYNAPSE_H

#ifndef MODULE_EXPORT
   #if defined(_WIN32) || defined(_WIN_64)
      #ifdef SpikeNN_EXPORTS
         #define MODULE_EXPORT __declspec(dllexport)    // export DLL information
      #else
         #define MODULE_EXPORT __declspec(dllimport)    // import DLL information
      #endif
   #else
      #define MODULE_EXPORT
   #endif
#endif

#include "Logger.h"
#include <vector>
#include "GlobalVars.h"

namespace boost{ namespace serialization { class access; } namespace archive { class text_oarchive; } }
class Layer;
class Neuron;
class Synapse;

// this struct is implimented so that neurons can share some connection informations
struct MODULE_EXPORT SynapseBase
{
   //Synapse base can be either belong to a synapse or a layer (for shared weight mode)
   friend class Synapse;
   friend class Layer;
   friend class boost::serialization::access;

public:
   SynapseBase(Layer* layer, float weight, int delay, ChannelType type = EXCITATORY);
   SynapseBase(){ initialize(); } //used only by boost::serialization

   float getWeight() { return mWeight; }
   ChannelType getType() { return mType; }
   void updateWeight();
   void stepIncreaseSTDP(float dt);

private:
   Layer*          mLayer;
   float           mWeight;
   int             mDelay;
   ChannelType     mType;
   float           mC;      //STDP variable, eligibility trace

   void initialize();

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

class MODULE_EXPORT Synapse
{
   friend class boost::serialization::access;
public:
   Synapse(Layer* layer, Neuron* pre, Neuron* post, ChannelType type = EXCITATORY,
           float weight = -1, int delay = -1);
   Synapse() { initialize(); } //used only by boost::serialization
   ~Synapse();
   void wakeup();

   void setPostSpikeTime();
   void addSpike();
   bool isFrom(Neuron* n) {return n == mPreNeuron;}
   bool isConnectedTo(Neuron* n) {return n == mPostNeuron;}
   bool isFromLayer(int layerIndex);
   int  getID() { return mID; }
   int  getPreNeuronID() const;
   int  getPostNeuronID() const;
   float getWeight() const { return mBase->mWeight; }
   int   getDelay() const { return mBase->mDelay; }
   ChannelType getType() { return mBase->mType; }
   void logCurrentWeight() { mLogger.writeLine(Logger::toString(*mTime) + " " + Logger::toString(mBase->mWeight)); }
   //void addWeightLog(std::string directory = "");
   //void logWeight(bool (*pattern)(int) = 0);
   //void logWeight(bool (*pattern)(int, int, int, int) = 0);
   SynapseBase* getBase() { return mBase; }
   void rest() { mLastPostSpikeTime = mLastPreSpikeTime = -1000; mBase->mC = 0; }
   //void updateWeight()
   //{
   //   mBase->updateWeight();
   //   if (*mTime % 1000 == 0 && mLogWeightFlag)
   //      mLogger.writeLine(Logger::toString((float)*mTime) + " " + Logger::toString(mBase->mWeight));
   //}

   SynapseBase*         mBase;

private:
   int                  mID;
   Neuron*              mPreNeuron;
   Neuron*              mPostNeuron;
   Logger               mLogger;
   bool                 mLogWeightFlag;
   float                mLastPostSpikeTime;     //pre and post neuron spikes used for STDP learning algorithm
   float                mLastPreSpikeTime;
   const float*         mTime;

   void  stepIncreaseSTDP();

   void initialize();
   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

#endif