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

#include <QFileInfo>
#include <QTime>
#include <Phonon/AudioOutput>
#include "MainWindow.h"
#include "ui_MainWindow.h"

using namespace Phonon;

MainWindow::QueuedItem::QueuedItem(const QUrl& url) : mMediaSource(url), mShortName(QFileInfo(url.path()).baseName()) {
	mListItem.setText(mShortName);
}

MainWindow::MainWindow(const QStringList& backends, const int currentBackend) : QMainWindow(0), mUi(new Ui::MainWindow),
	mMedia(new MediaObject(this)), mContextMenu(new QMenu(this)) {

	mUi->setupUi(this);
	setWindowTitle(QApplication::applicationName());

	//parse backends
	int i = 0;
	QMap<QString, QAction *> sortedBackends;
	QActionGroup * actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);
	foreach (QString backend, backends) {
		QAction * action = new QAction(this);
		action->setObjectName(backend);
		action->setCheckable(true);
		if (i == currentBackend) {
			action->setChecked(true);
		} else {
			action->setChecked(false);
		}
		QString shortName(backend.split('_').last());
		shortName.chop(3);
		action->setText(shortName);
		actionGroup->addAction(action);

		connect( action, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );

		mPhononBackends.insert(backend, action);
		sortedBackends.insert(shortName, action);
		++i;
	}

	//create menu
	mActionCaptionMenu = new QAction(this);
	mActionSeekBar = new QAction(this);
	mActionControls = new QAction(this);
	mActionInformation = new QAction(this);
	mActionStatistics = new QAction(this);
	mActionStatus = new QAction(this);
	mActionPlaylist = new QAction(this);
	mActionFullScreen = new QAction(this);
	mActionOnTopNever = new QAction(this);
	mActionOnTopAlways = new QAction(this);
	mActionOnTopWhilePlaying = new QAction(this);

	actionGroup = new QActionGroup(this);
	actionGroup->setEnabled(true);
	actionGroup->addAction(mActionOnTopNever);
	actionGroup->addAction(mActionOnTopAlways);
	actionGroup->addAction(mActionOnTopWhilePlaying);

	connect( mActionCaptionMenu, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionControls, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionInformation, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionSeekBar, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionStatistics, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionStatus, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionPlaylist, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionFullScreen, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionOnTopNever, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionOnTopAlways, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );
	connect( mActionOnTopWhilePlaying, SIGNAL(toggled(bool)), this, SLOT(actionClicked(bool)) );

	mActionCaptionMenu->setObjectName(QString::fromUtf8("actionCaptionMenu"));
	mActionCaptionMenu->setCheckable(true);
	mActionCaptionMenu->setChecked(true);
	mActionCaptionMenu->setText(QApplication::translate("MainWindow", "Caption && Menu", 0, QApplication::UnicodeUTF8));

	mActionSeekBar->setObjectName(QString::fromUtf8("actionSeekBar"));
	mActionSeekBar->setCheckable(true);
	mActionSeekBar->setChecked(true);
	mActionSeekBar->setText(QApplication::translate("MainWindow", "Seek Bar", 0, QApplication::UnicodeUTF8));

	mActionControls->setObjectName(QString::fromUtf8("actionControls"));
	mActionControls->setCheckable(true);
	mActionControls->setChecked(true);
	mActionControls->setText(QApplication::translate("MainWindow", "Controls", 0, QApplication::UnicodeUTF8));

	mActionInformation->setObjectName(QString::fromUtf8("actionInformation"));
	mActionInformation->setCheckable(true);
	mActionInformation->setChecked(false);
	mActionInformation->setText(QApplication::translate("MainWindow", "Information", 0, QApplication::UnicodeUTF8));

	mActionStatistics->setObjectName(QString::fromUtf8("actionStatistics"));
	mActionStatistics->setCheckable(true);
	mActionStatistics->setChecked(false);
	mActionStatistics->setText(QApplication::translate("MainWindow", "Statistics", 0, QApplication::UnicodeUTF8));

	mActionStatus->setObjectName(QString::fromUtf8("actionStatus"));
	mActionStatus->setCheckable(true);
	mActionStatus->setChecked(true);
	mActionStatus->setText(QApplication::translate("MainWindow", "Status", 0, QApplication::UnicodeUTF8));

	mActionPlaylist->setObjectName(QString::fromUtf8("actionPlaylist"));
	mActionPlaylist->setCheckable(true);
	mActionPlaylist->setChecked(false);
	mActionPlaylist->setText(QApplication::translate("MainWindow", "Playlist", 0, QApplication::UnicodeUTF8));

	mActionFullScreen->setObjectName(QString::fromUtf8("actionFullScreen"));
	mActionFullScreen->setCheckable(true);
	mActionFullScreen->setChecked(false);
	mActionFullScreen->setText(QApplication::translate("MainWindow", "Full Screen", 0, QApplication::UnicodeUTF8));

	mActionOnTopNever->setObjectName(QString::fromUtf8("actionOnTopNever"));
	mActionOnTopNever->setCheckable(true);
	mActionOnTopNever->setChecked(true);
	mActionOnTopNever->setText(QApplication::translate("MainWindow", "Never", 0, QApplication::UnicodeUTF8));

	mActionOnTopAlways->setObjectName(QString::fromUtf8("actionOnTopAlways"));
	mActionOnTopAlways->setCheckable(true);
	mActionOnTopAlways->setChecked(false);
	mActionOnTopAlways->setText(QApplication::translate("MainWindow", "Always", 0, QApplication::UnicodeUTF8));

	mActionOnTopWhilePlaying->setObjectName(QString::fromUtf8("actionOnTopWhilePlaying"));
	mActionOnTopWhilePlaying->setCheckable(true);
	mActionOnTopWhilePlaying->setChecked(false);
	mActionOnTopWhilePlaying->setText(QApplication::translate("MainWindow", "While Playing", 0, QApplication::UnicodeUTF8));

	QMenu * menuView = new QMenu(this);
	menuView->setObjectName(QString::fromUtf8("menuView"));
	menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));

	QMenu * menuFile = new QMenu(this);
	menuFile->setObjectName(QString::fromUtf8("menuFile"));
	menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));

	QMenu * menuPlay = new QMenu(this);
	menuPlay->setObjectName(QString::fromUtf8("menuPlay"));
	menuPlay->setTitle(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));

	QMenu * menuNavigate = new QMenu(this);
	menuNavigate->setObjectName(QString::fromUtf8("menuNavigate"));
	menuNavigate->setTitle(QApplication::translate("MainWindow", "Navigate", 0, QApplication::UnicodeUTF8));

	QMenu * menuFavorites = new QMenu(this);
	menuFavorites->setObjectName(QString::fromUtf8("menuFavorites"));
	menuFavorites->setTitle(QApplication::translate("MainWindow", "Favorites", 0, QApplication::UnicodeUTF8));

	QMenu * menuHelp = new QMenu(this);
	menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
	menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));

	QMenu * menuBackend = new QMenu(this);
	menuBackend->setObjectName(QString::fromUtf8("menuBackend"));
	menuBackend->setTitle(QApplication::translate("MainWindow", "Backend", 0, QApplication::UnicodeUTF8));

	QMenu * menuOnTop = new QMenu(this);
	menuOnTop->setObjectName(QString::fromUtf8("menuOnTop"));
	menuOnTop->setTitle(QApplication::translate("MainWindow", "On Top", 0, QApplication::UnicodeUTF8));

	menuView->addAction(mActionCaptionMenu);
	menuView->addAction(mActionSeekBar);
	menuView->addAction(mActionControls);
	menuView->addAction(mActionInformation);
	menuView->addAction(mActionStatistics);
	menuView->addAction(mActionStatus);
	menuView->addAction(mActionPlaylist);
	menuView->addSeparator();
	menuView->addAction(mActionFullScreen);
	menuView->addSeparator();
	menuOnTop->addAction(mActionOnTopNever);
	menuOnTop->addAction(mActionOnTopAlways);
	menuOnTop->addAction(mActionOnTopWhilePlaying);
	menuView->addAction(menuOnTop->menuAction());

	foreach (QAction * action, sortedBackends.values()) {
		menuBackend->addAction(action);
	}

	menuBar()->addAction(menuFile->menuAction());
	mContextMenu->addAction(menuFile->menuAction());
	menuBar()->addAction(menuView->menuAction());
	mContextMenu->addAction(menuView->menuAction());
	//menuBar()->addAction(menuPlay->menuAction());
	//mContextMenu->addAction(menuPlay->menuAction());
	//menuBar()->addAction(menuNavigate->menuAction());
	//mContextMenu->addAction(menuNavigate->menuAction());
	//menuBar()->addAction(menuFavorites->menuAction());
	//mContextMenu->addAction(menuFavorites->menuAction());
	menuBar()->addAction(menuBackend->menuAction());
	mContextMenu->addAction(menuBackend->menuAction());
	//menuBar()->addAction(menuHelp->menuAction());
	//mContextMenu->addAction(menuHelp->menuAction());

	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)) );
	setContextMenuPolicy(Qt::CustomContextMenu);

	//media stuff
	connect( mUi->mPlay, SIGNAL(clicked()), this, SLOT(play()) );
	connect( mUi->mPause, SIGNAL(clicked()), this, SLOT(pause()) );
	connect( mUi->mStop, SIGNAL(clicked()), this, SLOT(stop()) );
	connect( mUi->mSkipBack, SIGNAL(clicked()), this, SLOT(backward()) );
	connect( mUi->mSkipForward, SIGNAL(clicked()), this, SLOT(forward()) );
	connect( mUi->mDecreaseSpeed, SIGNAL(clicked()), this, SLOT(decreaseSpeed()) );
	connect( mUi->mIncreaseSpeed, SIGNAL(clicked()), this, SLOT(increaseSpeed()) );

	AudioOutput * audio = new AudioOutput(Phonon::MusicCategory, this);
	createPath(mMedia, mUi->mVideo);
	createPath(mMedia, audio);
	mUi->mSeekBar->setMediaObject(mMedia);
	mUi->mSoundBar->setAudioOutput(audio);
	mMedia->setTickInterval(100);

	connect( mMedia, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this,
		SLOT(stateChanged(Phonon::State,Phonon::State)) );
	connect( mMedia, SIGNAL(tick(qint64)), this, SLOT(updatePosition(qint64)) );
	connect( mMedia, SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SLOT(sourceChanged(Phonon::MediaSource)) );
	connect( mMedia, SIGNAL(metaDataChanged()), this, SLOT(updateInformation()) );
	connect( mMedia, SIGNAL(aboutToFinish()), this, SLOT(queueNext()) );

	//playlist stuff
	setAcceptDrops(true);

	connect( mUi->mPlaylist, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(playItem(QListWidgetItem*)) );
	connect( mUi->mPlaylist, SIGNAL(itemDropped()), this, SLOT(updatePlaylist()) );

	//TO DELETE
	//enqueue(QUrl("/home/klaas/Downloads/trailer_480p.mov"));
	//enqueue(QUrl("/home/klaas/Downloads/pirates.mov"));
}

