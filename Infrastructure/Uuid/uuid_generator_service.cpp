#include "uuid_generator_service.h"

// platform-native uuid generation: Win32 RPC on Windows, libuuid (BSD/util-linux API) elsewhere
#ifdef _WIN32
	#include <rpc.h>
#else
	#include <uuid/uuid.h>
#endif

std::string UuidGeneratorService::GenerateUuid() {
#ifdef _WIN32
	UUID uuid;
	UuidCreate(&uuid);

	RPC_CSTR str = nullptr;
	UuidToStringA(&uuid, &str);
	std::string result(reinterpret_cast<char*>(str));
	RpcStringFreeA(&str);

	return result;
#else
	uuid_t uuid;
	uuid_generate(uuid);

	char str[37];
	uuid_unparse_lower(uuid, str);

	return std::string(str);
#endif
}
