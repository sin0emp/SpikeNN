#ifndef VISUALNETWORK_H
#define VISUALNETWORK_H

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

#include "Network.h"

enum LayerType
{
   INPUT_LAYER = 0,
   LEARNING_MID_LAYER,
   NONLEARNING_MID_LAYER
};

enum ConnectionPatternMode
{
   RECEPTIVE_FIELD_CONNECTION = 0,
   SAME_INDEX_INHIBIT_CONNECTION,
   CUSTOM_CONNECTION
};

struct PixelInputInformation
{
   PixelInputInformation(Point2D pixel, int time) : mPixel(pixel), mTime(time) {}
   Point2D mPixel;
   int     mTime;
};

class MODULE_EXPORT VisualNetwork : public Network
{
public:
   VisualNetwork();

   template <class NeuronTemp>
   int addLayer(Point2D size, LayerType layerType = LEARNING_MID_LAYER, 
      ChannelType neuronType = EXCITATORY, ParameterContainer* neuronParams = 0);

   template <class NeuronTemp>
   int addReceptiveFieldLayer(int sourceLayer, Point2D receptiveFieldSize, Point2D fieldStep);

   template <class NeuronTemp>
   void addReceptiveFieldSuperLayer(int sourceLayer, int layersNumToAdd, Point2D receptiveFieldSize, Point2D fieldStep);

   void makeConnection(int sourceLayer, int destLayer, ConnectionInfo (*pattern)(Point2D, Point2D));

   void setInputImagesDirectory(std::string dirName);

   //TODO: void logSettings(); 
   void logPostSynapseWeights(int layer, Point2D neuron, std::string directory = "")
   { Network::logPostSynapseWeights(layer, to1D(layer, neuron), directory); }
   void logPreSynapseWeights(int layer, Point2D neuron, std::string directory = "") 
   { Network::logPreSynapseWeights(layer, to1D(layer, neuron), directory); }
   //TODO: void logSynapseWeight(bool (*pattern)(int, int, int, int));
   //TODO: void logSynapseWeight(bool (*pattern)(int));

   virtual ConnectionInfo defaultConnectingPattern(int sourceIndex, int destIndex);
   virtual std::vector<InputInformation> defaultInputPattern(int time);
   virtual std::string getAddress(int slayer, int sneuron = -1, int dlayer = -1, int dneuron = -1);

private:
   std::vector<Point2D>                mLayersSizeInfo;
   int                                 mInputLayerIndex;
   std::vector<std::vector<int> >      mSuperLayersContent;
   std::string                         mImagesFolderName;
   std::vector<std::string>            mImageFileNames;
   size_t                              mCurrentImageIndex;
   std::vector<PixelInputInformation>  mInputInfos;

   Point2D mReceptiveFieldSize;
   Point2D mFieldStep;
   int mSourceLayer;
   int mDestLayer;
   ConnectionPatternMode mConnectionPatternMode;
   ConnectionInfo (*mCustomConnectionPattern) (Point2D, Point2D);

   Point2D to2D(int layerIndex, int neuronIndex)
   { return Point2D(neuronIndex % mLayersSizeInfo[layerIndex].mWidth, neuronIndex / mLayersSizeInfo[layerIndex].mWidth); }

   int to1D(int layerIndex, Point2D neuronIndex)
   { return (neuronIndex.mHeight * mLayersSizeInfo[layerIndex].mWidth) + neuronIndex.mWidth; }
};

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
int VisualNetwork::addReceptiveFieldLayer(int sourceLayer, Point2D receptiveFieldSize, Point2D fieldStep)
{
   mReceptiveFieldSize = receptiveFieldSize;
   mFieldStep = fieldStep;
   mSourceLayer = sourceLayer;
   mConnectionPatternMode = RECEPTIVE_FIELD_CONNECTION;
   Point2D size ((mLayersSizeInfo[sourceLayer].mWidth - mReceptiveFieldSize.mWidth)/fieldStep.mWidth + 1,
                 (mLayersSizeInfo[sourceLayer].mHeight - mReceptiveFieldSize.mHeight)/fieldStep.mHeight + 1);
   mDestLayer = addLayer<NeuronTemp>(size, LEARNING_MID_LAYER, EXCITATORY);
   Network::makeConnection(mSourceLayer, mDestLayer);

   return mDestLayer;
}

template <class NeuronTemp>
void VisualNetwork::addReceptiveFieldSuperLayer(int sourceLayer, int layersNumToAdd, Point2D receptiveFieldSize, Point2D fieldStep)
{
   mSuperLayersContent.push_back(std::vector<int>());
   size_t slayerInd = mSuperLayersContent.size() - 1;

   for (int i = 0; i < layersNumToAdd; ++i)
   {
      mSuperLayersContent[slayerInd].push_back(
         addReceptiveFieldLayer<IzhikevichNeuron>(sourceLayer, receptiveFieldSize, fieldStep));
   }

   mConnectionPatternMode = SAME_INDEX_INHIBIT_CONNECTION;
   for (size_t i = 0; i < mSuperLayersContent[slayerInd].size(); ++i)
      for (size_t j = 0; j < mSuperLayersContent[slayerInd].size(); ++j)
      {
         if (i != j)
         {
            mSourceLayer = mSuperLayersContent[slayerInd][i];
            mDestLayer = mSuperLayersContent[slayerInd][j];
            Network::makeConnection(mSourceLayer, mDestLayer);
         }
      }
}


#endif