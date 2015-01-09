#ifndef VISUALNETWORK_H
#define VISUALNETWORK_H

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

#include "Network.h"

#define deg2rad(d) (d*3.14159265)/180.0

enum LayerType
{
   INPUT_LAYER = 0,
   LEARNING_MID_LAYER,
   NONLEARNING_MID_LAYER
};

enum ConnectionPatternMode
{
   RECEPTIVE_FIELD_CONNECTION = 0,
   LATERAL_CONNECTION,
   CUSTOM_CONNECTION
};

enum FinishCriterion
{
   EPOCH_NUMBER = 0,
   TIME_MILISECOND,
   TIME_SECOND,
   TIME_MINUTE,
   TIME_HOUR
};

struct Point2D
{
   Point2D(unsigned int width, unsigned int height) : mWidth(width), mHeight(height) { }
   Point2D() : mWidth(0), mHeight(0) { }
   unsigned int mWidth;
   unsigned int mHeight;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

struct PixelInputInformation
{
   PixelInputInformation(Point2D pixel, int time) : mPixel(pixel), mTime(time) {}
   Point2D mPixel;
   int     mTime;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

namespace boost{ namespace serialization { class access; } namespace archive { class text_oarchive; } }

class MODULE_EXPORT PFCLayer : public Layer
{
   friend class DAHandler;
   friend class boost::serialization::access;
public:
   PFCLayer(Network* net, int ID, bool shouldLearn = true, bool isContainer = false) 
      : Layer(net, ID, shouldLearn, isContainer) { initialize(); }
   PFCLayer() : Layer() { initialize(); } //used only by boost::serialization
   template <class NeuronTemp>
   void arrangeNeurons(int inNum, int groupsNum, int additionalNum, float inratio, int neuronsToConnect,
               ParameterContainer* inparams = 0, ParameterContainer* exparams = 0);
   virtual void recordSpike(int NeuronID);
   void getInputFrom(Layer* slayer);

private:
   //std::vector<int> mGroupsLastIndex; TODO: make it general
   int mG1LastIndex;
   int mG2LastIndex;
   int mInLastIndex;

   //void makeGroupIslands(int neuronNum);
   void initialize();
   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

class MODULE_EXPORT VisualNetwork : public Network
{
   friend class boost::serialization::access;
   friend class DAHandler;
public:
   VisualNetwork();

   template <class NeuronTemp>
   int addLayer(Point2D size, LayerType layerType = LEARNING_MID_LAYER, 
      ChannelType neuronType = EXCITATORY, ParameterContainer* neuronParams = 0);

   template <class NeuronTemp>
   int addLateralLayer(int sourceLayer, LayerType layerType = LEARNING_MID_LAYER,
      ChannelType neuronType = EXCITATORY, float weight = -1, int delay = -1, ParameterContainer* neuronParams = 0);

   template <class NeuronTemp>
   int addSimpleCellSuperLayer(int sourceLayer, int layersNumToAdd, Point2D receptiveFieldSize,
      Point2D fieldStep, bool shareConnectionsFlag = false, ParameterContainer* neuronParams = 0);

   template <class NeuronTemp>
   int addSimpleCellFromSuperLayer(int sourceSuperLayer, int layersNumToAdd, Point2D receptiveFieldSize,
         Point2D fieldStep, bool shareConnectionsFlag = false, ParameterContainer* neuronParams = 0);

   template <class NeuronTemp>
   int addComplexCellFromSuperLayer(int sourceSuperLayer, Point2D receptiveFieldSize,
      Point2D fieldStep, bool shareConnectionsFlag = false, ParameterContainer* neuronParams = 0);

   //template <class NeuronTemp>
   int addPFCLayer(int inNum=50, int groupsNum=50, int additionalNum=100, float inratio=0.2, int neuronsToConnect=25,
        ParameterContainer* inparams = 0, ParameterContainer* exparams = 0);

   void setOrientationalWeights(int superLayer);

   void makeConnection(int sourceLayer, int destLayer, ConnectionInfo (*pattern)(Point2D, Point2D));

   void setInputImagesDirectory(std::string dirName, int presentTimeStep = 40);

   //TODO: void logSettings(); 
   //void logPostSynapseWeights(int layer, Point2D neuron, std::string directory = "")
   //{ Network::logPostSynapseWeights(layer, to1D(layer, neuron), directory); }
   //void logPreSynapseWeights(int layer, Point2D neuron, std::string directory = "") 
   //{ Network::logPreSynapseWeights(layer, to1D(layer, neuron), directory); }
   void logSuperLayerActivity(int superLayerID)
   { for(size_t i = 0; i < mSuperLayersContent[superLayerID].size(); ++i) logLayerActivity(mSuperLayersContent[superLayerID][i]); }
   //TODO: void logSynapseWeight(bool (*pattern)(int, int, int, int));
   //TODO: void logSynapseWeight(bool (*pattern)(int));

