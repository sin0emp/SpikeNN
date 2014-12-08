#ifndef NETWORK_H
#define NETWORK_H

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

#include "Layer.h"
#include "Logger.h"
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "GlobalVars.h"
#include "DAHandler.h"

namespace boost{ namespace serialization { class access; } namespace archive { class text_oarchive; } }

class MODULE_EXPORT Network
{
   friend class boost::serialization::access;
   friend class DAHandler;
public:
   Network();
   virtual ~Network();

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

   void shareConnection(int layer, size_t sourceNeuron=0, int sharingTimeStep=40) 
   { mLayers[layer]->shareConnection(sourceNeuron, sharingTimeStep); }
   void shareConnection(int layer, std::vector<SynapseBase*> bases, int sharingTimeStep=40)
   { mLayers[layer]->shareConnection(bases, sharingTimeStep); }

   //Network control methods
   int  getTime() { return mTime; }
   const int* getPointerToTime() { return &mTime; }
   float getDAConcentraion() {return (mDAHandler)?mDAHandler->getDAConcentraion():-1; }
   int  getNextSynapseID() { return ++mLastSynapseID; }
   void addDAModule();
   void runNetwork(int maxTime);
   void logLayerActivity(int layer);
   void logSettings();
   //void logPostSynapseWeights(int layer, int neuron, std::string directory = "");
   //void logPreSynapseWeights(int layer, int neuron, std::string directory = "");
   //void logSynapseWeight(bool (*pattern)(int, int, int, int));
   //void logSynapseWeight(bool (*pattern)(int));
   void logPotential(int layer, bool (*pattern)(int) = 0) { mLayers[layer]->logPotential(pattern); }

   void setInputPattern(int layerIndex, InputPatternMode mode, std::vector<InputInformation> (*pattern)(int) = 0)
   {mLayers[layerIndex]->setInputPattern(mode, pattern);}

   void setSTDPParameters(int layerIndex, float CMultiplier, float AP, float AN, float decayMultiplier = 1,
      int STDPTimeStep = 100, float TaoP = 20, float TaoN = 20)
   { mLayers[layerIndex]->setSTDPParameters(CMultiplier, AP, AN, decayMultiplier, STDPTimeStep, TaoP, TaoN); }

   void setDefaultSTDPParameters(float CMultiplier, float AP, float AN, float decayMultiplier = 1, 
      int STDPTimeStep = 100, float TaoP = 20, float TaoN = 20)
   {
      //reset default parameters
      mCMultiplier = CMultiplier;
      mAP = AP;
      mAN = AN;
      mDecayWeightMultiplier = decayMultiplier;
      mSTDPTimeStep = STDPTimeStep;
      mTaoP = TaoP;
      mTaoN = TaoN;
   }


   void setBoundingParameters(int layerIndex, float exMaxWeight, float inMaxWeight, float exMinRandWeight, 
                              float exMaxRandWeight, float inMinRandWeight, float inMaxRandWeight,
                              int minRandDelay, int maxRandDelay)
   { mLayers[layerIndex]->setBoundingParameters(exMaxWeight, inMaxWeight, exMinRandWeight, 
                            exMaxRandWeight, inMinRandWeight, inMaxRandWeight, minRandDelay, maxRandDelay); }

   void setDefaultBoundingParameters(float exMaxWeight, float inMaxWeight, float exMinRandWeight, 
                              float exMaxRandWeight, float inMinRandWeight, float inMaxRandWeight,
                              int minRandDelay, int maxRandDelay)
   {
      //reset default parameters
      if (exMaxWeight != -1) mExMaxWeight = exMaxWeight;
      if (exMaxRandWeight != -1) mExMaxRandWeight = exMaxRandWeight;
      if (exMinRandWeight != -1) mExMinRandWeight = exMinRandWeight;
      if (inMaxWeight != -1) mInMaxWeight = inMaxWeight;
      if (inMaxRandWeight != -1) mInMaxRandWeight = inMaxRandWeight;
      if (inMinRandWeight != -1) mInMinRandWeight = inMinRandWeight;
      if (maxRandDelay != -1) mMaxRandDelay = maxRandDelay;
      if (minRandDelay != -1) mMinRandDelay = minRandDelay;
   }

   void setCurrentParameters(int layerIndex, float MinInputCurrent, float MaxInputCurrent)
   { mLayers[layerIndex]->setCurrentParameters(MinInputCurrent, MaxInputCurrent); }

   void setExcitatoryLearningFlag(int layerIndex, bool flag)
   { mLayers[layerIndex]->setExcitatoryLearningFlag(flag); }

   void setIInhibitoryLearningFlag(int layerIndex, bool flag)
   { mLayers[layerIndex]->setInhibitoryLearningFlag(flag); }

   void setExcitatoryLearningLock(int layerIndex, bool flag)
   { mLayers[layerIndex]->setExcitatoryLearningLock(flag); }

   void setInhibitoryLearningLock(int layerIndex, bool flag)
   { mLayers[layerIndex]->setInhibitoryLearningLock(flag); }

   void setContainerFlag(int layerIndex, bool containerFlag)
   { mLayers[layerIndex]->setContainerFlag(containerFlag); }

   void setExcitatoryLearningLock(bool flag)
   {
      for (size_t i = 0; i < mLayers.size(); ++i)
         mLayers[i]->setExcitatoryLearningLock(flag);
   }

   void setInhibitoryLearningLock(bool flag)
   {
      for (size_t i = 0; i < mLayers.size(); ++i)
         mLayers[i]->setInhibitoryLearningLock(flag);
   }

   void setLearningLock(bool flag)
   {
      setExcitatoryLearningLock(flag); setInhibitoryLearningLock(flag);
   }

   std::vector<float> getResponseFromLayer(int sourceLayer, int destLayer, int destNeuron)
   { return mLayers[destLayer]->getResponseFromLayer(sourceLayer, destNeuron); }

   virtual ConnectionInfo defaultConnectingPattern(int sourceIndex, int destIndex);
   virtual std::vector<InputInformation> defaultInputPattern(int time);
   virtual std::string getAddress(int slayer, int sneuron = -1, int dlayer = -1, int dneuron = -1);

   void saveNetwork(std::string path);
   static Network* loadNetwork(std::string path);

protected:
   int                  mTime;
   std::vector<Layer*>  mLayers;
   DAHandler*           mDAHandler;
   int                  mLastLayerID;
   int                  mLastSynapseID;
   Logger               mLogger;
   bool                 mLogSettingsFlag;


   //default STDP settings
   float mAP;    //max of positive part of STDP function
   float mAN;    //min of negative part of STDP function
   float mTaoP;  //determines convergence pace of positive part of STDP function
   float mTaoN;  //determines convergence pace of negative part of STDP function
   float mCMultiplier;  //a constant which multiplies variable mC of every synapse, every mSTDPTimeStep milisecs
   float mDecayWeightMultiplier; //a constant which multiplies weight of every synapse, every mSTDPTimeStep milisecs
   int   mSTDPTimeStep;

   //bounding parameters for excitatory connections
   float mExMaxWeight;
   float mExMaxRandWeight;
   float mExMinRandWeight;
   //bounding parameters for inhibitory connections
   float mInMaxWeight;
   float mInMaxRandWeight;
   float mInMinRandWeight;
   //bounding parameters for connections' delay
   int   mMaxRandDelay;
   int   mMinRandDelay;

   //default input current parameters
   float mMinInputCurrent;
   float mMaxInputCurrent;

   float mDefaultConnectionProbability;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
   void initialize();
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