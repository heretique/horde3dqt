#include "genprofiling.h"

#if !defined(Q_WS_MAEMO_5)
int QLogging::getMicroSecondsCount()
	{
#if defined(Q_OS_SYMBIAN)
	return User::FastCounter();
#else
	return -1;
#endif
	}

#endif // Q_WS_MAEMO_5
