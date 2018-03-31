#include "dbgraph.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
#ifdef USE_CMD_LOG_VIEWER
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle(L"logv");
		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE)
			, FOREGROUND_RED
			| FOREGROUND_GREEN
			| FOREGROUND_BLUE
		);
	}
#endif

	QApplication a(argc, argv);
	dbgraph w;
	w.show();
	return a.exec();
}
