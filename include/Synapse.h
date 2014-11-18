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

class MODULE_EXPORT Synapse
{
   friend class boost::serialization::access;
   friend class DAHandler;
public:
   Synapse(Layer* layer, Neuron* pre, Neuron* post, ChannelType type = EXCITATORY,
           float weight = -1, int delay = -1);
   void wakeup();

   void setPostSpikeTime();
   void addSpike();
   bool isFrom(Neuron* n) {return n == mPreNeuron;}
   bool isConnectedTo(Neuron* n) {return n == mPostNeuron;}
   bool isFromLayer(int layerIndex);
   float getWeight() { return mWeight; }
   ChannelType getType() { return mType; }
   void addWeightLog(std::string directory = "");
   void logWeight(bool (*pattern)(int) = 0);
   void logWeight(bool (*pattern)(int, int, int, int) = 0);
   void  updateWeight();

private:
   Layer*               mLayer;
   int                  mID;
   Neuron*              mPreNeuron;
   Neuron*              mPostNeuron;
   float                mWeight;
   int                  mDelay;
   ChannelType          mType;
   Logger               mLogger;
   bool                 mLogWeightFlag;
   int                  mLastPostSpikeTime;     //pre and post neuron spikes used for STDP learning algorithm
   int                  mLastPreSpikeTime;
   const int*           mTime;
   float                mC;                     //STDP variable, eligibility trace

   void  stepIncreaseSTDP();

   void initialize();
   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
   Synapse() { initialize(); } //used only by boost::serialization
};

#endif