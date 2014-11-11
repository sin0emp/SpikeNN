#include "Logger.h"
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>

void Logger::set(std::string filename, std::string directory)
{
   if (filename.find(".log") == std::string::npos)
      filename += ".log";

   if (directory != "")
      if (directory[directory.size()-1] != '/')
            directory += '/';

   mFileName = filename;
   mDirectory = directory;

   if(mStream.is_open()) mStream.close();
}

std::string Logger::toString(float input)
{
   std::stringstream ss;
   ss << input;
   return ss.str();
}

//template <class Archive>
//void Logger::serialize(Archive &ar, const unsigned int version)
//{
//   ar & mFileName) & mDirectory);
//}
//
//template void Logger::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
//template void Logger::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);
