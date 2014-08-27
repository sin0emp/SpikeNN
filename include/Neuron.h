#ifndef NEURON_H
#define NEURON_H

#ifndef MODULE_EXPORT
   #if defined(_WIN32) || defined(_WIN_64)
      #ifdef SpikeNN_EXPORTS
         #define MODULE_EXPORT  __declspec(dllexport)   // export DLL information
      #else
         #define MODULE_EXPORT  __declspec(dllimport)   // import DLL information
      #endif
   #else
      #define MODULE_EXPORT
   #endif
#endif

#include <vector>
#include "Logger.h"
#include "Synapse.h"
#include "GlobalVars.h"

class Layer;
struct CurrentInfo;

class MODULE_EXPORT Neuron
{
public:
   Neuron(Layer* layer, int ID, ChannelType type = EXCITATORY);
   ~Neuron();

   void update();
   void addInputCurrent(int time, float current);
   void propagateSpike();
   template <int SIZE>
   void addStimulus(int (&stimuliTimes)[SIZE]);
   bool isConnectedTo(Neuron* n);
   int  getID() { return mID; }
   int  getLayerID();
   ChannelType getType() { return mType; }
   static Synapse* makeConnection(Neuron* source, Neuron* dest, float weight = -1, int delay = -1, ChannelType type = DEPENDENT);
   void logPotential();
   std::string getSpikeTimes();

   void logPostSynapseWeight();
   void logPreSynapseWeight();

   virtual void rest();

protected:
   int                           mID;
   ChannelType                   mType;
   Layer*                        mLayer;
   float                         mInputCurrent;
   std::vector<Synapse*>         mPostSynapses;
   size_t                        mPostSynapsesNum;
   std::vector<Synapse*>         mPreSynapses;
   size_t                        mPreSynapsesNum;
   std::vector<int>              mSpikeTimes;
   Logger                        mLogger;
   bool                          mLogPotentialFlag;

   //int                          mLastLoggedIndex;
   std::vector<CurrentInfo>      mInputCurrentQueue;
   int                           mInputCurrentNum;

   virtual float updatePotential() = 0;
};

struct CurrentInfo
{
   CurrentInfo(int time, float current) : fireTime(time), currentToAdd(current){};
   int      fireTime;
   float    currentToAdd;
};

#endif