MainWindow::~MainWindow() {
	delete mUi;
}

void MainWindow::mousePressEvent(QMouseEvent * event) {
	if (event->button() == Qt::LeftButton) {
		if (!isFullScreen() && !mActionCaptionMenu->isChecked()) {
			mStartMove = event->globalPos() - pos();
		} else if ( mUi->mVideo->geometry().contains(event->pos()) ) {
			pause();
		}
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *) {
	mStartMove = QPoint();
}

void MainWindow::mouseMoveEvent(QMouseEvent * event) {
	if (!mStartMove.isNull()) {
		move(event->globalPos() - mStartMove);
	}
}

void MainWindow::updateView() {
	Qt::WindowFlags flags(windowFlags());

	if (mActionCaptionMenu->isChecked()) {
		flags &= ~Qt::FramelessWindowHint;
		if (!isFullScreen()) {
			menuBar()->show();
		} else {
			menuBar()->hide();
		}
	} else {
		flags |= Qt::FramelessWindowHint;
		menuBar()->hide();
	}

	if (mActionControls->isChecked()) {
		mUi->mControls->show();
	} else {
		mUi->mControls->hide();
	}

	if (mActionInformation->isChecked()) {
		mUi->mInformation->show();
	} else {
		mUi->mInformation->hide();
	}

	if (mActionSeekBar->isChecked()) {
		mUi->mSeekBar->show();
	} else {
		mUi->mSeekBar->hide();
	}

	if (mActionStatistics->isChecked()) {
		mUi->mStatistics->show();
	} else {
		mUi->mStatistics->hide();
	}

	if (mActionStatus->isChecked()) {
		mUi->mStatus->show();
	} else {
		mUi->mStatus->hide();
	}

	if (mActionPlaylist->isChecked()) {
		mUi->mPlaylist->show();
	} else {
		mUi->mPlaylist->hide();
	}

	if (!isFullScreen()) {
		if (mActionOnTopNever->isChecked() || mActionOnTopWhilePlaying->isChecked()) {
			flags &= ~Qt::WindowStaysOnTopHint;
		} else if (mActionOnTopAlways->isChecked()) {
			flags |= Qt::WindowStaysOnTopHint;
		}
	}

	setWindowFlags(flags);
	show();
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event) {
	event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent * event) {
	if (event->mimeData()->hasUrls()) {
		if ( !mUi->mPlaylist->geometry().contains(event->pos()) ) {
			clearQueue();
		}
		foreach (QUrl url, event->mimeData()->urls()) {
			enqueue(url);
		}
	}
}

void MainWindow::clearQueue() {
	mMedia->clear();
	mQueue.clear();
	mUi->mPlaylist->clear();
}

void MainWindow::enqueue(const QUrl& url) {
	PlaylistItem item(new QueuedItem(url));
	if (item->mMediaSource.type() != Phonon::MediaSource::Invalid &&
		item->mMediaSource.type() != Phonon::MediaSource::Empty) {

		mQueue.push_back(item);
		mUi->mPlaylist->addItem(&item->mListItem);
		mMedia->enqueue(item->mMediaSource);
		updatePlaylist();
	}
}

QListIterator<MainWindow::PlaylistItem> MainWindow::getCurrentSource() {
	QListIterator<PlaylistItem> queueIt(mQueue);
	while (queueIt.hasNext()) {
		if (queueIt.next()->mMediaSource == mMedia->currentSource()) {
			return queueIt;
		}
	}
	return queueIt;
}

void MainWindow::keyPressEvent(QKeyEvent * event) {
	if (mUi->mPlaylist->hasFocus()) {
		if (!mUi->mPlaylist->selectedItems().isEmpty()) {
			if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) { //play
				playItem(mUi->mPlaylist->selectedItems().first());
			} else if (event->key() == Qt::Key_Delete) { //delete item
				PlaylistItem item( getQueuedItem(mUi->mPlaylist->selectedItems().first()) );
				mUi->mPlaylist->takeItem(mUi->mPlaylist->row(&item->mListItem));
				mQueue.removeOne(item);
				if (item->mMediaSource == mMedia->currentSource()) {
					queueNext();
					stop();
					if (!mQueue.isEmpty()) {
						mMedia->setCurrentSource(mMedia->queue().first());
					} else {
						mMedia->clear();
					}
				}
				updatePlaylist();
			}
		}
	} else {
		if (event->key() == Qt::Key_Space) { //pause
			pause();
		}
	}
}

