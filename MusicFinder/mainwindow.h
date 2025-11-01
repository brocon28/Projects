#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Song.h"
#include "MaxHeap.h"
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnPopularity_clicked();
    void on_btnDanceability_clicked();
    void on_btnEnergy_clicked();
    void on_btnDuration_clicked();
    void on_loadBtn_clicked();

    // New slots for upvote/downvote buttons
    void handleUpvote(int songIndex);
    void handleDownvote(int songIndex);

private:
    Ui::MainWindow *ui;

    std::vector<Song> songs;
    std::unordered_map<std::string, std::string> voteMemory; //keep track of votes
    MaxHeap popularityHeap;
    MaxHeap danceHeap;
    MaxHeap energyHeap;
    MaxHeap durationHeap;

    SortMode activeMode = SortMode::POPULARITY;
    int currentDisplayCount = 0;
    const int kInitialBatch = 20;
    const int kBatchIncrement = 20;

    void loadKaggleData();
    void populateHeaps();

    void displaySongs(SortMode mode, bool reset = true);
    void highlightButton(QPushButton* clickedButton);
    QString truncateText(const QString& text, int maxLength = 30);
    const MaxHeap* activeHeap() const;

    // Helper to map visible table rows to global song indices
    std::vector<int> currentSongIndices;
};

#endif // MAINWINDOW_H
