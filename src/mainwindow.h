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
#include <QtGui/QIcon>
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
    void copySnippets();
    void saveSnippets();
    void loadSnippets();
    void onYamlFileChanged();
    void openMatchDirectory();
    void createNewYamlFile();
    void deleteCurrentFile();
    void updateButtonStates();

private:
    void setupUI();
    void createConnections();
    void setupKeyboardShortcuts();
    QString getEspansoConfigPath() const;
    QString getEspansoMatchDirectory() const;
    void scanYamlFiles();
    bool validateYaml(const QString &content) const;
    bool validateYamlContent(const QString &content) const;
    bool needsQuotes(const QString &text) const;
    QString escapeForYaml(const QString &text) const;
    void updateYamlStatusIcon();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    QTableWidget *snippetTable;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *copyButton;
    QPushButton *saveButton;
    QPushButton *reloadButton;
    QComboBox *yamlFileComboBox;
    QPushButton *openDirButton;
    QPushButton *newFileButton;
    QPushButton *deleteFileButton;
    QLabel *yamlStatusLabel;

    QString configPath;
    QString matchDirectory;
    // YAML::Node config;  // Commented out for now
};

#endif // MAINWINDOW_H 