MainWindow::PlaylistItem MainWindow::getQueuedItem(QListWidgetItem * item) {
	for (QList<PlaylistItem>::iterator queuedItem(mQueue.begin()); queuedItem != mQueue.end(); ++queuedItem) {
		if (&(*queuedItem)->mListItem == item) {
			return *queuedItem;
		}
	}
	return PlaylistItem();
}

void MainWindow::setCurrentBackend(const QString& backend) {
	if (QFile::exists(BACKEND_CURRENT)) {
		QFile::remove(BACKEND_CURRENT);
	}
	QFile(backend).link(BACKEND_CURRENT);
}

void MainWindow::actionClicked(bool checked) {
	const QObject * const sender = QObject::sender();

	if (checked) {
		if (sender == mActionFullScreen) {
			showFullScreen();
		} else {
			for (QMap<QString, QAction *>::const_iterator backendIt(mPhononBackends.begin());
				backendIt != mPhononBackends.end(); ++backendIt) {

				if (sender == backendIt.value()) {
					setCurrentBackend(backendIt.key());
				}
			}
		}
	} else {
		if (sender == mActionFullScreen) {
			showNormal();
		}
	}

	updateView();
}

void MainWindow::showContextMenu(QPoint point) {
	mContextMenu->exec(mapToGlobal(point));
}

