/*
 * SomePlayer - An alternate music player for Maemo 5
 * Copyright (C) 2010 Nikolay (somebody) Tischenko <niktischenko@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "playerform.h"
#include "libraryform.h"
#include "busywidget.h"
#include "library.h"

namespace Ui {
	class MainWindow;
}

using SomePlayer::DataObjects::Library;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;

public slots:
	void aboutQt();
	void about();
	void player();
	void library();
	void showBusyWidget(QString);
	void showSearchPanel();
	void hideSearchPanel();
private slots:
	void _add_directory();
	void _save_playlist();
	void _clear_current_playlist();
	void _toggle_search_line();
	void _search(QString);
	void _nextItem();
	void _prevItem();
	void _cancelSearch();
	void _toggle_full_screen();
private:
	PlayerForm *_player_form;
	LibraryForm *_library_form;
	BusyWidget *_busy_widget;
	Library *_library;
};

#endif // MAINWINDOW_H
