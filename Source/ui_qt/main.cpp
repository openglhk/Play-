#include <QApplication>
#include <QCommandLineParser>
#include "mainwindow.h"
#include "QStringUtils.h"
#include <QtGlobal>

#ifdef _WIN32
#include "BugTrap.h"
#include <tchar.h>
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
	BT_SetFlags(BTF_DETAILEDMODE);
	BT_SetSupportURL(_T("https://purei.org"));
	BT_InstallSehFilter();
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QApplication a(argc, argv);

	QCoreApplication::setApplicationName("Play!");
	QCoreApplication::setApplicationVersion("Version: " PLAY_VERSION);

	QCommandLineParser parser;
	parser.setApplicationDescription("Description: A multiplatform PS2 emulator.");
	parser.addHelpOption();
	parser.addVersionOption();

#ifdef DEBUGGER_INCLUDED
	QCommandLineOption debugger_option("debugger", "Show debugger");
	parser.addOption(debugger_option);
	QCommandLineOption frame_debugger_option("framedebugger", "Show frame debugger");
	parser.addOption(frame_debugger_option);
#endif
	QCommandLineOption cdrom_image_option("cdrom0", "Boot last booted cdvd image");
	parser.addOption(cdrom_image_option);

	QCommandLineOption disc_image_option("disc", "Boot any supported disc image", "disc_image");
	parser.addOption(disc_image_option);

	QCommandLineOption elf_image_option("elf", "Boot supported elf image", "elf_image");
	parser.addOption(elf_image_option);

	QCommandLineOption load_state_option("state", "Load state at index", "state_index");
	parser.addOption(load_state_option);

	parser.process(a);

	MainWindow w;
	w.show();

	if(parser.isSet(cdrom_image_option))
	{
		try
		{
			w.BootCDROM();
		}
		catch(const std::exception& e)
		{
			printf("Error: %s\r\n", e.what());
		}
	}
	else if(parser.isSet(disc_image_option))
	{
		QString disc_image = parser.value(disc_image_option);
		w.LoadCDROM(QStringToPath(disc_image));
		w.BootCDROM();
	}
	else if(parser.isSet(elf_image_option))
	{
		QString elf_image = parser.value(elf_image_option);
		w.BootElf(QStringToPath(elf_image));
	}

	if(parser.isSet(load_state_option))
	{
		QString stateIndex = parser.value(load_state_option);
		w.loadState(stateIndex.toInt());
	}

#ifdef DEBUGGER_INCLUDED
	if(parser.isSet(debugger_option))
	{
		w.ShowDebugger();
	}
	if(parser.isSet(frame_debugger_option))
	{
		w.ShowFrameDebugger();
	}
#endif
	return a.exec();
}
