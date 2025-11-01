#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DatasetLoader.h"
#include <QString>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QWidget>

QString MainWindow::truncateText(const QString &text, int maxLength) {
    return (text.length() <= maxLength) ? text : text.left(maxLength - 3) + "...";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnPopularity, &QPushButton::clicked, this, &MainWindow::on_btnPopularity_clicked);
    connect(ui->btnDanceability, &QPushButton::clicked, this, &MainWindow::on_btnDanceability_clicked);
    connect(ui->btnEnergy, &QPushButton::clicked, this, &MainWindow::on_btnEnergy_clicked);
    connect(ui->btnDuration, &QPushButton::clicked, this, &MainWindow::on_btnDuration_clicked);

    loadKaggleData();
    on_btnPopularity_clicked();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadKaggleData() {
    DatasetLoader loader;
    const std::string kagglePath = "tracks.csv";
    songs = loader.loadFromCSV(kagglePath, 50000);

    for (auto &s : songs) s.computeVotes();
    populateHeaps();
}

void MainWindow::populateHeaps() {
    popularityHeap = MaxHeap(SortMode::POPULARITY);
    danceHeap = MaxHeap(SortMode::DANCEABILITY);
    energyHeap = MaxHeap(SortMode::ENERGY);
    durationHeap = MaxHeap(SortMode::DURATION);

    for (const auto &s : songs) {
        popularityHeap.insert(s);
        danceHeap.insert(s);
        energyHeap.insert(s);
        durationHeap.insert(s);
    }
}

const MaxHeap* MainWindow::activeHeap() const {
    switch (activeMode) {
    case SortMode::POPULARITY: return &popularityHeap;
    case SortMode::DANCEABILITY: return &danceHeap;
    case SortMode::ENERGY: return &energyHeap;
    case SortMode::DURATION: return &durationHeap;
    }
    return &popularityHeap;
}

void MainWindow::highlightButton(QPushButton* clickedButton) {
    QList<QPushButton*> buttons = { ui->btnPopularity, ui->btnDanceability, ui->btnEnergy, ui->btnDuration };
    for (auto b : buttons) {
        if (b == clickedButton)
            b->setStyleSheet("background-color: #555555; color: white;");
        else
            b->setStyleSheet("");
    }
}

