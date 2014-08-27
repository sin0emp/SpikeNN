#include "DAHandler.h"
#include "Network.h"
#include <iostream>

DAHandler::DAHandler()
{
   AcceptableDuration = 20;
   TauD = 200;
   mD = 0;
   mDMultiplier = 0.995f;
   mLastPostRewarded = mLastPreRewarded = -1000;
   mLogger.set("DA");
}

void DAHandler::set(Network* network)
{
   mNetwork = network;
   mSynapse = network->mLayers[0]->mSynapses[0];
   mSynapse->mWeight = 0;
}

float DAHandler::getDAConcentraion()
{
   return mD;
}

void DAHandler::updateDA()
{
   mD *= mDMultiplier;
}

void DAHandler::update()
{
   updateDA();
   checkForReward();
}

void DAHandler::checkForReward()
{
   int t = mNetwork->getTime();

   if (mSynapse->mLastPostSpikeTime != mLastPostRewarded || mSynapse->mLastPreSpikeTime != mLastPreRewarded)
   {
      mLastPostRewarded = mSynapse->mLastPostSpikeTime;
      mLastPreRewarded = mSynapse->mLastPreSpikeTime;

      if (mLastPostRewarded > mLastPreRewarded && t - mLastPreRewarded < AcceptableDuration)
      {
         mRewardTimes.push_back(t + 1000 + rand() % 2000);

         for (std::size_t i = mRewardTimes.size() - 1; i > 0; --i)
         {
            if (mRewardTimes[i] < mRewardTimes[i-1])
            {
               int temp = mRewardTimes[i];
               mRewardTimes[i] = mRewardTimes[i-1];
               mRewardTimes[i-1] = temp;
            }
         }
      }
   }

   float re = 0;
   while (mRewardTimes.size() > 0)
   {
      if (mRewardTimes[0] <= t)
      {
         mLogger.writeLine(Logger::toString((float)mRewardTimes[0]));
         updateDA();
         mD += 0.5;
         std::cout<<"Happend! DA = " << mD << std::endl;
         mRewardTimes.erase(mRewardTimes.begin());
      }
      else
         break;
   }
}
