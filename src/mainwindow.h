#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>
// #include <yaml-cpp/yaml.h>  // Commented out for now

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addNewSnippet();
    void editSnippet();
    void deleteSnippet();
    void saveSnippets();
    void loadSnippets();
    void onYamlFileChanged();
    void openMatchDirectory();
    void createNewYamlFile();

private:
    void setupUI();
    void createConnections();
    QString getEspansoConfigPath() const;
    QString getEspansoMatchDirectory() const;
    void scanYamlFiles();
    bool validateYaml(const QString &content) const;
    bool validateYamlContent(const QString &content) const;
    bool needsQuotes(const QString &text) const;
    bool hasDuplicateTriggers() const;
    void updateYamlStatusIcon();

    QTableWidget *snippetTable;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QPushButton *reloadButton;
    QComboBox *yamlFileComboBox;
    QPushButton *openDirButton;
    QPushButton *newFileButton;
    QLabel *yamlStatusLabel;

    QString configPath;
    QString matchDirectory;
    // YAML::Node config;  // Commented out for now
};

#endif // MAINWINDOW_H 