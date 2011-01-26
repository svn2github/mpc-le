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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMenuBar>
#include <QMouseEvent>
#include <QUrl>
#include <QSharedPointer>
#include <Phonon/MediaObject>

#define APPLICATION_NAME "Media Player Classic - Linux Edition"
#define APPLICATION_NAME_SHORT "mpc-le"
#define BACKEND_BASE_PATH QDir::homePath() + "/." APPLICATION_NAME_SHORT
#define BACKEND_CURRENT BACKEND_BASE_PATH + "/phonon_backend/current.so"

using Phonon::MediaObject;
using Phonon::MediaSource;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	struct QueuedItem {
		MediaSource mMediaSource;
		QString mShortName;
		QListWidgetItem mListItem;

		explicit QueuedItem(const QUrl& url);
	};

	typedef QSharedPointer<QueuedItem> PlaylistItem;

	Q_OBJECT

	Ui::MainWindow * mUi;
	MediaObject * mMedia;
	QMenu * mContextMenu;
	QAction * mActionCaptionMenu,
		* mActionSeekBar,
		* mActionControls,
		* mActionInformation,
		* mActionStatistics,
		* mActionStatus,
		* mActionPlaylist,
		* mActionFullScreen,
		* mActionOnTopNever,
		* mActionOnTopAlways,
		* mActionOnTopWhilePlaying;
	QPoint mStartMove;
	QMap<QString, QAction *> mPhononBackends;
	QList<PlaylistItem> mQueue;

	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent * event);
	void updateView();
	void dragEnterEvent(QDragEnterEvent * event);
	void dropEvent(QDropEvent * event);
	void clearQueue();
	void enqueue(const QUrl& url);
	QListIterator<PlaylistItem> getCurrentSource();
	void keyPressEvent(QKeyEvent * event);
	PlaylistItem getQueuedItem(QListWidgetItem * item);
public:
	explicit MainWindow(const QStringList& backends, const int currentBackend);
	~MainWindow();

	static void setCurrentBackend(const QString& backend);

public slots:
	void actionClicked(bool);
	void showContextMenu(QPoint point);
	void play();
	void pause();
	void stop();
	void backward();
	void decreaseSpeed();
	void increaseSpeed();
	void forward();
	void stateChanged(Phonon::State to,Phonon::State from);
	void updatePosition(qint64 currentTime);
	void sourceChanged(Phonon::MediaSource source);
	void updateInformation();
	void queueNext();
	void playItem(QListWidgetItem * item);
	void updatePlaylist();
};

#endif // MAINWINDOW_H
