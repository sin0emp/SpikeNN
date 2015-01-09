#include "DAHandler.h"
#include "VisualNetwork.h"
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

DAHandler::DAHandler()
{
   AcceptableDuration = 20;
   TauD = 200;
   mD = 0;
   mDMultiplier = 0.995f;
   mLastPostRewarded = mLastPreRewarded = -1000;
   mLogger.set("DA");
}

void DAHandler::set(Layer* layer, int timestep)
{
   mLayer = layer;
   mTimeStep = timestep;
}

void DAHandler::update()
{
   mD *= mDMultiplier;
   //checkForReward();
   int t = *(mLayer->mTime);

   //TODO: nasty! just think about something more general!
   if (t % mTimeStep == 1)
   {
      VisualNetwork* vn = static_cast<VisualNetwork*> (mLayer->mNetwork);
      std::string fn = vn->mImageFileNames[vn->mCurrentImageIndex];
      if ((mG1SpikeNum > mG2SpikeNum && fn.find("face") != std::string::npos) ||
          (mG1SpikeNum < mG2SpikeNum && fn.find("non-face") != std::string::npos))
         mRewardTimes.push_back(t);

      mG1SpikeNum = mG2SpikeNum = 0;
   }

   while (mRewardTimes.size() > 0)
   {
      if (mRewardTimes[0] <= t)
      {
         mLogger.writeLine(Logger::toString((float)mRewardTimes[0]));
         mD += 0.5;
         //std::cout<<"Happend! DA = " << mD << " weight = " 
         //   << mSynapse->mBase->mWeight << std::endl;
         std::cout<<"Happend! DA = " << mD << std::endl;

         mRewardTimes.erase(mRewardTimes.begin());
      }
      else
         break;
   }
}

//void DAHandler::checkForReward()
//{
//   int t = *(mLayer->mTime);
//
//   if (mSynapse->mLastPostSpikeTime != mLastPostRewarded || mSynapse->mLastPreSpikeTime != mLastPreRewarded)
//   {
//      mLastPostRewarded = mSynapse->mLastPostSpikeTime;
//      mLastPreRewarded = mSynapse->mLastPreSpikeTime;
//
//      if (mLastPostRewarded > mLastPreRewarded + mSynapse->mBase->mDelay && t - mLastPreRewarded < AcceptableDuration)
//      {
//         mRewardTimes.push_back(t + 1000 + rand() % 2000);
//
//         for (std::size_t i = mRewardTimes.size() - 1; i > 0; --i)
//         {
//            if (mRewardTimes[i] < mRewardTimes[i-1])
//            {
//               int temp = mRewardTimes[i];
//               mRewardTimes[i] = mRewardTimes[i-1];
//               mRewardTimes[i-1] = temp;
//            }
//         }
//      }
//   }
//}

template <class Archive>
void DAHandler::serialize(Archive &ar, const unsigned int version)
{
   ar & mD & mDMultiplier
      & mLayer & AcceptableDuration;
}

template void DAHandler::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void DAHandler::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);