   std::vector<float> getResponseFromLayer(int sourceLayer, int destLayer, Point2D destNeuron)
   { return Network::getResponseFromLayer(sourceLayer, destLayer, to1D(destLayer, destNeuron)); }
   std::vector<float> getResponseFromSuperLayer(int sourceSuperLayer, int destLayer, Point2D destNeuron);

   void shareConnection(int layer, Point2D sourceNeuron=Point2D(0,0), int sharingTimeStep=40) 
   { mLayers[layer]->shareConnection(to1D(layer, sourceNeuron), sharingTimeStep); }

   void runNetwork(FinishCriterion crit, int critNum);
   static void saveNetwork(VisualNetwork& visNet, std::string path);
   static VisualNetwork* loadNetwork(std::string path);

   virtual ConnectionInfo defaultConnectingPattern(int sourceIndex, int destIndex);
   virtual std::vector<InputInformation> defaultInputPattern(int time);
   virtual std::string getAddress(int slayer, int sneuron = -1, int dlayer = -1, int dneuron = -1);

private:
   std::vector<Point2D>                mLayersSizeInfo;
   int                                 mInputLayerIndex;
   std::string                         mImagesFolderName;
   std::vector<std::string>            mImageFileNames;
   size_t                              mCurrentImageIndex;
   std::vector<PixelInputInformation>  mInputInfos;
   std::vector<std::vector<int> >      mSuperLayersContent;
   int                                 mPresentTimeStep;
   PFCLayer*                           mPFCLayer;

   //parameters for making connection which is used in defaultConnectingPattern
   Point2D mReceptiveFieldSize;
   Point2D mFieldStep;
   int     mSourceLayer;
   int     mDestLayer;
   float   mLateralWeight;
   int     mLateralDelay;

   ConnectionPatternMode mConnectionPatternMode;
   ConnectionInfo (*mCustomConnectionPattern) (Point2D, Point2D);

   Point2D to2D(int layerIndex, int neuronIndex)
   { return Point2D(neuronIndex % mLayersSizeInfo[layerIndex].mWidth, neuronIndex / mLayersSizeInfo[layerIndex].mWidth); }

   int to1D(int layerIndex, Point2D neuronIndex)
   { return (neuronIndex.mHeight * mLayersSizeInfo[layerIndex].mWidth) + neuronIndex.mWidth; }

   std::vector<float> makeOrientationWeights(int matSize, float maxW, int orientationDegree);

   template <class NeuronTemp>
   int addReceptiveFieldLayer(int sourceLayer, Point2D receptiveFieldSize, Point2D fieldStep, LayerType type,
      bool shareConnections = false, ParameterContainer* neuronParams = 0);

