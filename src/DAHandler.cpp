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
   //mLastPostRewarded = mLastPreRewarded = -1000;
   /*mG1SpikeNum = mG2SpikeNum =*/ mTimeStep = mGSpikeNum = 0;
   //mG1WinFlag = mG2WinFlag = false;
   //mGuessedRight = 0;
   //mLogger.set("DA");
}

void DAHandler::set(Layer* layer, RewardChecker* rewardChecker, int representClass, int timestep)
{
   mLayer = layer;
   mRewartChecker = rewardChecker;
   mTimeStep = timestep;
   mRepresentClass = representClass;
   mRewartChecker->addDAHandler(this);
}

void DAHandler::update()
{
   mD *= mDMultiplier;
   //checkForReward();
   int t = *(mLayer->mTime);

   //TODO: nasty! just think about something more general!
   if (t % mTimeStep == 0)
   {
      VisualNetwork* vn = static_cast<VisualNetwork*> (mLayer->mNetwork);
      std::string fn = vn->mImageFileNames[vn->mCurrentImageIndex];
      int target = (fn.find("non-face") != std::string::npos)? 2 : 1;
      //std::cout<<"g1="<<mG1SpikeNum<<" g2="<<mG2SpikeNum<<std::endl;
      //mG1WinFlag=mG2WinFlag=true;

      if (mRepresentClass == target)
      {
         bool rw = mRewartChecker->checkForReward(mRepresentClass);
         if (rw)
         {
            mLogger.writeLine(Logger::toString((float)t));
            mD += 0.5;
            //mRewardTimes.push_back(t);
            /*std::cout<<"\""<<fn<<"\"  won "<<" time=" << t <<"\n";*/
         }
         else //punishment
         {
            for (size_t i=0; i<mRewartChecker->mDAHandlers.size(); ++i)
            {
               if (mRewartChecker->mDAHandlers[i]->mRepresentClass != mRepresentClass)
                  mD -= 0.2;
            }
         }
      }
   }

   if (t % 1000 == 0)
   {
      std::cout << mRewartChecker->mWinTimes << " right guesses.\n";
      mRewartChecker->mWinTimes = 0;
   }

   //while (mRewardTimes.size() > 0)
   //{
   //   if (mRewardTimes[0] <= t)
   //   {
   //      mLogger.writeLine(Logger::toString((float)mRewardTimes[0]));
   //      mD += 0.5;
   //      //std::cout<<"Happend! DA = " << mD << " weight = " 
   //      //   << mSynapse->mBase->mWeight << std::endl;
   //      //std::cout<<"Happened! DA = " << mD << std::endl;

   //      mRewardTimes.erase(mRewardTimes.begin());
   //   }
   //   else
   //      break;
   //}
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
      & mLayer & AcceptableDuration
      & mTimeStep & mRewartChecker;
}

template void DAHandler::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void DAHandler::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);

bool RewardChecker::checkForReward(int fromClass)
{
   int n1=mDAHandlers[0]->mGSpikeNum, n2=mDAHandlers[1]->mGSpikeNum;
   int max = 0;
   int maxnum = mDAHandlers[0]->mGSpikeNum;
   mDAHandlers[0]->mGSpikeNum = 0;
   bool sameNum = false;
   
   for (size_t i=1; i<mDAHandlers.size(); ++i)
   {
      if (mDAHandlers[i]->mGSpikeNum > maxnum)
      {
         max = i;
         maxnum = mDAHandlers[i]->mGSpikeNum;
         sameNum = false;
      }
      else if (mDAHandlers[i]->mGSpikeNum == maxnum)
         sameNum = true;
      
      mDAHandlers[i]->mGSpikeNum = 0;
   }

   if (mDAHandlers[max]->mRepresentClass == fromClass && !sameNum)
   {
      std::cout<<"class "<< fromClass <<" won "<<n1<<"-"<<n2<<"\n";
      //std::cout<<"DA1="<<mDAHandlers[0]->mD<<" DA2="<<mDAHandlers[1]->mD<<std::endl;
      ++mWinTimes;
      return true;
   }
   else
      return false;
}

template <class Archive>
void RewardChecker::serialize(Archive &ar, const unsigned int version)
{
   ar & mDAHandlers;
}

template void RewardChecker::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void RewardChecker::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);