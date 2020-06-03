#ifndef NE_BASE_DEFINE
#define NE_BASE_DEFINE

#include "neutil.h"

namespace node_edit
{
	class NEBase
	{
	protected:
		NEBase() :uid(UID::Generate()) {}
		NEBase(const NEBase& _t) :uid(UID::Generate()) {}

	public:
		const uint32_t uid;

		bool operator==(const NEBase& _t) {
			return uid == _t.uid;
		}
		bool operator!=(const NEBase& _t) {
			return uid != _t.uid;
		}

		virtual rapidjson::Value PrintSelf(rapidjson::Document& d) {
			rapidjson::Value o(rapidjson::kObjectType);
			o.AddMember("uid", to_value(uid, d), d.GetAllocator());
			return o;
		}

	};
}

#endif
