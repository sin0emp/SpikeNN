#ifndef LAYER_H
#define LAYER_H

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

#include "Neuron.h"
#include "IzhikevichNeuron.h"
#include "GlobalVars.h"
#include <vector>
#include <map>

namespace boost{ namespace serialization { class access; } namespace archive { class text_oarchive; } }
class Network;
struct SpikeInfo;

class MODULE_EXPORT Layer
{
   friend class boost::serialization::access;
   friend class DAHandler;
public:
   Layer(Network* net, int ID, bool shouldLearn = true, bool isContainer = false);
   void wakeup(); //used to set dependent parameters loading
   ~Layer();

   template <class NeutonTemp>
   void addNeuron(int neuronNum, ChannelType type = EXCITATORY, ParameterContainer* params = 0);
   void setInputPattern(InputPatternMode mode, std::vector<InputInformation> (*pattern)(int))
   { mInputPatternMode = mode; mInputPattern = pattern; }

   void update();
   std::vector<int> getWeightFrequencies();

   void logWeight(bool (*pattern)(int) = 0);
   void logWeight(bool (*pattern)(int, int, int, int) = 0);
   void logPotential(bool (*pattern)(int) = 0);
   void logActivity() { mLogActivityFlag = true; }
   void logPostSynapseWeight(int neuron, std::string directory = "");
   void logPreSynapseWeight(int neuron, std::string directory = "");

   int  getTime() { return *mTime; }
   bool getContainerFlag() { return mContainerFlag; }
   void setContainerFlag(bool flag) { mContainerFlag = flag; }

   bool getExcitatoryLearningFlag() { return mExLearningFlag; }
   void setExcitatoryLearningFlag(bool flag) { mExLearningFlag = flag; updateLearningFlags(); }
   bool getInhibitoryLearningFlag() { return mInLearningFlag; }
   void setInhibitoryLearningFlag(bool flag) { mInLearningFlag = flag; updateLearningFlags(); }
   bool getLockExcitatoryLearningFlag() { return mLockExLearningFlag; }
   void setExcitatoryLearningLock(bool flag) { mLockExLearningFlag = flag; updateLearningFlags(); }
   bool getLockInhibitoryLearningFlag() { return mLockInLearningFlag; }
   void setInhibitoryLearningLock(bool flag) { mLockInLearningFlag = flag; updateLearningFlags(); }
   bool shouldExcitatoryLearn() { return mExShouldLearn; }
   bool shouldInhibitoryLearn() { return mInShouldLearn; }
   
   void recordSpike(int NeuronID);
   
   void setSTDPParameters(float CMultiplier, float AP, float AN, float decayMultiplier = 1,
      int STDPTimeStep = 100, float TaoP = 20, float TaoN = 20)
   { mCMultiplier = CMultiplier; mAP = AP; mAN = AN; mDecayWeightMultiplier = decayMultiplier,
     mSTDPTimeStep = STDPTimeStep; mTaoP = TaoP; mTaoN = TaoN; }

   void setBoundingParameters(float exMaxWeight, float inMaxWeight, float exMinRandWeight, 
      float exMaxRandWeight, float inMinRandWeight, float inMaxRandWeight,
      int minRandDelay, int maxRandDelay);

   void setCurrentParameters(float minInputCurrent, float maxInputCurrent)
   { mMinInputCurrent = minInputCurrent; mMaxInputCurrent = maxInputCurrent; }

   int   getID() { return mID; }
   float getExcitatoryMaxWeight() { return mExMaxWeight; }
   float getExcitatoryMinRandWeight() { return mExMinRandWeight; }
   float getExcitatoryMaxRandWeight() { return mExMaxRandWeight; }
   float getInhibitoryMaxWeight() { return mInMaxWeight; }
   float getInhibitoryMinRandWeight() { return mInMinRandWeight; }
   float getInhibitoryMaxRandWeight() { return mInMaxRandWeight; }
   int   getMinRandDelay() { return mMinRandDelay; }
   int   getMaxRandDelay() { return mMaxRandDelay; }
   float getAP() { return mAP; }
   float getAN() { return mAN; }
   float getTaoP() { return mTaoP; }
   float getTaoN() { return mTaoN; }
   float getCMultiplier() { return mCMultiplier; }
   float getDecayMultiplier() { return mDecayWeightMultiplier; }
   int   getNextSynapseID();
   float getDAConcentraion();
   const int* getPointerToTime() { return mTime; }
   void  restNeurons();
   std::string getAddress(int slayer, int sneuron = -1, int dlayer = -1, int dneuron = -1);
   std::vector<float> getResponseFromLayer(int sourceLayer, int destNeuron)
   { return mNeurons[destNeuron]->getResponseFromLayer(sourceLayer); }

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

private:
   Network*                      mNetwork;
   const int*                    mTime;
   int                           mID;
   std::vector<Neuron*>          mNeurons;
   std::vector<Synapse*>         mSynapses;
   std::vector<SpikeInfo>        mSpikes;
   InputPatternMode              mInputPatternMode;
   std::vector<InputInformation> (*mInputPattern)(int);

   //flags to control learning
   bool                          mExLearningFlag;
   bool                          mInLearningFlag;
   bool                          mLockExLearningFlag;
   bool                          mLockInLearningFlag;
   //these two will be computed automatically to avoid unneccessary calculations
   bool                          mExShouldLearn;
   bool                          mInShouldLearn;
   void updateLearningFlags() { mExShouldLearn = mExLearningFlag && !mLockExLearningFlag;
                                mInShouldLearn = mInLearningFlag && !mLockInLearningFlag; }

   bool                          mContainerFlag;
   bool                          mLogActivityFlag;
   Logger                        mLogger;

   //STDP settings
   float mAP;                    //max of positive part of STDP function
   float mAN;                    //min of negative part of STDP function
   float mTaoP;                  //determines convergence pace of positive part of STDP function
   float mTaoN;                  //determines convergence pace of negative part of STDP function
   float mCMultiplier;           //a constant which multiplies variable mC of every synapse, every mSTDPTimeStep milisecs
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
   //bounding parameters for input currents
   float mMinInputCurrent;
   float mMaxInputCurrent;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
   Layer() { initialize(); } //used only by boost::serialization
   void initialize(); // default parameters
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

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version)
   {
      ar & mNeuronID & mTime;
   }
};
#endif