   void initialize();
   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
};

template <class NeuronTemp>
void PFCLayer::arrangeNeurons(int inNum, int groupsNum, int additionalNum, float inratio, int neuronsToConnect,
                   ParameterContainer* inparams, ParameterContainer* exparams)
{
   int total = inNum+2*groupsNum+additionalNum;

   for (size_t i=0; i < total; ++i)
   {
      float ran = ((float)rand() / RAND_MAX);
      addNeuron<NeuronTemp>(1, (ran <= inratio) ? INHIBITORY : EXCITATORY,
                               (ran <= inratio) ? inparams : exparams);
   }

   mInLastIndex = inNum-1;
   mG1LastIndex = mInLastIndex + groupsNum;
   mG2LastIndex = mG1LastIndex + groupsNum;

   Layer::makeConnection(this, this, neuronsToConnect, 6, 5, -1, 1);
}

template <class NeuronTemp>
int VisualNetwork::addLayer(Point2D size, LayerType layerType, 
   ChannelType neuronType, ParameterContainer* neuronParams)
{
   bool shouldLearn = true, isContainer = false;

   if (layerType == NONLEARNING_MID_LAYER)
      shouldLearn = false;

   if (layerType == INPUT_LAYER)
      isContainer = true;

   int ind = Network::addLayer(shouldLearn, isContainer);

   if (layerType == INPUT_LAYER)
      mInputLayerIndex = ind;

   mLayersSizeInfo.push_back(size);
   Network::addNeuron<NeuronTemp>(ind, size.mWidth*size.mHeight, neuronType, neuronParams);
   return ind;
}

template <class NeuronTemp>
int VisualNetwork::addReceptiveFieldLayer(int sourceLayer, Point2D receptiveFieldSize,
                                          Point2D fieldStep, LayerType type, bool shareConnectionsFlag,
                                          ParameterContainer* neuronParams)
{
   mReceptiveFieldSize = receptiveFieldSize;
   mFieldStep = fieldStep;
   mSourceLayer = sourceLayer;
   mConnectionPatternMode = RECEPTIVE_FIELD_CONNECTION;
   Point2D size ((mLayersSizeInfo[sourceLayer].mWidth - mReceptiveFieldSize.mWidth)/fieldStep.mWidth + 1,
                 (mLayersSizeInfo[sourceLayer].mHeight - mReceptiveFieldSize.mHeight)/fieldStep.mHeight + 1);
   mDestLayer = addLayer<NeuronTemp>(size, type, EXCITATORY);
   Network::makeConnection(mSourceLayer, mDestLayer);

   if (shareConnectionsFlag)
      mLayers[mDestLayer]->shareConnection(0, mPresentTimeStep);

   return mDestLayer;
}

template <class NeuronTemp>
int VisualNetwork::addLateralLayer(int sourceLayer, LayerType layerType, ChannelType neuronType,
                                   float connectionWeight, int connectionDelay, ParameterContainer* neuronParams)
{
   mSourceLayer = sourceLayer;
   mLateralWeight = connectionWeight;
   mLateralDelay = connectionDelay;
   mDestLayer = addLayer<NeuronTemp>(mLayersSizeInfo[sourceLayer], layerType, neuronType, neuronParams);
   mConnectionPatternMode = LATERAL_CONNECTION;
   Network::makeConnection(mSourceLayer, mDestLayer);
   return mDestLayer;
}

template <class NeuronTemp>
int VisualNetwork::addSimpleCellSuperLayer(int sourceLayer, int layersNumToAdd, Point2D receptiveFieldSize,
      Point2D fieldStep, bool shareConnectionsFlag, ParameterContainer* neuronParams)
{
   mSuperLayersContent.push_back(std::vector<int>());
   size_t sindex = mSuperLayersContent.size()-1;

   for (size_t i = 0; i < layersNumToAdd; ++i)
      mSuperLayersContent[sindex].push_back(addReceptiveFieldLayer<NeuronTemp>(
         sourceLayer, receptiveFieldSize, fieldStep, LEARNING_MID_LAYER, false, neuronParams));

   mConnectionPatternMode = LATERAL_CONNECTION;
   for (size_t i = 0; i < layersNumToAdd; ++i)
   {
      for (size_t j = 0; j < layersNumToAdd; ++j)
      {
         if (i != j)
         {
            mSourceLayer = mSuperLayersContent[sindex][i];
            mDestLayer = mSuperLayersContent[sindex][j];
            mLateralWeight = CHOOSE_RANDOM;
            mLateralDelay = 0;
            Network::makeConnection(mSourceLayer, mDestLayer);
         }
      }
   }

   if (shareConnectionsFlag)
      for (size_t i = 0; i < mSuperLayersContent[sindex].size(); ++i)
         mLayers[mSuperLayersContent[sindex][i]]->shareConnection(0, mPresentTimeStep);

   return sindex;
}

template <class NeuronTemp>
int VisualNetwork::addSimpleCellFromSuperLayer(int sourceSuperLayer, int layersNumToAdd, Point2D receptiveFieldSize,
      Point2D fieldStep, bool shareConnectionsFlag, ParameterContainer* neuronParams)
{
   int sindex = addSimpleCellSuperLayer<NeuronTemp>(mSuperLayersContent[sourceSuperLayer][0], layersNumToAdd,
      receptiveFieldSize, fieldStep, false, neuronParams);

   mConnectionPatternMode = RECEPTIVE_FIELD_CONNECTION;
   mReceptiveFieldSize = receptiveFieldSize;
   mFieldStep = fieldStep;
   for (size_t i = 1; i < mSuperLayersContent[sourceSuperLayer].size(); ++i)
   {
      for (size_t j = 0; j < layersNumToAdd; ++j)
      {
         mSourceLayer = mSuperLayersContent[sourceSuperLayer][i];
         mDestLayer = mSuperLayersContent[sindex][j];
         Network::makeConnection(mSourceLayer, mDestLayer);
      }
   }

   if (shareConnectionsFlag)
      for (size_t i = 0; i < mSuperLayersContent[sindex].size(); ++i)
         mLayers[mSuperLayersContent[sindex][i]]->shareConnection(0, mPresentTimeStep);


   return sindex;
}

template <class NeuronTemp>
int VisualNetwork::addComplexCellFromSuperLayer(int sourceSuperLayer, Point2D receptiveFieldSize,
      Point2D fieldStep, bool shareConnectionsFlag, ParameterContainer* neuronParams)
{
   mSuperLayersContent.push_back(std::vector<int>());
   size_t sindex = mSuperLayersContent.size()-1;

   for (size_t i = 0; i < mSuperLayersContent[sourceSuperLayer].size(); ++i)
      mSuperLayersContent[sindex].push_back(addReceptiveFieldLayer<NeuronTemp>(
         mSuperLayersContent[sourceSuperLayer][i], receptiveFieldSize, fieldStep, NONLEARNING_MID_LAYER, false, neuronParams));

   if (shareConnectionsFlag)
      for (size_t i = 0; i < mSuperLayersContent[sindex].size(); ++i)
         mLayers[mSuperLayersContent[sindex][i]]->shareConnection(0, mPresentTimeStep);

   return sindex;
}



#endif