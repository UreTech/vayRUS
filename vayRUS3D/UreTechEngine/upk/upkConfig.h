#pragma once

#define upkPartitionMaxFileCount 48 //maximum files in one partition
#define upkMaxPartitionCount 256     //maximum partition count


//#define debugFileInputOutput //debugs input output
//#define debugFileSearchOutput //debugs search output
//#define debugFileObjectCreateOutput //debugs object creation output
//#define debugFileReaderOutput //debugs file reader output
//#define debugFileWriterOutput //debugs file writer output
//#define debugZeroByteFileOutput //debugs zero byte files output


#define upkVersion "upk.version:    1.7 Stable"
#define upkEncryptorVersion "upk.encryption: basicEncryptor: v1.1"
#define upkEncryptorModel "upk.encryption.model: encryptor model 1.0X/N"
#define upkBuild "upk.build:      b.5445"

#define upk_PackagerHeaderTitle "upk.package:"
#define upk_EncryptorHeaderTitle "upk.encrypt:"

#define upk_PackagerFileSysHeaderTitle "upk.package.fileSys:"
#define upk_EncryptorFileSysHeaderTitle "upk.encrypt.fileSys:"

#define upk_PackagerDebugFileSysHeaderTitle "upk.package.debug.fileSys:"
#define upk_EncryptorDebugFileSysHeaderTitle "upk.encrypt.debug.fileSys:"

#define upk_PackagerDebugHeaderTitle "upk.package.debug:"
#define upk_EncryptorDebugHeaderTitle "upk.encrypt.debug:"

#define upk_error "(!)ERROR:"
#define upk_error_warn "(i)ERROR:"

#define upk_warn "(i)WARN:"
#define upk_warn_error "(!)WARN:"

#define upk_info "(i)Info:"