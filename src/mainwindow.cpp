#include "mainwindow.h"
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
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtWidgets/QFileDialog>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QInputDialog>
#include <QHeaderView>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <algorithm>
#include <QRegularExpression>
#include <QTextStream>
#include <QSet>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    configPath = getEspansoConfigPath();
    matchDirectory = getEspansoMatchDirectory();
    setupUI();
    createConnections();
    scanYamlFiles();
    if (yamlFileComboBox->count() > 0) {
        loadSnippets();
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("Espanso Helper");
    setGeometry(100, 100, 800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // YAML file selection
    QHBoxLayout *yamlLayout = new QHBoxLayout();
    yamlLayout->addWidget(new QLabel("YAML File:"));
    yamlFileComboBox = new QComboBox();
    yamlFileComboBox->setMinimumWidth(200);
    yamlLayout->addWidget(yamlFileComboBox);
    openDirButton = new QPushButton("Open Directory");
    yamlLayout->addWidget(openDirButton);
    newFileButton = new QPushButton("New File");
    yamlLayout->addWidget(newFileButton);
    yamlStatusLabel = new QLabel("✓");
    yamlStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    yamlLayout->addWidget(yamlStatusLabel);
    yamlLayout->addStretch();
    mainLayout->addLayout(yamlLayout);

    // Table
    snippetTable = new QTableWidget();
    snippetTable->setColumnCount(3);
    snippetTable->setHorizontalHeaderLabels({"Trigger", "Replace", "Description"});
    snippetTable->horizontalHeader()->setStretchLastSection(true);
    snippetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    snippetTable->setSelectionMode(QAbstractItemView::SingleSelection);
    snippetTable->setAlternatingRowColors(true);
    mainLayout->addWidget(snippetTable);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Add");
    editButton = new QPushButton("Edit");
    deleteButton = new QPushButton("Delete");
    saveButton = new QPushButton("Save");
    reloadButton = new QPushButton("Reload");

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(reloadButton);
    mainLayout->addLayout(buttonLayout);
}

void MainWindow::createConnections()
{
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addNewSnippet);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editSnippet);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSnippet);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveSnippets);
    connect(reloadButton, &QPushButton::clicked, this, &MainWindow::loadSnippets);
    connect(yamlFileComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &MainWindow::onYamlFileChanged);
    connect(openDirButton, &QPushButton::clicked, this, &MainWindow::openMatchDirectory);
    connect(newFileButton, &QPushButton::clicked, this, &MainWindow::createNewYamlFile);
}

QString MainWindow::getEspansoConfigPath() const
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString espansoPath = configPath + "/espanso";
    qDebug() << "Config location:" << configPath;
    qDebug() << "Espanso path:" << espansoPath;
    return espansoPath;
}

QString MainWindow::getEspansoMatchDirectory() const
{
    QString matchPath = configPath + "/match";
    qDebug() << "Match directory:" << matchPath;
    return matchPath;
}

void MainWindow::scanYamlFiles()
{
    yamlFileComboBox->clear();
    QDir matchDir(matchDirectory);
    qDebug() << "Scanning directory:" << matchDirectory;
    qDebug() << "Directory exists:" << matchDir.exists();
    if (matchDir.exists()) {
        QStringList filters;
        filters << "*.yml" << "*.yaml";
        QStringList files = matchDir.entryList(filters, QDir::Files);
        qDebug() << "Found files:" << files;
        yamlFileComboBox->addItems(files);
    } else {
        qDebug() << "Directory does not exist:" << matchDirectory;
        // Try alternative paths
        QStringList alternativePaths;
        alternativePaths << QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/espanso/match";
        alternativePaths << QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/AppData/Roaming/espanso/match";
        alternativePaths << "C:/Users/" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation).split("/").last() + "/AppData/Roaming/espanso/match";
        
        for (const QString &altPath : alternativePaths) {
            QDir altDir(altPath);
            qDebug() << "Trying alternative path:" << altPath;
            if (altDir.exists()) {
                qDebug() << "Found alternative directory:" << altPath;
                matchDirectory = altPath;
                QStringList filters;
                filters << "*.yml" << "*.yaml";
                QStringList files = altDir.entryList(filters, QDir::Files);
                qDebug() << "Found files in alternative:" << files;
                yamlFileComboBox->addItems(files);
                break;
            }
        }
    }
}

