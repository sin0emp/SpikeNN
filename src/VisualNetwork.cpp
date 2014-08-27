#include "VisualNetwork.h"
#include "EasyBMP.h"
#include <boost/filesystem.hpp>

#include <iostream>

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

   case SAME_INDEX_INHIBIT_CONNECTION:
      if (source.mWidth == dest.mWidth && source.mHeight == dest.mHeight)
         return ConnectionInfo(true, INHIBITORY, 20, 0); //TODO: is 20 enough??
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
   if (time % 50 == 1)
   {
      for (size_t i = 0; i < mLayers.size(); ++i)
         mLayers[i]->restNeurons();

      ++mCurrentImageIndex;
      if (mCurrentImageIndex >= mImageFileNames.size()) mCurrentImageIndex = 0;
      mInputInfos.clear();
      BMP image;
      image.ReadFromFile((mImagesFolderName + mImageFileNames[mCurrentImageIndex]).c_str());

      for (int i = 0; i < image.TellWidth(); ++i)
         for (int j = 0; j < image.TellHeight(); ++j)
         {
            RGBApixel pix = image.GetPixel(i, j);
            //TODO: should we pass the image through grayscale and DOG filter here??
            if (pix.Red < 50) //a threshold for color strength which leads to an spikes
            {
               PixelInputInformation info(Point2D(i,j), time + (int)(20*((float)pix.Red/256)));
               mInputInfos.push_back(info); //distribute spikes with respect to their color strength over 100 milisecond
               //insertion sort!
               for (size_t i = mInputInfos.size()-1; i > 0; --i)
               {
                  if (mInputInfos[i].mTime < mInputInfos[i-1].mTime)
                  {
                     int hold = mInputInfos[i].mTime;
                     mInputInfos[i].mTime = mInputInfos[i-1].mTime;
                     mInputInfos[i-1].mTime=hold;
                  }
                  else
                     break;
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
      for(boost::filesystem::directory_iterator dir_iter(dirPath); dir_iter != end_iter; ++dir_iter)
      {
         if (dir_iter->path().extension().string() == ".bmp")
            mImageFileNames.push_back(dir_iter->path().filename().string());
      }

      setInputPattern(mInputLayerIndex, MANUAL_INPUT);
      mImagesFolderName = dirName;
      if (mImagesFolderName[mImagesFolderName.size()-1] != '/')
         mImagesFolderName += '/';
   }
   else
   {
     //TODO: throw error: could not open directory
   }
}

std::string VisualNetwork::getAddress(int slayer, int sneuron, int dlayer, int dneuron)
{
   std::string s;
   if (slayer != -1)
      s += "l" + Logger::toString(slayer);
   if (sneuron != -1)
   {
      Point2D n = to2D(slayer, sneuron);
      s += "n" + Logger::toString(n.mWidth) + "," + Logger::toString(n.mHeight);
   }
   if (dlayer != -1 && dlayer != -1)
   {
      Point2D n = to2D(dlayer, dneuron);
      s += "-TO-l" + Logger::toString(dlayer) + "n" + Logger::toString(n.mWidth) + "," + Logger::toString(n.mHeight);
   }

   return s;
}