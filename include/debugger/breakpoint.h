#pragma once

#include <map>
#include <string>
#include <deque>
#include <base/util/hybrid_array.h> 
#include <debugger/pathconvert.h>
#include <debugger/protocol.h>
#include <debugger/path.h>
#include <debugger/hashmap.h>

struct lua_State;
namespace lua { struct Debug; }

namespace vscode
{
	class breakpoint;
	struct bp_source;

	struct source {
		bool vaild = false;
		std::string path;
		intptr_t ref = 0;
		source();
		source(rapidjson::Value const& info);
		void output(wprotocol& res);
	};

	struct bp_breakpoint {
		size_t id;
		unsigned int line;
		bool verified;
		unsigned int verified_line;

		std::string cond;
		std::string hitcond;
		std::string log;
		int hit;

		bp_breakpoint(size_t id, rapidjson::Value const& info, int h);
		bool verify(bp_source& src, debugger_impl* dbg = nullptr);
		bool run(lua_State* L, lua::Debug* ar, debugger_impl* dbg);
		void output(wprotocol& res);
	};

	enum class eLine : uint8_t {
		unknown = 0,
		undef = 1,
		defined = 2,
	};

	struct bp_source {
		source src;
		std::deque<eLine>               defined;
		//std::vector<bp_breakpoint>      waitverfy;
		std::map<size_t, bp_breakpoint> verified;

		bp_source(source& s);
		void update(lua_State* L, lua::Debug* ar, debugger_impl* dbg);

		bp_breakpoint& add(size_t line, rapidjson::Value const& bpinfo, size_t& next_id);
		bp_breakpoint* get(size_t line);
		void clear();
		bool has_breakpoint();
	};

	struct bp_function {
		bp_source* src;
		bp_function(lua_State* L, lua::Debug* ar, debugger_impl* dbg, breakpoint* breakpoint);
	};

	class breakpoint
	{
	public:
		breakpoint(debugger_impl* dbg);
		void clear();
		bool has(bp_source* src, size_t line, lua_State* L, lua::Debug* ar) const;
		bp_function* get_function(lua_State* L, lua::Debug* ar);
		bp_source&   get_source(source& source);
		void set_breakpoint(source& s, rapidjson::Value const& args, wprotocol& res);

	private:
		debugger_impl* dbg_;
		std::map<std::string, bp_source, path::less<std::string>> files_;
		std::map<intptr_t, bp_source>    memorys_;
		hashmap<bp_function>             functions_;
		size_t                           next_id_;
	};
}
