#include "helpers.h"

#include <QWidget>
#include <QProcess>
#include <QApplication>

#include <execinfo.h> // for backtrace
#include <dlfcn.h>    // for dladdr
#include <cxxabi.h>   // for __cxa_demangle

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>

#include <unistd.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>


UIDC *UIDC::instance()
{
    static UIDC *inst = new UIDC;
    return inst;
}

void UIDC::less()
{
    this->generationPos+=1;
}

int UIDC::uid()
{
    int v = UIDC::instance()->generationPos;
    UIDC::instance()->less();
    return v;
}

void centerWidget(QWidget *p, QWidget *t)
{
	if (p && t) {
		t->move(p->x() + (p->width() - t->width())/2, p->y() + (p->height() - t->height())/2);
	}
}

std::string Backtrace(int skip = 1)
{
	void *callstack[128];
	const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
	char buf[1024];
	int nFrames = backtrace(callstack, nMaxFrames);
	char **symbols = backtrace_symbols(callstack, nFrames);

	std::ostringstream trace_buf;
	for (int i = skip; i < nFrames; i++) {
		Dl_info info;
		if (dladdr(callstack[i], &info) && info.dli_sname) {
			char *demangled = nullptr;
			int status = -1;
			if (info.dli_sname[0] == '_')
				demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			snprintf(buf, sizeof(buf), "%-3d %*p %s + %zd\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i],
					 status == 0 ? demangled :
					 info.dli_sname == nullptr ? symbols[i] : info.dli_sname,
					 (char *)callstack[i] - (char *)info.dli_saddr);
			free(demangled);
		} else {
			snprintf(buf, sizeof(buf), "%-3d %*p %s\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
		}
		trace_buf << buf;
	}
	free(symbols);
	if (nFrames == nMaxFrames)
		trace_buf << "[truncated]\n";
	return trace_buf.str();
}

const int MAX_STACK_FRAMES = 128;

void printTrace()
{
	void *array[MAX_STACK_FRAMES];
	size_t size = 0;
	char **strings = nullptr;
	size_t i;
	size = static_cast<size_t>(backtrace(array, MAX_STACK_FRAMES));
	strings = backtrace_symbols(array, int(size));
	for (i = 0; i < size; ++i) {
		std::string symbol(strings[i]);
		QString strSymbol = QString::fromStdString(symbol);
		int pos1 = strSymbol.indexOf("[");
		int pos2 = strSymbol.lastIndexOf("]");
		QString address = strSymbol.mid(pos1 + 1,pos2 - pos1 - 1);

		QString cmd = "addr2line -C -f -e " + qApp->arguments().first() + " " + address;
		QProcess *p = new QProcess;
		p->setReadChannel(QProcess::StandardOutput);
		p->start(cmd);
		p->waitForFinished();
		p->waitForReadyRead();
		QString __data = p->readAllStandardOutput();
		delete p;
		p = nullptr;
	}
	std::cerr << Backtrace() << std::endl;

	free(strings);

}
