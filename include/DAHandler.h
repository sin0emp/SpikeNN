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
   friend class RewardChecker;
public:
   DAHandler();
   void set(Layer* layer, RewardChecker* rewardChecker, int representClass, int checkTimeStep);
   
   float getDAConcentraion() { return mD; }
   void  notifyOfSpike() { /*(group == 1)? ++mG1SpikeNum : ++mG2SpikeNum;*/ ++mGSpikeNum; }

   void  update();
   float TauD;  //DA uptake constant

private:
   //model variables and parameters
   float mD;           //DA concentration
   float mDMultiplier;
   //int   mG1SpikeNum;
   //int   mG2SpikeNum;
   int   mGSpikeNum;
   int   mRepresentClass;

   int   mCheckTimeStep;
   //bool  mG1WinFlag;
   //bool  mG2WinFlag;
   std::vector<int> mRewardTimes;

   //int mLastPreRewarded;
   //int mLastPostRewarded;

   //int mGuessedRight;
   //elements that DA module might work with
   Layer*          mLayer;
   RewardChecker*  mRewartChecker;
   Logger          mLogger;
   int AcceptableDuration;

   //void  checkForReward();

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

class MODULE_EXPORT RewardChecker
{
   friend class boost::serialization::access;
   friend class DAHandler;
public:
   RewardChecker() {mWinTimes = 0;}
   void addDAHandler(DAHandler* dh) { mDAHandlers.push_back(dh); }
   bool checkForReward(int fromClass);

private:
   std::vector<DAHandler*> mDAHandlers;
   int mWinTimes;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

#endif