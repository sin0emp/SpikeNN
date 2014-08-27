#include "Layer.h"
#include "Network.h"
#include <stdlib.h>
#include <iostream>

#define RANDOMINPUT ((float)rand()/RAND_MAX)*(mMaxInputCurrent - mMinInputCurrent) + mMinInputCurrent

Layer::Layer(Network* net, int ID, bool shouldLearn, bool isContainer)
{
   mNetwork = net;
   mInputPatternMode = NO_INPUT;
   mInputPattern = 0;
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
   mLearningFlag = true;
   mContainerFlag = false;
   mLogActivityFlag = false;
   mContainerFlag = isContainer;
   mLearningFlag = shouldLearn;
   mID = ID;
}

Layer::~Layer()
{
   for (std::size_t i = 0; i < mNeurons.size(); ++i) 
      delete mNeurons[i];
}

void Layer::update()
{
   mTime = mNetwork->getTime();

   size_t k;
   switch (mInputPatternMode)
   {
   case NO_INPUT:
      break;

   case ALL_RANDOM_CURRENT:
      for (k = 0; k < mNeurons.size(); k++)
         mNeurons[k]->addInputCurrent(mTime, RANDOMINPUT);
      break;

   case ALL_MAX_CURRENT:
      for (k = 0; k < mNeurons.size(); k++)
         mNeurons[k]->addInputCurrent(mTime, RANDOMINPUT);
      break;

   case ONE_RANDOM_CURRENT:
      k = rand() % mNeurons.size();
      mNeurons[k]->addInputCurrent(mTime, RANDOMINPUT);
      break;

   case ONE_MAX_CURRENT:
      k = rand() % mNeurons.size();
      mNeurons[k]->addInputCurrent(mTime, mMaxInputCurrent);
      break;

   case MANUAL_INPUT:
      {
      std::vector<InputInformation> infos = (mInputPattern) ? 
         (*mInputPattern)(mTime) :
         mNetwork->defaultInputPattern(mTime);

      for (size_t i = 0; i < infos.size(); ++i)
      {
         switch (infos[i].mInputMode)
         {
         case FORCE_FIRE:
            mNeurons[infos[i].mNeuronIndex]->propagateSpike();
            break;

         case RANDOM_CURRENT:
            mNeurons[infos[i].mNeuronIndex]->addInputCurrent(mTime, RANDOMINPUT);
            break;

         case MAX_CURRENT:
            mNeurons[infos[i].mNeuronIndex]->addInputCurrent(mTime, mMaxInputCurrent);
            break;

         case MANUAL_CURRENT:
            mNeurons[infos[i].mNeuronIndex]->addInputCurrent(mTime, infos[i].mManualCurrent);
            break;

         default:
            break;
         }
      }
      break;
      }
   default:
      break;
   }

   if (!mContainerFlag)
      for (size_t i = 0; i < mNeurons.size(); ++i)
         mNeurons[i]->update();

   if (mLearningFlag)
      if (mTime % mSTDPTimeStep == 0)
         applyWeightChanges();

   if (mLogActivityFlag)
      if (mTime % 60000 == 0)
      {
         mLogger.set("Layer" + Logger::toString((float)mID) + "Min" + Logger::toString((float)(mTime / 60000)));

         std::string s = "";
         for (size_t i = 0; i < mSpikes.size(); ++i)
            s += (Logger::toString((float)mSpikes[i].mTime) + " " + Logger::toString((float)mSpikes[i].mNeuronID) + "\n");

         mSpikes.clear();

         if(s != "")
            mLogger.write(s);
      }
}

void Layer::makeConnection(Layer* source, Layer* dest, float synapseProb, float excitatoryWeight,
      float inhibitoryWeight, int excitatoryDelay, int inhibitoryDelay)
{
   //TODO: throw error if the two layers dosn't belong to a same network
   
   for (std::size_t i = 0; i < source->mNeurons.size(); ++i)
      for (std::size_t j = 0; j < dest->mNeurons.size(); ++j)
      {
         float f = float(rand() / RAND_MAX);
         if (f < synapseProb)
         {
            if (source->mNeurons[i]->getType() == EXCITATORY)
            {
               Synapse* syn = Neuron::makeConnection(source->mNeurons[i], dest->mNeurons[j], excitatoryWeight, excitatoryDelay);
               source->mSynapses.push_back(syn);
               if (source != dest)
                  dest->mSynapses.push_back(syn);
            }
            else
            {
               Synapse* syn = Neuron::makeConnection(source->mNeurons[i], dest->mNeurons[j], inhibitoryWeight, inhibitoryDelay);
               source->mSynapses.push_back(syn);
               //if (source != dest)
               //   dest->mSynapses.push_back(syn);
            }
         }
      }
}

