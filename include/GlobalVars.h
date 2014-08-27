#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <vector>

#define CHOOSE_RANDOM -1

enum ChannelType
{
   EXCITATORY = 0,
   INHIBITORY,
   DEPENDENT
};

enum LoggingBehavior
{
   NETWORK_ACTIVITY = 0,
   NETWORK_SETTINGS,
   NETWORK_TIME,
   SYNAPSE_WIGHT_FREQUENCIES,
   SYNAPSE_WEIGHT,
   NEURON_POTENTIAL,
   REWARD_TIMES
};

enum CurrentMode
{
   FORCE_FIRE = 0,
   RANDOM_CURRENT,
   MAX_CURRENT,
   MANUAL_CURRENT
};

enum InputPatternMode
{
   ALL_RANDOM_CURRENT = 0,
   ALL_MAX_CURRENT,
   ONE_RANDOM_CURRENT,
   ONE_MAX_CURRENT,
   NO_INPUT,
   MANUAL_INPUT
};

struct ParameterContainer
{
   std::vector<float> mParams;
};

struct InputInformation
{
   InputInformation(int neuronIndex, CurrentMode inputMode, float manualCurrent = 0):
      mInputMode(inputMode), mNeuronIndex(neuronIndex), mManualCurrent(manualCurrent) {}

   CurrentMode mInputMode;
   int         mNeuronIndex;
   float       mManualCurrent;
};

struct ConnectionInfo
{
   ConnectionInfo(bool connectFlag, ChannelType type = EXCITATORY, float weight = -1, int delay = -1) :
      mConnectFlag(connectFlag), mType(type), mWeight(weight), mDelay(delay) { }

   bool        mConnectFlag;
   ChannelType mType;
   float       mWeight;
   int         mDelay;
};

struct Point2D
{
   Point2D(unsigned int width, unsigned int height) : mWidth(width), mHeight(height) { }
   Point2D() : mWidth(0), mHeight(0) { }
   unsigned int mWidth;
   unsigned int mHeight;
};

#endif