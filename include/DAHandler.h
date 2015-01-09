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

class MODULE_EXPORT DAHandler
{
   friend class boost::serialization::access;
public:
   DAHandler();
   void set(Layer* layer, int timeStep);
   
   float getDAConcentraion() { return mD; }
   void  notifyOfSpike(int group) { (group == 1)? ++mG1SpikeNum : ++mG2SpikeNum; }

   void  update();
   float TauD;  //DA uptake constant

private:
   //model variables and parameters
   float mD;           //DA concentration
   float mDMultiplier;
   int   mG1SpikeNum;
   int   mG2SpikeNum;
   int   mTimeStep;
   std::vector<int> mRewardTimes;

   int mLastPreRewarded;
   int mLastPostRewarded;

   //elements that DA module might work with
   Layer*                mLayer;

   Logger       mLogger;
   int AcceptableDuration;

   //void  checkForReward();

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

#endif