void Layer::makeConnection(Layer* source, Layer* dest,
      int neuronsNumToConnect, float excitatoryWeight, float inhibitoryWeight,
      int excitatoryDelay, int inhibitoryDelay)
{
   for (size_t i = 0; i < source->mNeurons.size(); ++i)
   {
      for (int j = 0; j < neuronsNumToConnect; ++j)
      {
         int k = rand() % dest->mNeurons.size();

         //no self or multiple or inhibitory to inhibitory connection!
         if (i == k || source->mNeurons[i]->isConnectedTo(source->mNeurons[k])
            || (source->mNeurons[i]->getType() == INHIBITORY && dest->mNeurons[k]->getType() == INHIBITORY))
         {
            --j;
            continue;
         }

         if (source->mNeurons[i]->getType() == EXCITATORY)
         {
            Synapse* syn = Neuron::makeConnection(source->mNeurons[i], dest->mNeurons[k], excitatoryWeight, excitatoryDelay);
            source->mSynapses.push_back(syn);
            if (source != dest)
               dest->mSynapses.push_back(syn);
         }
         else
         {
            Synapse* syn = Neuron::makeConnection(source->mNeurons[i], dest->mNeurons[k], inhibitoryWeight, inhibitoryDelay);
            source->mSynapses.push_back(syn);
            //if (source != dest)
            //   dest->mSynapses.push_back(syn);
         }
      }
   }
}

void Layer::makeConnection(Layer* source, Layer* dest, ConnectionInfo (*pattern)(int, int))
{
   for (size_t i = 0; i < source->mNeurons.size(); ++i)
      for (size_t j = 0; j < dest->mNeurons.size(); ++j)
      {
         ConnectionInfo info = (pattern) ? (*pattern)(i, j) : source->mNetwork->defaultConnectingPattern(i, j);
         if (info.mConnectFlag)
         {
            Synapse* syn = Neuron::makeConnection(source->mNeurons[i], dest->mNeurons[j], info.mWeight, info.mDelay, info.mType);
            source->mSynapses.push_back(syn);

            //is it right??
            //if (source != dest)
            //   dest->mSynapses.push_back(syn);
         }
      }
}

void Layer::applyWeightChanges()
{
   int t = mNetwork->getTime();
   for (size_t i = 0; i < mSynapses.size(); ++i)
   {
      if(mSynapses[i]->mType == EXCITATORY) mSynapses[i]->updateWeight();

      if (t % 1000 == 0 && mSynapses[i]->mLogWeightFlag)
         mSynapses[i]->mLogger.writeLine(Logger::toString((float)t) + " " + Logger::toString(mSynapses[i]->mWeight));
   }
}

void Layer::logWeight(bool (*pattern)(int))
{
   for (size_t i = 0; i < mSynapses.size(); ++i)
   {
      mSynapses[i]->logWeight(pattern);
   }
}

void Layer::logWeight(bool (*pattern)(int, int, int, int))
{
   for (size_t i = 0; i < mSynapses.size(); ++i)
   {
      mSynapses[i]->logWeight(pattern);
   }
}

void Layer::logPotential(bool (*pattern)(int))
{
   for (size_t i = 0; i < mNeurons.size(); ++i)
   {
      if(pattern)
         if(!(*pattern)(i))
            continue;

      mNeurons[i]->logPotential();
   }
}

void Layer::logPostSynapseWeight(int neuron)
{
   mNeurons[neuron]->logPostSynapseWeight();
}

void Layer::logPreSynapseWeight(int neuron)
{
   mNeurons[neuron]->logPreSynapseWeight();
}


std::vector<int> Layer::getWeightFrequencies()
{
   std::vector<int> freq ((unsigned int)(mMaxWeight / 0.2)+1, 0);
   for (size_t i = 0; i < mSynapses.size(); ++i)
      freq[(unsigned int)(mSynapses[i]->mWeight / 0.2)]++;

   return freq;
}

void Layer::recordSpike(int NeuronID)
{
   if (mLogActivityFlag)
   {
      SpikeInfo s = {NeuronID, mNetwork->getTime()};
      mSpikes.push_back(s);
   }
}

float Layer::getDAConcentraion()
{
   return mNetwork->getDAConcentraion();
}

int Layer::getNextSynapseID()
{
   return mNetwork->getNextSynapseID();
}

void Layer::setBoundingParameters(float maxWeight, float minRandWeight, 
      float maxRandWeight, int minRandDelay, int maxRandDelay)
{
   if (maxWeight != -1) mMaxWeight = maxWeight;
   if (maxRandWeight != -1) mMaxRandWeight = maxRandWeight;
   if (minRandWeight != -1) mMinRandWeight = minRandWeight;
   if (maxRandDelay != -1) mMaxRandDelay = maxRandDelay;
   if (minRandDelay != -1) mMinRandDelay = minRandDelay;
}

void Layer::restNeurons()
{
   for (size_t i = 0; i < mNeurons.size(); ++i)
      mNeurons[i]->rest();
}

std::string Layer::getAddress(int slayer, int sneuron, int dlayer, int dneuron)
{
   return mNetwork->getAddress(slayer, sneuron, dlayer, dneuron);
}