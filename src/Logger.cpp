#include "Logger.h"
#include <sstream>

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