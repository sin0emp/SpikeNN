#include "Logger.h"
#include <sstream>

void Logger::set(std::string filename)
{
   mFileName = filename;

   if (filename.find(".log") == std::string::npos)
      mFileName += ".log";

   if(mStream.is_open()) mStream.close();

   //mStream.open(mFileName.c_str());
}

std::string Logger::toString(float input)
{
   std::stringstream ss;
   ss << input;
   return ss.str();
}