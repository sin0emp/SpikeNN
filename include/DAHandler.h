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
   void set(Network* network);
   
   float getDAConcentraion();
   void  checkForReward();
   void  update();
   float TauD;  //DA uptake constant

private:
   //model variables and parameters
   float mD;           //DA concentration
   float mDMultiplier;
   std::vector<int> mRewardTimes;

   int mLastPreRewarded;
   int mLastPostRewarded;
   int mLastDAUpdate;

   Synapse* mSynapse;
   Network* mNetwork;
   Logger   mLogger;
   int AcceptableDuration;
   
   void  updateDA();

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

#endif