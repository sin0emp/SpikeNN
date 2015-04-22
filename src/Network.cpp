#include "Network.h"
#include "Synapse.h"
#include "IzhikevichNeuron.h"
#include <iostream>
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

Network::Network(float timeStep)
{
   initialize();
   mTimeStep = timeStep;
   srand(time(NULL));
}

void Network::initialize()
{
   mTime = 0.0f;
   mTimeStep = 0.2f;
   mLastLayerID = -1;
   mLastSynapseID = -1;
   mLogger.set("Network");
   mLogSettingsFlag = false;
   mAP = 0.1f;
   mAN = 0.12f;
   mTaoP = 20.0f;
   mTaoN = 20.0f;
   mCMultiplier = 0.9f;
   mDecayWeightMultiplier = 1.0f;
   mSTDPTimeStep = 100;
   mExMaxWeight = 10.0f;
   mExMaxRandWeight = 10.0f;
   mExMinRandWeight = 0.0f;
   mInMaxWeight = 20.0f;
   mInMaxRandWeight = 20.0f;
   mInMinRandWeight = 0.0f;
   mMaxRandDelay = 20;
   mMinRandDelay = 0;
   mMinInputCurrent = 0.0f;
   mMaxInputCurrent = 20.0f;
   mDefaultConnectionProbability = 0.1f;
}

Network::~Network()
{
   for (size_t i=0; i<mLayers.size(); ++i)
      delete mLayers[i];
}

int Network::addLayer(bool shouldLearn, bool isContainer)
{
   Layer* lay = new Layer(this, ++mLastLayerID, shouldLearn, isContainer);
   mLayers.push_back(lay);
   mLayers[mLastLayerID]->setBoundingParameters(mExMaxWeight, mInMaxWeight, mExMinRandWeight,
      mExMaxRandWeight, mInMinRandWeight, mInMaxRandWeight, mMinRandDelay, mMaxRandDelay);
   mLayers[mLastLayerID]->setSTDPParameters(mCMultiplier, mAP, mAN, mDecayWeightMultiplier, 
      mSTDPTimeStep, mTaoP, mTaoN);
   return mLastLayerID;
}

void Network::runNetwork(int maxTime)
{
#if defined(_WIN32) || defined(_WIN_64)
   _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif
   
   clock_t start = clock();
   mLogger.set("Network");
   std::cout << "Network started." << std::endl;
   
   for (mTime = mTimeStep; mTime <= maxTime; mTime+=mTimeStep)
   {
      if (std::fmod(mTime, 1000) < mTimeStep)
         std::cout << "mTime = " << mTime << std::endl;

      //call layers to update in a random order
      //std::vector<std::size_t> order = SHUFFLE(mLayers.size());
      for (std::size_t i = 0; i < mLayers.size(); i++)
         //mLayers[order[i]]->update();
         mLayers[i]->update();

      //or hierarichally??
      //for (std::size_t i = 0; i < mLayers.size(); i++)
      //   mLayers[i]->update();
   }

   mLogger.writeLine("Frequencies are");
   //std::vector<int> freq = mLayers[0]->getWeightFrequencies();
   //for (int i = 1; i < mLayers.size(); ++i)
   //{
   //   if (mLayers[i]->getLearningFlag())
   //   {
   //      std::vector<int> tempfre = mLayers[i]->getWeightFrequencies();
   //      for (int j = 0; j < tempfre.size(); ++j)
   //         freq[j] += tempfre[j];
   //   }
   //}

   //for (size_t i = 0; i < freq.size(); ++i)
   //   mLogger.writeLine(Logger::toString(i*0.2f) + "-" + Logger::toString((i+1)*0.2f) + ": " + Logger::toString((float)freq[i]));

   clock_t end = clock();
   double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
   mLogger.writeLine("Network time: " + Logger::toString((float)elapsed_secs));
}

void Network::logLayerActivity(int layer)
{
   mLayers[layer]->logActivity(); 
}

void Network::logSettings()
{
   mLogSettingsFlag = true;
}

//void Network::logPostSynapseWeights(int layer, int neuron, std::string directory)
//{
//   mLayers[layer]->logPostSynapseWeight(neuron, directory);
//}
//
//void Network::logPreSynapseWeights(int layer, int neuron, std::string directory)
//{
//   mLayers[layer]->logPreSynapseWeight(neuron, directory);
//}
//
//void Network::logSynapseWeight(bool (*pattern)(int, int, int, int))
//{
//   for (size_t i = 0; i < mLayers.size(); ++i)
//   {
//      mLayers[i]->logWeight(pattern);
//   }
//}
//

void Network::logSynapseWeight(bool (*pattern)(int))
{
   for (size_t i = 0; i < mLayers.size(); ++i)
   {
      mLayers[i]->logWeight(pattern);
   }
}

ConnectionInfo Network::defaultConnectingPattern(int sourceIndex, int destIndex)
{
   float f = float(rand()) / RAND_MAX;

   if (f < mDefaultConnectionProbability)
      return ConnectionInfo(true, EXCITATORY, CHOOSE_RANDOM, CHOOSE_RANDOM);
   else
      return ConnectionInfo(false);
}

std::vector<InputInformation> Network::defaultInputPattern(float time)
{
   return std::vector<InputInformation>();
}

std::string Network::getAddress(int slayer, int sneuron, int dlayer, int dneuron)
{
   std::string s;
   if (slayer != -1)
      s += "l" + Logger::toString((float)slayer);
   if (sneuron != -1)
      s += "n" + Logger::toString((float)sneuron);
   if (dlayer != -1 && dlayer != -1)
      s += "-TO-l" + Logger::toString((float)dlayer) + "n" + Logger::toString((float)dneuron);

   return s;
}

const Synapse* Network::getSynapse(int synapseID)
{
   for (size_t i = 0; i < mLayers.size(); ++i)
   {
      const Synapse* syn = mLayers[i]->getSynapse(synapseID);
      if (syn)
         return syn;
   }

   return 0;
}

template <class Archive>
void Network::serialize(Archive &ar, const unsigned int version)
{
   ar & mLayers
      & mLastLayerID & mLastSynapseID
      & mAP & mAN
      & mTaoP & mTaoN
      & mCMultiplier & mDecayWeightMultiplier
      & mSTDPTimeStep & mExMaxWeight
      & mExMaxRandWeight & mExMinRandWeight
      & mInMaxWeight & mInMaxRandWeight
      & mInMinRandWeight & mMaxRandDelay
      & mMinRandDelay & mMinInputCurrent
      & mMaxInputCurrent & mDefaultConnectionProbability
      & mTimeStep;
}

template void Network::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void Network::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);

void Network::saveNetwork(std::string path)
{
    std::ofstream file(path.c_str(), std::ios::binary);
    boost::archive::text_oarchive oa(file);
    oa.register_type<Network>();
    oa & *this;
    file.close();
}

Network* Network::loadNetwork(std::string path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    boost::archive::text_iarchive oa(file);
    oa.register_type<Network>();
    Network* net = new Network();
    oa & *net;

    for (size_t i = 0; (int)i < net->mLastLayerID; ++i)
       net->mLayers[i]->wakeup();

   file.close();
   return net;
}
