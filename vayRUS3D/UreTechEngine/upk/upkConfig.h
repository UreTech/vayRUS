#pragma once

#define upkPartitionMaxFileCount 48 //maximum files in one partition
#define upkMaxPartitionCount 256     //maximum partition count


//#define debugFileInputOutput //debugs input output
//#define debugFileSearchOutput //debugs search output
//#define debugFileObjectCreateOutput //debugs object creation output
//#define debugFileReaderOutput //debugs file reader output
//#define debugFileWriterOutput //debugs file writer output
//#define debugZeroByteFileOutput //debugs zero byte files output


#define upkVersion std::string("upk.version:    1.7 Stable")
#define upkEncryptorVersion std::string("upk.encryption: basicEncryptor: v1.1")
#define upkEncryptorModel std::string("upk.encryption.model: encryptor model 1.0X/N")
#define upkBuild std::string("upk.build:      b.5445")

#define upk_PackagerHeaderTitle std::string("upk.package:")
#define upk_EncryptorHeaderTitle std::string("upk.encrypt:")

#define upk_PackagerFileSysHeaderTitle std::string("upk.package.fileSys:")
#define upk_EncryptorFileSysHeaderTitle std::string("upk.encrypt.fileSys:")

#define upk_PackagerDebugFileSysHeaderTitle std::string("upk.package.debug.fileSys:")
#define upk_EncryptorDebugFileSysHeaderTitle std::string("upk.encrypt.debug.fileSys:")

#define upk_PackagerDebugHeaderTitle std::string("upk.package.debug:")
#define upk_EncryptorDebugHeaderTitle std::string("upk.encrypt.debug:")

#define upk_error std::string("(!)ERROR:")
#define upk_error_warn std::string("(i)ERROR:")

#define upk_warn std::string("(i)WARN:")
#define upk_warn_error std::string("(!)WARN:")

#define upk_info std::string("(i)Info:")