void MainWindow::addNewSnippet()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Snippet");
    dialog.setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *triggerLabel = new QLabel("Trigger:");
    QLineEdit *triggerEdit = new QLineEdit();
    layout->addWidget(triggerLabel);
    layout->addWidget(triggerEdit);

    QLabel *replaceLabel = new QLabel("Replace:");
    QTextEdit *replaceEdit = new QTextEdit();
    replaceEdit->setMaximumHeight(100);
    layout->addWidget(replaceLabel);
    layout->addWidget(replaceEdit);

    QLabel *descLabel = new QLabel("Description:");
    QLineEdit *descEdit = new QLineEdit();
    layout->addWidget(descLabel);
    layout->addWidget(descEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString trigger = triggerEdit->text().trimmed();
        QString replace = replaceEdit->toPlainText().trimmed();
        QString description = descEdit->text().trimmed();

        if (trigger.isEmpty()) {
            QMessageBox::warning(this, "Error", "Trigger cannot be empty!");
            return;
        }

        if (replace.isEmpty()) {
            QMessageBox::warning(this, "Error", "Replace text cannot be empty!");
            return;
        }

        // Check for duplicate triggers
        for (int i = 0; i < snippetTable->rowCount(); ++i) {
            if (snippetTable->item(i, 0)->text() == trigger) {
                QMessageBox::warning(this, "Error", "A snippet with this trigger already exists!");
                return;
            }
        }

        int row = snippetTable->rowCount();
        snippetTable->insertRow(row);
        snippetTable->setItem(row, 0, new QTableWidgetItem(trigger));
        snippetTable->setItem(row, 1, new QTableWidgetItem(replace));
        snippetTable->setItem(row, 2, new QTableWidgetItem(description));

        updateYamlStatusIcon();
    }
}

void MainWindow::editSnippet()
{
    int currentRow = snippetTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please select a snippet to edit!");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Edit Snippet");
    dialog.setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *triggerLabel = new QLabel("Trigger:");
    QLineEdit *triggerEdit = new QLineEdit(snippetTable->item(currentRow, 0)->text());
    layout->addWidget(triggerLabel);
    layout->addWidget(triggerEdit);

    QLabel *replaceLabel = new QLabel("Replace:");
    QTextEdit *replaceEdit = new QTextEdit();
    replaceEdit->setText(snippetTable->item(currentRow, 1)->text());
    replaceEdit->setMaximumHeight(100);
    layout->addWidget(replaceLabel);
    layout->addWidget(replaceEdit);

    QLabel *descLabel = new QLabel("Description:");
    QLineEdit *descEdit = new QLineEdit(snippetTable->item(currentRow, 2)->text());
    layout->addWidget(descLabel);
    layout->addWidget(descEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString trigger = triggerEdit->text().trimmed();
        QString replace = replaceEdit->toPlainText().trimmed();
        QString description = descEdit->text().trimmed();

        if (trigger.isEmpty()) {
            QMessageBox::warning(this, "Error", "Trigger cannot be empty!");
            return;
        }

        if (replace.isEmpty()) {
            QMessageBox::warning(this, "Error", "Replace text cannot be empty!");
            return;
        }

        // Check for duplicate triggers (excluding current row)
        for (int i = 0; i < snippetTable->rowCount(); ++i) {
            if (i != currentRow && snippetTable->item(i, 0)->text() == trigger) {
                QMessageBox::warning(this, "Error", "A snippet with this trigger already exists!");
                return;
            }
        }

        snippetTable->setItem(currentRow, 0, new QTableWidgetItem(trigger));
        snippetTable->setItem(currentRow, 1, new QTableWidgetItem(replace));
        snippetTable->setItem(currentRow, 2, new QTableWidgetItem(description));

        updateYamlStatusIcon();
    }
}

