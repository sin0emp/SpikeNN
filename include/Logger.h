#ifndef LOGGER_H
#define LOGGER_H

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

#include <fstream>
#include <string>

class MODULE_EXPORT Logger
{
public:
   //Logger(const Logger& rhs)
   //{
   //   mFileName = rhs.mFileName;
   //   mDirectory = rhs.mDirectory;
   //}

   Logger(){}
   Logger(std::string filename, std::string directory = "") {set(filename, directory);}
   ~Logger() {mStream.close();}

   void set(std::string filename, std::string directory = "");
   void writeLine(std::string line)
   {
      if (!mStream.is_open()) mStream.open((mDirectory + mFileName).c_str());
      mStream << line << std::endl; mStream.flush();
   }

   void write(std::string str)
   {
      if (!mStream.is_open()) mStream.open((mDirectory + mFileName).c_str());
      mStream << str; mStream.flush();
   }
   
   static std::string toString(float input);

   //Logger& operator=(Logger& rhs)
   //{
   //   mFileName = rhs.mFileName;
   //   mDirectory = rhs.mDirectory;
   //   return *this;
   //}

private:
   //TODO: general stream should be replaced by file stream
   std::string   mFileName;
   std::string   mDirectory;
   std::ofstream mStream;

   //template <class Archive>
   //void serialize(Archive &ar, const unsigned int version);
};

#endif