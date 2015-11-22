#include "TBBContextID.h"

using namespace Managers::Task;

tbb::atomic<int32_t> TBBContextID::g_ContextIDCount;
tbb::enumerable_thread_specific<int32_t> TBBContextID::g_ContextID;