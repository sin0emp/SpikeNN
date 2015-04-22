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
#include "DAHandler.h"
#include <vector>
#include <map>

namespace boost{ namespace serialization { class access; } namespace archive { class text_oarchive; } }
class Network;
struct SpikeInfo;
struct STDPchangeRequest;

class MODULE_EXPORT Layer
{
   friend class boost::serialization::access;
public:
   Layer(Network* net, int ID, bool shouldLearn = true, bool isContainer = false);
   Layer() { initialize(); } //used only by boost::serialization
   void wakeup(); //used to set dependent parameters loading
   ~Layer();

   template <class NeutonTemp>
   void addNeuron(int neuronNum, ChannelType type = EXCITATORY, ParameterContainer* params = 0);
   void setInputPattern(InputPatternMode mode, std::vector<InputInformation> (*pattern)(float))
   { mInputPatternMode = mode; mInputPattern = pattern; }

   void update();
   std::vector<int> getWeightFrequencies();

   void logWeight(bool (*pattern)(int) = 0);
   //void logWeight(bool (*pattern)(int, int, int, int) = 0);
   void logPotential(bool (*pattern)(int) = 0);
   void logActivity() { mLogActivityFlag = true; }
   //void logPostSynapseWeight(int neuron, std::string directory = "");
   //void logPreSynapseWeight(int neuron, std::string directory = "");

   const Synapse* getSynapse(int synapseID);
   float getTime() { return *mTime; }
   float getTimeStep() {return *mTimeStep; }
   bool  getContainerFlag() { return mContainerFlag; }
   void  setContainerFlag(bool flag) { mContainerFlag = flag; }
   int   getNeuronsNumber() { return mNeurons.size(); }
   bool  getExcitatoryLearningFlag() { return mExLearningFlag; }
   void  setExcitatoryLearningFlag(bool flag) { mExLearningFlag = flag; updateLearningFlags(); }
   bool  getInhibitoryLearningFlag() { return mInLearningFlag; }
   void  setInhibitoryLearningFlag(bool flag) { mInLearningFlag = flag; updateLearningFlags(); }
   bool  getLockExcitatoryLearningFlag() { return mLockExLearningFlag; }
   void  setExcitatoryLearningLock(bool flag) { mLockExLearningFlag = flag; updateLearningFlags(); }
   bool  getLockInhibitoryLearningFlag() { return mLockInLearningFlag; }
   void  setInhibitoryLearningLock(bool flag) { mLockInLearningFlag = flag; updateLearningFlags(); }
   bool  shouldExcitatoryLearn() { return mExShouldLearn; }
   bool  shouldInhibitoryLearn() { return mInShouldLearn; }
   
   virtual void recordSpike(int NeuronID); //virtual for DAHandler notifications
   
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
   float getDAConcentraion() { return (mDAHandler) ? mDAHandler->getDAConcentraion() : -1; }
   float getDecayMultiplier() { return mDecayWeightMultiplier; }
   bool  getSharedConnectionFlag() { return mSharedConnectionFlag; }
   int   getNextSynapseID();
   //float getDAConcentraion() {return (mDAHandler)?mDAHandler->getDAConcentraion():-1; }
   void addDAModule(DAHandler* handler) { mDAHandler = handler; handler->setLayer(this); };
   const float* getPointerToTime() { return mTime; }
   const float* getPointerToTimeStep() { return mTimeStep; }
   void  restNeurons();
   std::string getAddress(int slayer, int sneuron = -1, int dlayer = -1, int dneuron = -1);
   std::vector<float> getResponseFromLayer(int sourceLayer, int destNeuron)
   { return mNeurons[destNeuron]->getResponseFromLayer(sourceLayer); }

   void shareConnection(size_t sourceNeuron=0, int sharingTimeStep=40);
   void shareConnection(std::vector<SynapseBase*> bases, int sharingTimeStep=40);
   void setSharedWeights(std::vector<float> weights)
   { for(size_t i=0; i<weights.size(); ++i) mSharedConnections[i]->mWeight = weights[i]; }
   //void giveChangeRequest(STDPchangeRequest request);

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

   int                             mSharedWinnerID;            //the ID of the neuron that is allowed to changed
                                                               //shared synapses during a time-step.
protected:
   Network*                        mNetwork;
   DAHandler*                      mDAHandler;
   const float*                    mTime;
   const float*                    mTimeStep;
   int                             mID;
   std::vector<Neuron*>            mNeurons;
   std::vector<Synapse*>           mSynapses;
   std::vector<Synapse*>           mSynapsesToLog;
   std::vector<SpikeInfo>          mSpikes;
   InputPatternMode                mInputPatternMode;
   std::vector<InputInformation> (*mInputPattern)(float);
   bool                            mSharedConnectionFlag;
   std::vector<SynapseBase*>       mSharedConnections;
   int                             mSharedConnectionTimeStep;  //a time step which determines how much time
                                                               //the layer should wait for neurons' responces
                                                               //to recognise the winner and let it to increase
                                                               //or decrease the shared weights.

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
   void flushActivity();

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
   int   mNeuronID;
   float mTime;
};

struct STDPchangeRequest
{
   int mNeuronID;
   int mDt;
   SynapseBase* mSynapse;
};

#endif