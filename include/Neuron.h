#ifndef NEURON_H
#define NEURON_H

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

#include <vector>
#include "Logger.h"
#include "Synapse.h"
#include "GlobalVars.h"

namespace boost{ namespace serialization { class access; } namespace archive { class text_oarchive; } }
class Layer;
struct CurrentInfo;

class MODULE_EXPORT Neuron
{
   friend class boost::serialization::access;
   friend class DAHandler;
public:
   Neuron(Layer* layer, int ID, ChannelType type = EXCITATORY);
   Neuron() { initialize(); }  //used only by boost::serialization
   ~Neuron();
   void wakeup();

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

   std::vector<SynapseBase*> getPreSynapsesToShare();
   void setPreSynapsesToShare(std::vector<SynapseBase*>);


   //void logPostSynapseWeight(std::string directory = "");
   //void logPreSynapseWeight(std::string directory = "");

   std::vector<float> getResponseFromLayer(int sourceLayer);
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
   const int*                    mTime;

   //int                          mLastLoggedIndex;
   std::vector<CurrentInfo>      mInputCurrentQueue;
   int                           mInputCurrentNum;

   virtual float updatePotential() = 0;

   void initialize();

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

struct CurrentInfo
{
   CurrentInfo(int time, float current) : fireTime(time), currentToAdd(current){};
   int      fireTime;
   float    currentToAdd;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version)
   {
      ar & fireTime & currentToAdd;
   }
};

#endif