void MainWindow::displaySongs(SortMode mode, bool reset) {
    activeMode = mode;
    const MaxHeap* heapPtr = activeHeap();
    if (!heapPtr || heapPtr->isEmpty()) {
        ui->tableSongs->clear();
        ui->tableSongs->setRowCount(0);
        return;
    }

    if (reset) currentDisplayCount = kInitialBatch;

    int totalAvailable = heapPtr->size();
    int toShow = std::min(currentDisplayCount, totalAvailable);
    if (toShow <= 0) {
        ui->tableSongs->clear();
        ui->tableSongs->setRowCount(0);
        ui->loadBtn->setVisible(totalAvailable > 0);
        return;
    }

    std::vector<Song> rows = heapPtr->peekTop(toShow);

    ui->tableSongs->clear();
    ui->tableSongs->setRowCount(static_cast<int>(rows.size()));
    ui->tableSongs->setColumnCount(10);
    ui->tableSongs->setHorizontalHeaderLabels(
        {"Rank", "Song Name", "Artist", "Popularity", "Danceability", "Energy",
         "Duration(ms)", "Votes", "Upvote", "Downvote"});

    // --- Button base style ---
    QString buttonStyle = R"(
        QPushButton {
            background-color: white;
            color: black;
            border: 1px solid #999;
            border-radius: 8px;
            min-width: 32px;
            min-height: 32px;
            max-width: 32px;
            max-height: 32px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #f0f0f0;
        }
    )";

    for (int i = 0; i < static_cast<int>(rows.size()); ++i) {
        const Song &s = rows[i];
        QString songName = QString::fromStdString(s.name);
        QString artistName = QString::fromStdString(s.artist);
        std::string key = s.name + " - " + s.artist;

        int displayVotes = s.votes;
        if (voteMemory[key] == "upvoted") displayVotes += 1;
        else if (voteMemory[key] == "downvoted") displayVotes -= 1;

        ui->tableSongs->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tableSongs->setItem(i, 1, new QTableWidgetItem(truncateText(songName, 30)));
        ui->tableSongs->setItem(i, 2, new QTableWidgetItem(truncateText(artistName, 30)));
        ui->tableSongs->setItem(i, 3, new QTableWidgetItem(QString::number(s.popularity)));
        ui->tableSongs->setItem(i, 4, new QTableWidgetItem(QString::number(s.danceability, 'f', 2)));
        ui->tableSongs->setItem(i, 5, new QTableWidgetItem(QString::number(s.energy, 'f', 2)));
        ui->tableSongs->setItem(i, 6, new QTableWidgetItem(QString::number(s.duration_ms)));
        ui->tableSongs->setItem(i, 7, new QTableWidgetItem(QString::number(displayVotes)));

        // --- Create Upvote and Downvote buttons ---
        QPushButton *upvoteBtn = new QPushButton("▲");
        QPushButton *downvoteBtn = new QPushButton("▼");

        upvoteBtn->setCursor(Qt::PointingHandCursor);
        downvoteBtn->setCursor(Qt::PointingHandCursor);
        upvoteBtn->setStyleSheet(buttonStyle);
        downvoteBtn->setStyleSheet(buttonStyle);

        // --- Restore previous vote memory ---
        if (voteMemory[key] == "upvoted") {
            upvoteBtn->setStyleSheet("background-color: #0f0; color: black; "
                                     "border-radius: 8px; max-width: 32px; max-height: 32px; box-shadow: 0 0 10px #0f0;");
            upvoteBtn->setEnabled(false);
            downvoteBtn->setEnabled(false);
        } else if (voteMemory[key] == "downvoted") {
            downvoteBtn->setStyleSheet("background-color: #f00; color: white; "
                                       "border-radius: 8px; max-width: 32px; max-height: 32px; box-shadow: 0 0 10px #f00;");
            upvoteBtn->setEnabled(false);
            downvoteBtn->setEnabled(false);
        }

        // --- Upvote click handler ---
        connect(upvoteBtn, &QPushButton::clicked, this, [this, i, key, upvoteBtn, downvoteBtn]() {
            QTableWidgetItem *voteItem = ui->tableSongs->item(i, 7);
            if (!voteItem) return;
            int currentVotes = voteItem->text().toInt();
            voteItem->setText(QString::number(currentVotes + 1));

            upvoteBtn->setStyleSheet("background-color: #0f0; color: black; "
                                     "border-radius: 8px; max-width: 32px; max-height: 32px; box-shadow: 0 0 10px #0f0;");
            upvoteBtn->setEnabled(false);
            downvoteBtn->setEnabled(false);
            voteMemory[key] = "upvoted";
        });

        // --- Downvote click handler ---
        connect(downvoteBtn, &QPushButton::clicked, this, [this, i, key, upvoteBtn, downvoteBtn]() {
            QTableWidgetItem *voteItem = ui->tableSongs->item(i, 7);
            if (!voteItem) return;
            int currentVotes = voteItem->text().toInt();
            voteItem->setText(QString::number(currentVotes - 1));

            downvoteBtn->setStyleSheet("background-color: #f00; color: white; "
                                       "border-radius: 8px; max-width: 32px; max-height: 32px; box-shadow: 0 0 10px #f00;");
            upvoteBtn->setEnabled(false);
            downvoteBtn->setEnabled(false);
            voteMemory[key] = "downvoted";
        });

        ui->tableSongs->setCellWidget(i, 8, upvoteBtn);
        ui->tableSongs->setCellWidget(i, 9, downvoteBtn);
    }

    // --- Table appearance ---
    ui->tableSongs->resizeColumnsToContents();
    ui->tableSongs->horizontalHeader()->setStretchLastSection(true);
    ui->tableSongs->verticalHeader()->setVisible(false);
    ui->tableSongs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableSongs->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableSongs->setFocusPolicy(Qt::NoFocus);

    ui->loadBtn->setVisible(currentDisplayCount < totalAvailable);
}




void MainWindow::on_loadBtn_clicked() {
    currentDisplayCount += kBatchIncrement;
    displaySongs(activeMode, false);
}

void MainWindow::handleUpvote(int songIndex) {
    songs[songIndex].upvote();
    displaySongs(activeMode, false); // refresh visible table only
}

void MainWindow::handleDownvote(int songIndex) {
    songs[songIndex].downvote();
    displaySongs(activeMode, false);
}

void MainWindow::on_btnPopularity_clicked() {
    Song::sortMode = SortMode::POPULARITY;
    highlightButton(ui->btnPopularity);
    displaySongs(SortMode::POPULARITY, true);
}

void MainWindow::on_btnDanceability_clicked() {
    Song::sortMode = SortMode::DANCEABILITY;
    highlightButton(ui->btnDanceability);
    displaySongs(SortMode::DANCEABILITY, true);
}

void MainWindow::on_btnEnergy_clicked() {
    Song::sortMode = SortMode::ENERGY;
    highlightButton(ui->btnEnergy);
    displaySongs(SortMode::ENERGY, true);
}

void MainWindow::on_btnDuration_clicked() {
    Song::sortMode = SortMode::DURATION;
    highlightButton(ui->btnDuration);
    displaySongs(SortMode::DURATION, true);
}
