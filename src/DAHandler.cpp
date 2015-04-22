#include "DAHandler.h"
#include "VisualNetwork.h"
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

DAHandler::DAHandler(int checkTimeStep)
{
   initialize();
   mCheckTimeStep = checkTimeStep;
}

void DAHandler::initialize()
{
   mD = 0;
   mDMultiplier = 0.995f;
   mCheckTimeStep = 1;
   mTime = mTimeStep = 0;
   mLayer = 0;
}

void DAHandler::wakeup()
{
   mTime = mLayer->getPointerToTime();
   mTimeStep = mLayer->getPointerToTimeStep();
   mLogger.set("DA" + mLayer->getID());
}

void DAHandler::update()
{
   //TODO: nasty! just think about something more general!
   if (std::fmod(*mTime, mCheckTimeStep) < *(mTimeStep))
   {
      mD *= mDMultiplier;
      float rw = checkForReward();
      if (rw)
      {
         mLogger.writeLine(Logger::toString(*mTime));
         mD += rw;
      }
   }
}

IzhikevichDAHandler::IzhikevichDAHandler(int checkTimeStep)
   : DAHandler(checkTimeStep)
{
   initialize();
}

void IzhikevichDAHandler::initialize()
{
   mPreID = mPostID = mSynapseDelay = -1;
   mLastPreFireTime = mLastPostFireTime = -1000;
   mAnythingFired = false;
   mAcceptableDuration = 20;
}

void IzhikevichDAHandler::notifyOfSpike(int neuronID)
{
   if (neuronID == mPreID)
   {
      mLastPreFireTime = *mTime;
      mAnythingFired = true;
   }
   else if (neuronID == mPostID)
   {
      mLastPostFireTime = *mTime;
      mAnythingFired = true;
   }
}

float IzhikevichDAHandler::checkForReward()
{
   if (mAnythingFired)
   {
      mAnythingFired = false;
      if (mLastPostFireTime > mLastPreFireTime + mSynapseDelay &&
         *mTime - mLastPreFireTime < mAcceptableDuration)
      {
         std::cout << "Rewarded. DA = " << (mD + 0.5f) << std::endl;
         return 0.5f;
      }
   }

   return 0.0f;
}

void IzhikevichDAHandler::setSynapse(const Synapse* syn)
{
   mPreID = syn->getPreNeuronID();
   mPostID = syn->getPostNeuronID();
   mSynapseDelay = syn->getDelay();
}

template <class Archive>
void DAHandler::serialize(Archive &ar, const unsigned int version)
{
   ar & mDMultiplier
      & mLayer & mCheckTimeStep;
}

template void DAHandler::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void DAHandler::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);