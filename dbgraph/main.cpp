#include "dbgraph.h"
#include <QtWidgets/QApplication>
#include <qmessagebox.h>
#include <qdesktopwidget.h>

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

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	dbgraph w;
	w.show();

	return a.exec();
}