void MainWindow::deleteSnippet()
{
    int currentRow = snippetTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please select a snippet to delete!");
        return;
    }

    QString trigger = snippetTable->item(currentRow, 0)->text();
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete the snippet with trigger '%1'?").arg(trigger),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        snippetTable->removeRow(currentRow);
        updateYamlStatusIcon();
    }
}

void MainWindow::saveSnippets()
{
    if (yamlFileComboBox->currentText().isEmpty()) {
        QMessageBox::warning(this, "Error", "No YAML file selected!");
        return;
    }

    QString filePath = matchDirectory + "/" + yamlFileComboBox->currentText();
    
    // Generate YAML content
    QString yamlContent = "# Espanso snippets\n\nmatches:\n";
    
    for (int i = 0; i < snippetTable->rowCount(); ++i) {
        QString trigger = snippetTable->item(i, 0)->text();
        QString replace = snippetTable->item(i, 1)->text();
        QString description = snippetTable->item(i, 2)->text();

        yamlContent += "  - trigger: " + (needsQuotes(trigger) ? "\"" + trigger + "\"" : trigger) + "\n";
        
        // Handle multiline replace content
        if (replace.contains("\n")) {
            yamlContent += "    replace: |\n";
            QStringList lines = replace.split("\n");
            for (const QString &line : lines) {
                yamlContent += "      " + line + "\n";
            }
        } else {
            yamlContent += "    replace: " + (needsQuotes(replace) ? "\"" + replace + "\"" : replace) + "\n";
        }
        
        if (!description.isEmpty()) {
            yamlContent += "    vars:\n";
            yamlContent += "      - name: description\n";
            yamlContent += "        type: string\n";
            yamlContent += "        params:\n";
            yamlContent += "          value: " + (needsQuotes(description) ? "\"" + description + "\"" : description) + "\n";
        }
        yamlContent += "\n";
    }

    // Write to file directly
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << yamlContent;
        file.close();
        
        QMessageBox::information(this, "Success", "Snippets saved successfully!");
        updateYamlStatusIcon();
    } else {
        QMessageBox::critical(this, "Error", "Could not save file: " + file.errorString());
    }
}

