#include "VisualNetwork.h"
#include "EasyBMP.h"
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <iostream>

BOOST_CLASS_EXPORT(VisualNetwork);

VisualNetwork::VisualNetwork()
{
   mCustomConnectionPattern = 0;
   mCurrentImageIndex = -1;
}

ConnectionInfo VisualNetwork::defaultConnectingPattern(int sourceIndex, int destIndex)
{
   Point2D source = to2D(mSourceLayer, sourceIndex);
   Point2D dest = to2D(mDestLayer, destIndex);
   
   switch (mConnectionPatternMode)
   {
   case RECEPTIVE_FIELD_CONNECTION:
      if (source.mWidth >= dest.mWidth * mFieldStep.mWidth && 
         source.mWidth < dest.mWidth * mFieldStep.mWidth + mReceptiveFieldSize.mWidth &&
         source.mHeight >= dest.mHeight * mFieldStep.mHeight && 
         source.mHeight < dest.mHeight * mFieldStep.mHeight + mReceptiveFieldSize.mHeight)
         return ConnectionInfo(true, EXCITATORY);
      else
         return ConnectionInfo(false);

      break;

   case LATERAL_CONNECTION:
      if (source.mWidth == dest.mWidth && source.mHeight == dest.mHeight)
         return ConnectionInfo(true, INHIBITORY, mLateralWeight, mLateralDelay);
      else
         return ConnectionInfo(false);

      break;

   case CUSTOM_CONNECTION:
      return (*mCustomConnectionPattern)(source, dest);
      break;

   default:
      return ConnectionInfo(false);
      break;
   }
}

std::vector<InputInformation> VisualNetwork::defaultInputPattern(int time)
{
   if (time % 100 == 1)
   {
      for (size_t i = 0; i < mLayers.size(); ++i)
         mLayers[i]->restNeurons();

      //mCurrentImageIndex = (int)(((float)rand() / RAND_MAX) * mImageFileNames.size());
      ++mCurrentImageIndex;
      if (mCurrentImageIndex == mImageFileNames.size()) mCurrentImageIndex = 0;

      mLogger.writeLine(Logger::toString((float)time) + " " + mImageFileNames[mCurrentImageIndex]);
      //++mCurrentImageIndex;
      //if (mCurrentImageIndex >= mImageFileNames.size()) mCurrentImageIndex = 0;
      mInputInfos.clear();
      BMP image;
      image.ReadFromFile((mImagesFolderName + mImageFileNames[mCurrentImageIndex]).c_str());

      for (int i = 0; i < image.TellWidth(); ++i)
         for (int j = 0; j < image.TellHeight(); ++j)
         {
            RGBApixel pix = image.GetPixel(i, j);
            //TODO: should we pass the image through grayscale and DOG filter here??
            float gray = (float)(0.21*pix.Red + 0.72*pix.Green + 0.07*pix.Blue);
            if (gray < 220) //a threshold for color strength which leads to a spikes
            {
               int timeToAdd = time + (int)(10*(gray/256));
               while (timeToAdd < time + 100)
               {
                  PixelInputInformation info(Point2D(i,j), timeToAdd);
                  mInputInfos.push_back(info);

                  for (size_t k = mInputInfos.size()-1; k > 0; --k)
                  {
                     if (mInputInfos[k].mTime < mInputInfos[k-1].mTime)
                     {
                        PixelInputInformation hold = mInputInfos[k];
                        mInputInfos[k] = mInputInfos[k-1];
                        mInputInfos[k-1]=hold;
                     }
                     else
                        break;
                  }

                  timeToAdd += 200;
               }
            }
         }
   }

   std::vector<InputInformation> re;
   while (mInputInfos.size() > 0)
   {
      if(mInputInfos[0].mTime <= time)
      {
         re.push_back(InputInformation(to1D(mInputLayerIndex, mInputInfos[0].mPixel), FORCE_FIRE));
         mInputInfos.erase(mInputInfos.begin());
      }
      else
         break;
   }

   return re;
}

void VisualNetwork::makeConnection(int sourceLayer, int destLayer, ConnectionInfo (*pattern)(Point2D, Point2D))
{
   mCustomConnectionPattern = pattern;
   mConnectionPatternMode = CUSTOM_CONNECTION;
   Network::makeConnection(sourceLayer, destLayer);
}

