#include "../include/neutil.h"

namespace node_edit
{
	uint32_t UID::Generate() {
		static uint32_t curid = 0;
		static std::mutex m;
		std::lock_guard<std::mutex> lock(m);
		curid++;
		return curid;
	}


	rapidjson::Value empty_value() {
		return rapidjson::Value("NULL");
	}
	rapidjson::Value to_value(uint32_t v, rapidjson::Document& d) {
		rapidjson::Value rv;
		rv.SetString(std::to_string(v).c_str(), d.GetAllocator());
		return rv;
	}
	rapidjson::Value to_value(std::string v, rapidjson::Document& d) {
		rapidjson::Value rv;
		rv.SetString(v.c_str(),d.GetAllocator());
		return rv;
	}
	rapidjson::Value to_value(std::vector<std::string> v, rapidjson::Document& d) {
		rapidjson::Value a(rapidjson::kArrayType);
		for (size_t i = 0; i < v.size(); i++)
		{
			a.PushBack(to_value(v[i], d), d.GetAllocator());
		}
		return a;
	}
	rapidjson::Value to_value(std::vector<int> v, rapidjson::Document& d) {
		rapidjson::Value a(rapidjson::kArrayType);
		for (size_t i = 0; i < v.size(); i++)
		{
			a.PushBack(v[i], d.GetAllocator());
		}
		return a;
	}
	rapidjson::Value to_value(std::vector<float> v, rapidjson::Document& d) {
		rapidjson::Value a(rapidjson::kArrayType);
		for (size_t i = 0; i < v.size(); i++)
		{
			a.PushBack(v[i], d.GetAllocator());
		}
		return a;
	}
	rapidjson::Value to_value(RobinMap<std::string, float> v, rapidjson::Document& d) {
		rapidjson::Value a(rapidjson::kObjectType);
		for (auto itr : v)
		{
			rapidjson::Value b(rapidjson::kNumberType);
			a.AddMember(to_value(itr.first, d),b.SetFloat(itr.second), d.GetAllocator());
		}
		return a;
	}
	
}