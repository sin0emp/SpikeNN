#ifndef LAYER_H
#define LAYER_H

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

#include "Neuron.h"
#include "IzhikevichNeuron.h"
#include "GlobalVars.h"
#include <vector>
#include <map>

class Network;
struct SpikeInfo;

class MODULE_EXPORT Layer
{
   friend class DAHandler;
public:
   Layer(Network* net, int ID, bool shouldLearn = true, bool isContainer = false);
   ~Layer();

   template <class NeutonTemp>
   void addNeuron(int neuronNum, ChannelType type = EXCITATORY, ParameterContainer* params = 0);
   void setInputPattern(InputPatternMode mode, std::vector<InputInformation> (*pattern)(int))
   { mInputPatternMode = mode; mInputPattern = pattern; }

   void update();
   void applyWeightChanges();
   std::vector<int> getWeightFrequencies();

   void logWeight(bool (*pattern)(int) = 0);
   void logWeight(bool (*pattern)(int, int, int, int) = 0);
   void logPotential(bool (*pattern)(int) = 0);
   void logActivity() { mLogActivityFlag = true; }
   void logPostSynapseWeight(int neuron);
   void logPreSynapseWeight(int neuron);

   int  getTime() { return mTime; }
   bool getContainerFlag() { return mContainerFlag; }
   bool getLearningFlag() { return mLearningFlag; }
   void setLearningFlag(bool learningFlag) { mLearningFlag = learningFlag; }
   void setContainerFlag(bool containerFlag) { mContainerFlag = containerFlag; }
   void recordSpike(int NeuronID);
   
   void setSTDPParameters(float CMultiplier, float AP, float AN, int STDPTimeStep = 100, float TaoP = 20, float TaoN = 20)
   { mCMultiplier = CMultiplier; mAP = AP; mAN = AN; mSTDPTimeStep = STDPTimeStep; mTaoP = TaoP; mTaoN = TaoN; }

   void setBoundingParameters(float maxWeight, float minRandWeight, 
      float maxRandWeight, int minRandDelay, int maxRandDelay);

   void setCurrentParameters(float MinInputCurrent, float MaxInputCurrent)
   { mMinInputCurrent = MinInputCurrent; mMaxInputCurrent = MaxInputCurrent; }

   int   getID() { return mID; }
   float getMaxWeight() { return mMaxWeight; }
   float getMinRandWeight() { return mMinRandWeight; }
   float getMaxRandWeight() { return mMaxRandWeight; }
   int   getMinRandDelay() { return mMinRandDelay; }
   int   getMaxRandDelay() { return mMaxRandDelay; }
   float getAP() { return mAP; }
   float getAN() { return mAN; }
   float getTaoP() { return mTaoP; }
   float getTaoN() { return mTaoN; }
   float getCMultiplier() { return mCMultiplier; }
   int   getNextSynapseID();
   float getDAConcentraion();
   void  restNeurons();
   std::string getAddress(int slayer, int sneuron = -1, int dlayer = -1, int dneuron = -1);

   //static std::vector<InputInformation> allRandomInputPattern(int time);
   //static std::vector<InputInformation> oneRandomInputPattern(int time);

   static void makeConnection(Layer* source, Layer* dest, float synapseProb, float excitatoryWeight = -1.0f,
      float inhibitoryWeight = -1.0f, int excitatoryDelay = -1.0f, int inhibitoryDelay = -1.0f);

   static void makeConnection(Layer* source, Layer* dest,
      int neuronsNumToConnect, float excitatoryWeight = -1.0f, float inhibitoryWeight = -1.0f,
      int excitatoryDelay = -1.0f, int inhibitoryDelay = -1.0f);

   static void makeConnection(Layer* source, int sourceNeuronIndex, Layer* dest, 
      int destNeuronIndex, float weight = -1, int delay = -1)
   { Neuron::makeConnection(source->mNeurons[sourceNeuronIndex], dest->mNeurons[destNeuronIndex], weight, delay); }

   static void makeConnection(Layer* source, Layer* dest, ConnectionInfo (*pattern)(int, int) = 0);

public:
   Network*                      mNetwork;
   int                           mTime;
   int                           mID;
   std::vector<Neuron*>          mNeurons;
   std::vector<Synapse*>         mSynapses;
   std::vector<SpikeInfo>        mSpikes;
   InputPatternMode              mInputPatternMode;
   std::vector<InputInformation> (*mInputPattern)(int);
   bool                          mLearningFlag;
   bool                          mContainerFlag;
   bool                          mLogActivityFlag;
   Logger                        mLogger;

   //STDP settings
   float mAP;    //max of positive part of STDP function
   float mAN;    //min of negative part of STDP function
   float mTaoP;  //determines convergence pace of positive part of STDP function
   float mTaoN;  //determines convergence pace of negative part of STDP function
   float mCMultiplier;  //a constant which multiplies variable mC of every synapse, every mSTDPTimeStep milisecs
   int   mSTDPTimeStep;

   //bounding parameters
   float mMaxWeight;
   float mMaxRandWeight;
   float mMinRandWeight;
   int   mMaxRandDelay;
   int   mMinRandDelay;

   //input current parameters
   float mMinInputCurrent;
   float mMaxInputCurrent;
};

template <class NeuronTemp>
void Layer::addNeuron(int neuronNum, ChannelType type, ParameterContainer* params)
{
   for (int i = 0; i < neuronNum; ++i)
      mNeurons.push_back(new NeuronTemp(this, mNeurons.size(), type, params));
}

struct SpikeInfo
{
   int mNeuronID;
   int mTime;
};
#endif