void MainWindow::loadSnippets()
{
    if (yamlFileComboBox->currentText().isEmpty()) {
        qDebug() << "No YAML file selected";
        return;
    }

    QString filePath = matchDirectory + "/" + yamlFileComboBox->currentText();
    qDebug() << "Loading file:" << filePath;
    QFile file(filePath);
    
    if (!file.exists()) {
        QMessageBox::warning(this, "Error", "Selected file does not exist!");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file: " + file.errorString());
        return;
    }

    QString content = file.readAll();
    file.close();

    // Clear current table
    snippetTable->setRowCount(0);

    // Only parse under 'matches:'
    QStringList lines = content.split('\n');
    int matchesIndex = -1;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].trimmed() == "matches:") {
            matchesIndex = i;
            break;
        }
    }
    if (matchesIndex == -1) {
        updateYamlStatusIcon();
        return;
    }
    // Parse only lines after 'matches:'
    QStringList matchLines;
    for (int i = matchesIndex + 1; i < lines.size(); ++i) {
        matchLines << lines[i];
    }

    QString currentTrigger, currentReplace, currentDescription;
    bool inVars = false;
    bool inParams = false;
    bool inReplaceBlock = false;

    for (int i = 0; i < matchLines.size(); ++i) {
        QString line = matchLines[i];
        QString trimmedLine = line.trimmed();
        if (trimmedLine.startsWith("- trigger:")) {
            // Save previous snippet if exists
            if (!currentTrigger.isEmpty() && !currentReplace.isEmpty()) {
                int row = snippetTable->rowCount();
                snippetTable->insertRow(row);
                snippetTable->setItem(row, 0, new QTableWidgetItem(currentTrigger));
                snippetTable->setItem(row, 1, new QTableWidgetItem(currentReplace));
                snippetTable->setItem(row, 2, new QTableWidgetItem(currentDescription));
            }
            // Start new snippet
            currentTrigger = trimmedLine.mid(10).trimmed();
            if (currentTrigger.startsWith('"') && currentTrigger.endsWith('"')) {
                currentTrigger = currentTrigger.mid(1, currentTrigger.length() - 2);
            }
            currentReplace.clear();
            currentDescription.clear();
            inVars = false;
            inParams = false;
            inReplaceBlock = false;
        }
        else if (trimmedLine.startsWith("replace: |")) {
            inReplaceBlock = true;
            currentReplace.clear();
        }
        else if (inReplaceBlock && line.startsWith("      ")) {
            currentReplace += (currentReplace.isEmpty() ? "" : "\n") + line.mid(6);
        }
        else if (inReplaceBlock && !line.startsWith("      ")) {
            inReplaceBlock = false;
        }
        else if (trimmedLine.startsWith("replace:")) {
            currentReplace = trimmedLine.mid(8).trimmed();
            if (currentReplace.startsWith('"') && currentReplace.endsWith('"')) {
                currentReplace = currentReplace.mid(1, currentReplace.length() - 2);
            }
        }
        else if (trimmedLine.startsWith("vars:")) {
            inVars = true;
        }
        else if (inVars && trimmedLine.startsWith("- name: description")) {
            inParams = true;
        }
        else if (inParams && trimmedLine.startsWith("value:")) {
            currentDescription = trimmedLine.mid(6).trimmed();
            if (currentDescription.startsWith('"') && currentDescription.endsWith('"')) {
                currentDescription = currentDescription.mid(1, currentDescription.length() - 2);
            }
            inVars = false;
            inParams = false;
        }
    }
    // Add last snippet
    if (!currentTrigger.isEmpty() && !currentReplace.isEmpty()) {
        int row = snippetTable->rowCount();
        snippetTable->insertRow(row);
        snippetTable->setItem(row, 0, new QTableWidgetItem(currentTrigger));
        snippetTable->setItem(row, 1, new QTableWidgetItem(currentReplace));
        snippetTable->setItem(row, 2, new QTableWidgetItem(currentDescription));
    }
    updateYamlStatusIcon();
}

void MainWindow::onYamlFileChanged()
{
    loadSnippets();
}

void MainWindow::openMatchDirectory()
{
    qDebug() << "Opening directory:" << matchDirectory;
    QDir dir(matchDirectory);
    if (!dir.exists()) {
        QMessageBox::warning(this, "Error", "Match directory does not exist: " + matchDirectory);
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(matchDirectory));
}

void MainWindow::createNewYamlFile()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, "New YAML File", 
                                           "Enter file name (without extension):", 
                                           QLineEdit::Normal, "", &ok);
    if (ok && !fileName.isEmpty()) {
        QString fullPath = matchDirectory + "/" + fileName + ".yml";
        QFile file(fullPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("# Espanso snippets\n");
            file.close();
            scanYamlFiles();
            yamlFileComboBox->setCurrentText(fileName + ".yml");
        }
    }
}

bool MainWindow::needsQuotes(const QString &text) const
{
    // Check if text needs quotes in YAML
    return text.contains(' ') || text.contains(':') || text.contains('"') || 
           text.contains('\'') || text.contains('[') || text.contains(']') ||
           text.contains('{') || text.contains('}') || text.contains(',') ||
           text.contains('&') || text.contains('*') || text.contains('#') ||
           text.contains('|') || text.contains('>') || text.contains('!') ||
           text.contains('@') || text.contains('%') || text.contains('`');
}

bool MainWindow::hasDuplicateTriggers() const
{
    QSet<QString> triggers;
    for (int i = 0; i < snippetTable->rowCount(); ++i) {
        QString trigger = snippetTable->item(i, 0)->text();
        if (triggers.contains(trigger)) {
            return true;
        }
        triggers.insert(trigger);
    }
    return false;
}

void MainWindow::updateYamlStatusIcon()
{
    if (hasDuplicateTriggers()) {
        yamlStatusLabel->setText("⚠");
        yamlStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    } else {
        yamlStatusLabel->setText("✓");
        yamlStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    }
}