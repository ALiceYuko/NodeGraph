#ifndef NODEEDIT_UTILS_DEFINE
#define NODEEDIT_UTILS_DEFINE

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#ifdef NE_USE_THIRD_LIB_SELF
#include "Container.h"
#else
#include "utils/Container.h"
#endif


#include <stdint.h>
#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>

#if defined(ANDROID)||defined(__ANDROID__)
#include <android/log.h>
#include <sys/system_properties.h>
#endif

#ifdef __ANDROID__
#define NELOGI(...) __android_log_print(ANDROID_LOG_INFO, __FILE__, __VA_ARGS__)
#define NELOGW(...) __android_log_print(ANDROID_LOG_WARN, __FILE__, __VA_ARGS__)
#define NELOGE(...) __android_log_print(ANDROID_LOG_ERROR, __FILE__, __VA_ARGS__)
#else
#define NELOGI(...) {printf("NE INFO[%s]", __FILE__);printf(__VA_ARGS__); printf("\n");}
#define NELOGW(...) {printf("NE WARN[%s]", __FILE__);printf(__VA_ARGS__); printf("\n");}
#define NELOGE(...) {printf("NE ERRO[%s]", __FILE__);printf(__VA_ARGS__); printf("\n");}
#endif


#define NE_CHECK_NOVALID_POINT_RETURN_VALUE(P,RET) \
if(nullptr == (P)) return (RET); \


#define NE_CHECK_NOVALID_POINT_RETURN(P) \
if(nullptr == (P)) return; \



#define NE_CHECK_FALSE_RETURN_VALUE(P,RET) \
if(!(P)) return (RET); \


#define NE_CHECK_FALSE_RETURN(P) \
if(!(P)) return; \


#define NE_CHECK_NOVALID_POINT_BREAK(P) \
if(nullptr == (P)) break; \

namespace node_edit
{
	namespace UID
	{
		uint32_t Generate();
	};

	rapidjson::Value empty_value();
	rapidjson::Value to_value(uint32_t v, rapidjson::Document& d);
	rapidjson::Value to_value(std::string v, rapidjson::Document& d);
	rapidjson::Value to_value(std::vector<std::string> v, rapidjson::Document& d);
	rapidjson::Value to_value(std::vector<int> v, rapidjson::Document& d);
	rapidjson::Value to_value(std::vector<float> v, rapidjson::Document& d);
	rapidjson::Value to_value(RobinMap<std::string, float> v, rapidjson::Document& d);
}


#endif