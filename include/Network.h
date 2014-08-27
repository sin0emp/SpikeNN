#ifndef NETWORK_H
#define NETWORK_H

#ifndef MODULE_EXPORT
   #if defined(_WIN32) || defined(_WIN_64)
      #ifdef SpikeNN_EXPORTS
         #define MODULE_EXPORT  __declspec(dllexport)   // export DLL information
      #else
         #define MODULE_EXPORT  __declspec(dllimport)   // import DLL information
      #endif
   #else
      #define MODULE_EXPORT
   #endif
#endif

#include "Layer.h"
#include "Logger.h"
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "GlobalVars.h"
#include "DAHandler.h"

class MODULE_EXPORT Network
{
   friend class DAHandler;
public:
   Network();
   ~Network();

   int  addLayer(bool shouldLearn = true, bool isContainer = false);

   template <class NeuronTemp>
   bool addNeuron(int layerIndex, int neuronNum, ChannelType type = EXCITATORY, 
      ParameterContainer* params = 0);

   void makeConnection(int sourceLayerIndex, int destLayerIndex,
      float synapseProb, float excitatoryWeight = -1.0f, float inhibitoryWeight = -1.0f,
      int excitatoryDelay = -1.0f, int inhibitoryDelay = -1.0f)
   {
      Layer::makeConnection(mLayers[sourceLayerIndex], mLayers[destLayerIndex],
         synapseProb, excitatoryWeight, inhibitoryWeight, excitatoryDelay, inhibitoryDelay);
   }

   void makeConnection(int sourceLayerIndex, int destLayerIndex,
      int neuronsNumToConnect, float excitatoryWeight = -1.0f, float inhibitoryWeight = -1.0f,
      int excitatoryDelay = -1.0f, int inhibitoryDelay = -1.0f)
   {
      Layer::makeConnection(mLayers[sourceLayerIndex], mLayers[destLayerIndex],
         neuronsNumToConnect, excitatoryWeight, inhibitoryWeight, excitatoryDelay, inhibitoryDelay);
   }

   void makeConnection(int sourceLayerIndex, int sourceNeuronIndex, int destLayerIndex, 
      int destNeuronIndex, float weight = -1, int delay = -1)
   {
      Layer::makeConnection(mLayers[sourceLayerIndex], sourceNeuronIndex,
         mLayers[destLayerIndex], destNeuronIndex, weight, delay);
   }

   void makeConnection(int sourceLayerIndex, int destLayerIndex, ConnectionInfo (*pattern)(int, int) = 0)
   {
      Layer::makeConnection(mLayers[sourceLayerIndex], mLayers[destLayerIndex], pattern);
   }

   //Network control methods
   int  getTime() {return mTime;}
   float getDAConcentraion() {return (mDAHandler)?mDAHandler->getDAConcentraion():-1;}
   int  getNextSynapseID() { return ++mLastSynapseID; }
   void addDAModule();
   void runNetwork(int maxTime);
   void logLayerActivity(int layer);
   void logSettings();
   void logPostSynapseWeights(int layer, int neuron);
   void logPreSynapseWeights(int layer, int neuron);
   void logSynapseWeight(bool (*pattern)(int, int, int, int));
   void logSynapseWeight(bool (*pattern)(int));
   void logPotential(int layer, bool (*pattern)(int) = 0) { mLayers[layer]->logPotential(pattern); }

   void setInputPattern(int layerIndex, InputPatternMode mode, std::vector<InputInformation> (*pattern)(int) = 0)
   {mLayers[layerIndex]->setInputPattern(mode, pattern);}

   void setSTDPParameters(int layerIndex, float CMultiplier, float AP, float AN, int STDPTimeStep = 100, float TaoP = 20, float TaoN = 20)
   { mLayers[layerIndex]->setSTDPParameters(CMultiplier, AP, AN, STDPTimeStep, TaoP, TaoN); }

   void setSTDPParameters(float CMultiplier, float AP, float AN, int STDPTimeStep = 100, float TaoP = 20, float TaoN = 20)
   {
      for (size_t i = 0; i < mLayers.size(); ++i)
         mLayers[i]->setSTDPParameters(CMultiplier, AP, AN, STDPTimeStep, TaoP, TaoN);

      //reset default parameters
      mCMultiplier = CMultiplier;
      mAP = AP;
      mAN = AN;
      mSTDPTimeStep = STDPTimeStep;
      mTaoP = TaoP;
      mTaoN = TaoN;
   }


   void setBoundingParameters(int layerIndex, float maxWeight, float minRandWeight, 
      float maxRandWeight, int minRandDelay, int maxRandDelay)
   { mLayers[layerIndex]->setBoundingParameters(maxWeight, minRandWeight, maxRandWeight, minRandDelay, maxRandDelay); }

   void setBoundingParameters(float maxWeight, float minRandWeight, 
      float maxRandWeight, int minRandDelay, int maxRandDelay)
   {
      for (size_t i = 0; i < mLayers.size(); ++i)
         mLayers[i]->setBoundingParameters(maxWeight, minRandWeight, maxRandWeight, minRandDelay, maxRandDelay);

      //reset default parameters
      mMaxWeight = maxWeight;
      mMaxRandWeight = maxRandWeight;
      mMinRandWeight = minRandWeight;
      mMaxRandDelay = maxRandDelay;
      mMinRandDelay = minRandDelay;
   }

   void setCurrentParameters(int layerIndex, float MinInputCurrent, float MaxInputCurrent)
   { mLayers[layerIndex]->setCurrentParameters(MinInputCurrent, MaxInputCurrent); }

   void setLearningFlag(int layerIndex, bool learningFlag)
   { mLayers[layerIndex]->setLearningFlag(learningFlag); }

   void setContainerFlag(int layerIndex, bool containerFlag)
   { mLayers[layerIndex]->setContainerFlag(containerFlag); }

   Logger mLogger;
   bool   mLogSettingsFlag;

   virtual ConnectionInfo defaultConnectingPattern(int sourceIndex, int destIndex);
   virtual std::vector<InputInformation> defaultInputPattern(int time);
   virtual std::string getAddress(int slayer, int sneuron = -1, int dlayer = -1, int dneuron = -1);

protected:
   int                  mTime;
   std::vector<Layer*>  mLayers;
   DAHandler*           mDAHandler;
   int                  mLastLayerID;
   int                  mLastSynapseID;

   //default STDP settings
   float mAP;    //max of positive part of STDP function
   float mAN;    //min of negative part of STDP function
   float mTaoP;  //determines convergence pace of positive part of STDP function
   float mTaoN;  //determines convergence pace of negative part of STDP function
   float mCMultiplier;  //a constant which multiplies variable mC of every synapse, every mSTDPTimeStep milisecs
   int   mSTDPTimeStep;

   //default bounding parameters
   float mMaxWeight;
   float mMaxRandWeight;
   float mMinRandWeight;
   int   mMaxRandDelay;
   int   mMinRandDelay;

   //default input current parameters
   float mMinInputCurrent;
   float mMaxInputCurrent;


   float mDefaultConnectionProbability;
};

template <class NeuronTemp>
bool Network::addNeuron(int layerIndex, int neuronNum, ChannelType type, ParameterContainer* params)
{
   if (type != DEPENDENT)
      mLayers[layerIndex]->addNeuron<NeuronTemp>(neuronNum, type, params);
   else
   {
      //TODO: throw error!
      return false;
   }

   return true;
}

#endif