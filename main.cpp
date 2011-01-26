/*
Copyright (c) 2011, Klaas Neirinck
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of The Media Player Classic - Linux Edition Developers nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QtGui/QApplication>
#include "MainWindow.h"

using std::make_pair;
using std::pair;

pair<QStringList, int> getBackends() {
	int current = 0;
	QStringList backends;

	const QDir backendPath(BACKEND_BASE_PATH + "/phonon_backend");
	if (!backendPath.exists()) {
		backendPath.mkpath(backendPath.path());
	}

	QStringList lib;
	lib << "phonon_*.so";
	foreach (QString path, QApplication::libraryPaths()) {
		const QDir dir(path + "/phonon_backend");
		const QStringList files(dir.entryList(lib, QDir::Files));
		foreach (QString backend, files) {
			backends.push_back(dir.path() + "/" + backend);
		}
	}

	//make sure we have a current backend
	const QString currentBackend(BACKEND_CURRENT);
	if (!QFile::exists(currentBackend)) {
		QFile(backends.last()).link(currentBackend);
	} else { //find the current backend
		const QString link(QFile(currentBackend).symLinkTarget());
		foreach (QString backend, backends) {
			if (backend == link) { //we found the current one
				break;
			}
			++current;
		}
	}

	return make_pair(backends, current);
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	a.setApplicationName("MediaPlayerClassic");

	const pair<QStringList, int> backends(getBackends());
	a.addLibraryPath(BACKEND_BASE_PATH);

	MainWindow w(backends.first, backends.second);
	w.show();

	return a.exec();
}