void MainWindow::play() {
	mMedia->play();
}

void MainWindow::pause() {
	if (mMedia->state() != Phonon::PausedState) {
		mMedia->pause();
	} else {
		mMedia->play();
	}
}

void MainWindow::stop() {
	mMedia->stop();
	mMedia->seek(0);
}

void MainWindow::backward() {
	const bool startPlay(mMedia->state() == Phonon::PlayingState);
	QListIterator<PlaylistItem> source(getCurrentSource());
	source.previous();
	if (source.hasPrevious()) {
		mMedia->setCurrentSource(source.previous()->mMediaSource);
		if (startPlay) {
			play();
		}
	} else {
		stop();
	}
}

void MainWindow::decreaseSpeed() {
}

void MainWindow::increaseSpeed() {
}

void MainWindow::forward() {
	const bool startPlay(mMedia->state() == Phonon::PlayingState);
	QListIterator<PlaylistItem> source(getCurrentSource());
	if (source.hasNext()) {
		mMedia->setCurrentSource(source.next()->mMediaSource);
		if (startPlay) {
			play();
		}
	} else {
		stop();
	}
}

void MainWindow::stateChanged(Phonon::State to, Phonon::State) {
	if (mActionOnTopWhilePlaying->isChecked()) { //update on top if needed
		Qt::WindowFlags flags(windowFlags());
		if (to == Phonon::PlayingState) {
			flags |= Qt::WindowStaysOnTopHint;
		} else {
			flags &= ~Qt::WindowStaysOnTopHint;
		}
		setWindowFlags(flags);
		show();
	}

	//update status
	if (mMedia->currentSource().type() != Phonon::MediaSource::Empty) {
		switch (to) {
			case Phonon::LoadingState:
				mUi->mLabelState->setText("Loading");
				break;
			case Phonon::StoppedState:
				mUi->mLabelState->setText("Stopped");
				break;
			case Phonon::PlayingState:
				mUi->mLabelState->setText("Playing");
				break;
			case Phonon::BufferingState:
				mUi->mLabelState->setText("Buffering");
				break;
			case Phonon::PausedState:
				mUi->mLabelState->setText("Paused");
				break;
			case Phonon::ErrorState:
				mUi->mLabelState->setText("Error");
				break;
		}
		updatePosition(mMedia->currentTime());
	} else {
		mUi->mLabelState->clear();
		mUi->mLabelPosition->clear();
	}
}

