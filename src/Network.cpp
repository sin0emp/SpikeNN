#include "Network.h"
#include "Synapse.h"
#include "IzhikevichNeuron.h"
#include <iostream>
#include <string>

Network::Network()
{
   srand(time(NULL));
   mLogger.set("Network");
   mLogSettingsFlag = false;

   mDefaultConnectionProbability = 0.1f;

   //default parameters
   mMinInputCurrent = 0;
   mMaxInputCurrent = 20;
   mMaxWeight = 10;
   mMaxRandWeight = 10;
   mMinRandWeight = 0;
   mMaxRandDelay = 20;
   mMinRandDelay = 0;
   mSTDPTimeStep = 1000;
   mCMultiplier = 0.9f;
   mAP = 0.1f;
   mAN = 0.12f;
   mTaoP = 20.0f;
   mTaoN = 20.0f;
   mCMultiplier = 0.9f;
   mLastLayerID = -1;
   mLastSynapseID = -1;
   mDAHandler = 0;
}

Network::~Network()
{
   if(mDAHandler)
      delete mDAHandler;
}

int Network::addLayer(bool shouldLearn, bool isContainer)
{
   Layer* lay = new Layer(this, ++mLastLayerID, shouldLearn, isContainer);
   mLayers.push_back(lay);
   int index = mLayers.size() - 1;
   mLayers[index]->setBoundingParameters(mMaxWeight, mMinRandWeight, mMaxRandWeight, mMinRandDelay, mMaxRandDelay);
   mLayers[index]->setSTDPParameters(mCMultiplier, mAP, mAN, mSTDPTimeStep, mTaoP, mTaoN);
   return index;
}

void Network::addDAModule()
{
   mDAHandler = new DAHandler();
   mDAHandler->set(this);
}

void Network::runNetwork(int maxTime)
{
   clock_t start = clock();

   std::cout << "Network started." << std::endl;
   for (mTime = 1; mTime <= maxTime; mTime++)
   {
      if (mTime % 1000 == 0)
      {
         std::cout << "mTime = " << mTime << std::endl;
      }

      //signal to layers to update components
      for (std::size_t i = 0; i < mLayers.size(); i++)
         mLayers[i]->update();

      if (mDAHandler)
         mDAHandler->update();
   }

   mLogger.set("Network");
   mLogger.writeLine("Frequencies are");
   std::vector<int> freq = mLayers[0]->getWeightFrequencies();
   for (size_t i = 0; i < freq.size(); ++i)
      mLogger.writeLine(Logger::toString(i*0.2f) + "-" + Logger::toString((i+1)*0.2f) + ": " + Logger::toString((float)freq[i]));

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

void Network::logPostSynapseWeights(int layer, int neuron)
{
   mLayers[layer]->logPostSynapseWeight(neuron);
}

void Network::logPreSynapseWeights(int layer, int neuron)
{
   mLayers[layer]->logPreSynapseWeight(neuron);
}

void Network::logSynapseWeight(bool (*pattern)(int, int, int, int))
{
   for (size_t i = 0; i < mLayers.size(); ++i)
   {
      mLayers[i]->logWeight(pattern);
   }
}

void Network::logSynapseWeight(bool (*pattern)(int))
{
   for (size_t i = 0; i < mLayers.size(); ++i)
   {
      mLayers[i]->logWeight(pattern);
   }
}

ConnectionInfo Network::defaultConnectingPattern(int sourceIndex, int destIndex)
{
   float f = float(rand() / RAND_MAX);

   if (f < mDefaultConnectionProbability)
      return ConnectionInfo(true, EXCITATORY, CHOOSE_RANDOM, CHOOSE_RANDOM);
   else
      return ConnectionInfo(false);
}

std::vector<InputInformation> Network::defaultInputPattern(int time)
{
   return std::vector<InputInformation>();
}

std::string Network::getAddress(int slayer, int sneuron, int dlayer, int dneuron)
{
   std::string s;
   if (slayer != -1)
      s += "l" + Logger::toString(slayer);
   if (sneuron != -1)
      s += "n" + Logger::toString(sneuron);
   if (dlayer != -1 && dlayer != -1)
      s += "-TO-l" + Logger::toString(dlayer) + "n" + Logger::toString(dneuron);

   return s;
}