#include "libraryform.h"
#include "ui_libraryform.h"
#include "library.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include <QModelIndexList>
#include "track.h"
#include "playlist.h"
#include <QDebug>
#include <QTime>

using namespace SomePlayer::DataObjects;

inline QString __format_track_string(TrackMetadata meta) {
	int minutes = meta.length() / 60;
	int seconds = meta.length() % 60;
	QTime time(0, minutes, seconds);
	return QString("[%1] %2").arg(time.toString("mm:ss")).arg(meta.title());

}

inline void __fill_model(QStandardItemModel *model, QList<QString> data) {
	model->clear();
	int count = data.count();
	model->setRowCount(count);
	for (int i = 0; i < count; i++) {
		model->setItem(i, 0, new QStandardItem(data.at(i)));
	}
}

inline void __fill_model_tracks (QStandardItemModel *model, QList<Track> tracks) {
	int count = tracks.count();
	model->setRowCount(count);
	for (int i = 0; i < count; i++) {
		TrackMetadata meta = tracks.at(i).metadata();
		model->setItem(i, 0, new QStandardItem(__format_track_string(meta)));
	}
}

LibraryForm::LibraryForm(Library *lib, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryForm)
{
	_lib = lib;
	_model = new QStandardItemModel(this);
	_state = STATE_NONE;
    ui->setupUi(this);
	connect(ui->playerButton, SIGNAL(clicked()), this, SLOT(_player()));
	connect(ui->viewButton, SIGNAL(clicked()), this, SLOT(_view_button()));
	connect(ui->playlistsButton, SIGNAL(clicked()), this, SLOT(_playlists_button()));
	connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(_process_list_click(QModelIndex)));
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(_add_button()));
	connect(ui->backButton, SIGNAL(clicked()), this, SLOT(_back_button()));
	_view_button();
}

LibraryForm::~LibraryForm()
{
    delete ui;
}

void LibraryForm::_player() {
	emit player();
}

void LibraryForm::_view_button() {
	QList<QString> artitst = _lib->getArtists();
	__fill_model(_model, artitst);
	ui->listView->setModel(_model);
	_state = STATE_ARTIST;
	ui->backButton->setEnabled(false);
	ui->listLabel->setText("Artists");
}

void LibraryForm::_dynamic_button() {
}

void LibraryForm::_process_list_click(QModelIndex index) {
	if (_state == STATE_NONE) return;
	QString data = index.data().toString();
	switch (_state) {
	case STATE_ARTIST:
		__fill_model(_model, _lib->getAlbumsForArtist(data));
		_current_artist = data;
		_state = STATE_ALBUM;
		ui->backButton->setEnabled(true);
		ui->listLabel->setText(QString("Albums by \"%1\"").arg(_current_artist));
		break;
	case STATE_ALBUM:
		_current_album = data;
		_current_tracks = _lib->getTracksForAlbum(data, _current_artist);
		__fill_model_tracks(_model, _current_tracks);
		_state = STATE_TRACK;
		ui->backButton->setEnabled(true);
		ui->listLabel->setText(QString("Tracks from \"%1\" by \"%2\"").arg(_current_album).arg(_current_artist));
		break;
	case STATE_PLAYLIST:
		{
			Playlist playlist = _lib->getPlaylist(data);
			_current_tracks = playlist.tracks();
			__fill_model_tracks(_model, _current_tracks);
			_state = STATE_PLAYLIST_TRACK;
			ui->backButton->setEnabled(true);
			ui->listLabel->setText(QString("Tracks in playlist \"%1\"").arg(data));
		}
		break;
	default:
		return;
	}
}

void LibraryForm::_add_button() {
	if (_state == STATE_NONE) return;
	QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
	ui->listView->selectionModel()->clearSelection();
	switch (_state) {
	case STATE_ARTIST:
		foreach (QModelIndex id, selected) {
			_add_artist(id.data().toString());
		}
		break;
	case STATE_ALBUM:
		foreach (QModelIndex id, selected) {
			_add_album(_current_artist, id.data().toString());
		}
		break;
	case STATE_TRACK:
		foreach (QModelIndex id, selected) {
			_add_track(_current_tracks.at(id.row()));
		}
		break;
	case STATE_PLAYLIST:
		foreach (QModelIndex id, selected) {
			_add_playlist(id.data().toString());
		}
		break;
	case STATE_PLAYLIST_TRACK:
		foreach (QModelIndex id, selected) {
			_add_track(_current_tracks.at(id.row()));
		}
		break;
	default:
		return;
	}
}


void LibraryForm::_add_artist(QString artist) {
	qDebug() << "adding ARTIST " << artist;
	QList<QString> albums = _lib->getAlbumsForArtist(artist);
	foreach(QString album, albums) {
		_add_album(artist, album);
	}
}

void LibraryForm::_add_album(QString artist, QString album) {
	qDebug() << "adding ALBUM " << album << " by " << artist;
	QList<Track> tracks = _lib->getTracksForAlbum(album, artist);
	foreach(Track track, tracks) {
		_add_track(track);
	}
}

void LibraryForm::_add_track(Track track) {
	qDebug() << "adding TRACK " << track.metadata().title() << " from " << track.metadata().album() << " by " << track.metadata().artist();
}

void LibraryForm::_add_playlist(QString name) {
	qDebug() << "adding playlist \"" << name << "\"";
	Playlist playlist = _lib->getPlaylist(name);
	QList<Track> tracks = playlist.tracks();
	foreach (Track track, tracks) {
		_add_track(track);
	}
}

void LibraryForm::_back_button() {
	switch (_state) {
	case STATE_ALBUM:
		_view_button();
		break;
	case STATE_TRACK:
		__fill_model(_model, _lib->getAlbumsForArtist(_current_artist));
		_state = STATE_ALBUM;
		ui->listLabel->setText(QString("Albums by \"%1\"").arg(_current_artist));
		break;
	case STATE_PLAYLIST_TRACK:
		_playlists_button();
	default:
		return;
	}
}

void LibraryForm::_playlists_button() {
	QList<QString> playlists = _lib->getPlaylistsNames();
	__fill_model(_model, playlists);
	ui->listView->setModel(_model);
	_state = STATE_PLAYLIST;
	ui->backButton->setEnabled(false);
	ui->listLabel->setText("Playlists");
}
