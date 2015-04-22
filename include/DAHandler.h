#ifndef DAHANDLER_H
#define DAHANDLER_H

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

#include "Synapse.h"

namespace boost{ namespace serialization { class access; } namespace archive { class text_oarchive; } }
class Network;
class RewardChecker;

class MODULE_EXPORT DAHandler
{
   friend class boost::serialization::access;
public:
   DAHandler(int checkTimeStep);
   DAHandler() { initialize(); }
   void initialize();
   void wakeup();

   void setLayer(Layer* layer) { mLayer = layer; wakeup(); }
   float getDAConcentraion() { return mD; }
   virtual void notifyOfSpike (int neuronID) = 0;
   virtual float checkForReward() = 0;
   void  update();

protected:
   //model variables and parameters
   float mD;           //DA concentration
   float mDMultiplier;
   int   mCheckTimeStep;
   const float* mTime;
   const float* mTimeStep;

   //elements that DA module might work with
   Layer* mLayer;
   Logger mLogger;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

class MODULE_EXPORT IzhikevichDAHandler : public DAHandler
{
public:
   IzhikevichDAHandler(int checkTimeStep);
   IzhikevichDAHandler() { initialize(); }

   void setSynapse(const Synapse* syn);

   virtual void notifyOfSpike(int neuronID);
   virtual float checkForReward();

protected:
   int   mPreID;
   int   mPostID;
   float mLastPreFireTime;
   float mLastPostFireTime;
   int   mSynapseDelay;
   int   mAcceptableDuration;
   bool  mAnythingFired;

   void initialize();
};

#endif