void VisualNetwork::setInputImagesDirectory(std::string dirName)
{
   boost::filesystem::path dirPath(dirName);
   boost::filesystem::directory_iterator end_iter;

   if (boost::filesystem::exists(dirPath) && boost::filesystem::is_directory(dirPath))
   {
      std::vector<std::string> filenames;
      for(boost::filesystem::directory_iterator dir_iter(dirPath); dir_iter != end_iter; ++dir_iter)
      {
         if (dir_iter->path().extension().string() == ".bmp")
            filenames.push_back(dir_iter->path().filename().string());
      }
      
      std::vector<std::size_t> shuffled = SHUFFLE(filenames.size());
      
      mImageFileNames.clear();
      for (std::size_t i = 0; i < filenames.size(); ++i)
         mImageFileNames.push_back(filenames[shuffled[i]]);

      setInputPattern(mInputLayerIndex, MANUAL_INPUT);
      mImagesFolderName = dirName;
      if (mImagesFolderName[mImagesFolderName.size()-1] != '/')
         mImagesFolderName += '/';
   }
   else
   {
      std::cout<<"could not find directory!";
     //TODO: throw error: could not open directory
   }
}

std::string VisualNetwork::getAddress(int slayer, int sneuron, int dlayer, int dneuron)
{
   std::string s;
   if (slayer != -1)
      s += "l" + Logger::toString((float)slayer);
   if (sneuron != -1)
   {
      Point2D n = to2D(slayer, sneuron);
      s += "n" + Logger::toString((float)n.mHeight) + "," + Logger::toString((float)n.mWidth);
   }
   if (dlayer != -1 && dlayer != -1)
   {
      Point2D n = to2D(dlayer, dneuron);
      s += "-TO-l" + Logger::toString((float)dlayer) + "n" + Logger::toString((float)n.mHeight) + "," + Logger::toString((float)n.mWidth);
   }

   return s;
}

std::vector<float> VisualNetwork::getResponseFromSuperLayer(int sourceSuperLayer, int destLayer, Point2D destNeuron)
{
   std::vector<float> re;
   for (size_t i = 0; i < mSuperLayersContent[sourceSuperLayer].size(); ++i)
   {
      if (re.size() == 0)
         re = getResponseFromLayer(mSuperLayersContent[sourceSuperLayer][i], destLayer, destNeuron);
      else
      {
         std::vector<float> res;
         res = getResponseFromLayer(mSuperLayersContent[sourceSuperLayer][i], destLayer, destNeuron);
         for (size_t j = 0; j < res.size(); ++j)
            if (res[j] > re[j]) re[j] = res[j];
      }
   }

   return re;
}

template <class Archive>
void VisualNetwork::serialize(Archive &ar, const unsigned int version)
{
   ar & boost::serialization::base_object<Network>(*this);
   ar & mLayersSizeInfo & mInputLayerIndex
      & mImagesFolderName & mImageFileNames
      & mSuperLayersContent & mConnectionPatternMode;
}

template void VisualNetwork::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void VisualNetwork::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);

void VisualNetwork::saveNetwork(VisualNetwork& visNet, std::string path)
{
    std::ofstream file(path.c_str(), std::ios::binary);
    boost::archive::text_oarchive oa(file);
    oa.register_type<VisualNetwork>();
    oa & visNet;
    file.close();
}

VisualNetwork* VisualNetwork::loadNetwork(std::string path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    boost::archive::text_iarchive ia(file);
    ia.register_type<VisualNetwork>();
    VisualNetwork* net = new VisualNetwork();
    ia & *net;

    for (size_t i = 0; i <= net->mLastLayerID; ++i)
       net->mLayers[i]->wakeup();

    file.close();
    return net;
}

void VisualNetwork::runNetwork(FinishCriterion crit, int critNum)
{
   switch (crit)
   {
   case EPOCH_NUMBER:
       Network::runNetwork(critNum*mImageFileNames.size()*100);
       break;
   case TIME_MILISECOND:
       Network::runNetwork(critNum);
       break;
   case TIME_SECOND:
       Network::runNetwork(critNum*1000);
       break;
   case TIME_MINUTE:
       Network::runNetwork(critNum*1000*60);
       break;
   case TIME_HOUR:
       Network::runNetwork(critNum*1000*60*60);
       break;
   default:
      break;
   }
}

template <class Archive>
void Point2D::serialize(Archive &ar, const unsigned int version)
{
   ar & mWidth & mHeight;
}

template void Point2D::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void Point2D::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);

template <class Archive>
void PixelInputInformation::serialize(Archive &ar, const unsigned int version)
{
   ar & mPixel & mTime;
}

template void PixelInputInformation::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void PixelInputInformation::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);