void MainWindow::updatePosition(qint64 currentTime) {
	QTime total;
	total = total.addMSecs(mMedia->totalTime());
	QTime current;
	current = current.addMSecs(currentTime);
	mUi->mLabelPosition->setText(current.toString("HH:mm:ss") + " / " + total.toString("HH:mm:ss"));
}

void MainWindow::sourceChanged(Phonon::MediaSource source) {
	if (source.type() == Phonon::MediaSource::Empty) {
		setWindowTitle(QApplication::applicationName());
	} else {
		const QFileInfo info(source.fileName());
		setWindowTitle(info.fileName());

		for (QList<PlaylistItem>::iterator item(mQueue.begin()); item != mQueue.end(); ++item) {
			if ((*item)->mMediaSource == source) {
				(*item)->mListItem.setTextColor(Qt::red);
			} else {
				(*item)->mListItem.setTextColor(Qt::black);
			}
		}
	}
}

void MainWindow::updateInformation() {
	if (mMedia->currentSource().type() != Phonon::MediaSource::Empty) {
		const QStringList title(mMedia->metaData(Phonon::TitleMetaData)),
			artist(mMedia->metaData(Phonon::ArtistMetaData)),
			description(mMedia->metaData(Phonon::DescriptionMetaData));

		if (!title.isEmpty() || !artist.isEmpty() || !description.isEmpty()) { //set the info
			if (!title.isEmpty()) {
				mUi->mLabelTitle->setText(title.first());
			} else {
				mUi->mLabelTitle->clear();
			}
			if (!artist.isEmpty()) {
				mUi->mLabelArtist->setText(artist.first());
			} else {
				mUi->mLabelArtist->clear();
			}
			if (!description.isEmpty()) {
				mUi->mLabelDescription->setText(description.first());
			} else {
				mUi->mLabelDescription->clear();
			}
			mUi->mLabelTitle->show();
			mUi->mLabelTitleText->show();
			mUi->mLabelArtist->show();
			mUi->mLabelArtistText->show();
			mUi->mLabelDescription->show();
			mUi->mLabelDescriptionText->show();
			return;
		}
	}

	//hide everything
	mUi->mLabelTitle->hide();
	mUi->mLabelTitleText->hide();
	mUi->mLabelArtist->hide();
	mUi->mLabelArtistText->hide();
	mUi->mLabelDescription->hide();
	mUi->mLabelDescriptionText->hide();
}

void MainWindow::queueNext() {
	QListIterator<PlaylistItem> current(getCurrentSource());
	if (current.hasNext()) {
		mMedia->enqueue(current.next()->mMediaSource);
	}
}

void MainWindow::playItem(QListWidgetItem * item) {
	for (QList<PlaylistItem>::iterator queuedItem(mQueue.begin()); queuedItem != mQueue.end(); ++queuedItem) {
		if (&(*queuedItem)->mListItem == item) {
			mMedia->setCurrentSource((*queuedItem)->mMediaSource);
			play();
			break;
		}
	}
}

void MainWindow::updatePlaylist() {
	QList<PlaylistItem> newQueue;

	for (int i = 0; i < mUi->mPlaylist->count(); ++i) {
		QListWidgetItem * item = mUi->mPlaylist->item(i);
		PlaylistItem queuedItem(getQueuedItem(item));
		item->setText(QString::number(i + 1) + ". " + queuedItem->mShortName);
		newQueue.push_back(queuedItem);
	}
	mQueue = newQueue;
}
