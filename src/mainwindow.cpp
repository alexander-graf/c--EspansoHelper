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
    setGeometry(100, 100, 900, 700);

    // Enable modern Windows styling
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
        }
        QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QTableWidget {
            background-color: #3c3c3c;
            alternate-background-color: #404040;
            gridline-color: #555555;
            border: 1px solid #555555;
            border-radius: 4px;
            selection-background-color: #0078d4;
            selection-color: #ffffff;
        }
        QTableWidget::item {
            padding: 4px;
            border-bottom: 1px solid #555555;
        }
        QTableWidget::item:selected {
            background-color: #0078d4;
            color: #ffffff;
        }
        QHeaderView::section {
            background-color: #404040;
            color: #ffffff;
            padding: 8px;
            border: none;
            border-bottom: 2px solid #0078d4;
            font-weight: bold;
        }
        QPushButton {
            background-color: #0078d4;
            color: #ffffff;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
            min-height: 20px;
        }
        QPushButton:hover {
            background-color: #106ebe;
        }
        QPushButton:pressed {
            background-color: #005a9e;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #888888;
        }
        QPushButton#deleteButton {
            background-color: #d83b01;
        }
        QPushButton#deleteButton:hover {
            background-color: #ea5a0e;
        }
        QPushButton#deleteFileButton {
            background-color: #d13438;
        }
        QPushButton#deleteFileButton:hover {
            background-color: #e81123;
        }
        QComboBox {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 6px 12px;
            min-height: 20px;
        }
        QComboBox:hover {
            border-color: #0078d4;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #ffffff;
            margin-right: 5px;
        }
        QComboBox QAbstractItemView {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
            selection-background-color: #0078d4;
        }
        QLabel {
            color: #ffffff;
            font-weight: bold;
        }
        QDialog {
            background-color: #2b2b2b;
        }
        QDialog QLabel {
            color: #ffffff;
        }
        QDialog QLineEdit, QDialog QTextEdit {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 6px;
        }
        QDialog QLineEdit:focus, QDialog QTextEdit:focus {
            border-color: #0078d4;
        }
        QToolTip {
            background-color: #2b2b2b;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 4px;
        }
    )");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // YAML file selection
    QHBoxLayout *yamlLayout = new QHBoxLayout();
    yamlLayout->setSpacing(10);
    
    QLabel *yamlLabel = new QLabel("YAML File:");
    yamlLabel->setMinimumWidth(80);
    yamlLayout->addWidget(yamlLabel);
    
    yamlFileComboBox = new QComboBox();
    yamlFileComboBox->setMinimumWidth(250);
    yamlFileComboBox->setToolTip("Select a YAML file to edit");
    yamlLayout->addWidget(yamlFileComboBox);
    
    openDirButton = new QPushButton("Open Directory");
    openDirButton->setMinimumWidth(120);
    openDirButton->setToolTip("Open the espanso match directory in Explorer");
    yamlLayout->addWidget(openDirButton);
    
    newFileButton = new QPushButton("New File");
    newFileButton->setMinimumWidth(100);
    newFileButton->setToolTip("Create a new YAML file");
    yamlLayout->addWidget(newFileButton);
    
    deleteFileButton = new QPushButton("Delete File");
    deleteFileButton->setObjectName("deleteFileButton");
    deleteFileButton->setMinimumWidth(100);
    deleteFileButton->setToolTip("Delete the currently selected YAML file");
    yamlLayout->addWidget(deleteFileButton);
    
    yamlStatusLabel = new QLabel("✓");
    yamlStatusLabel->setStyleSheet("color: #00ff00; font-weight: bold; font-size: 18px; background-color: #1e1e1e; border-radius: 12px; padding: 4px;");
    yamlStatusLabel->setMinimumWidth(40);
    yamlStatusLabel->setAlignment(Qt::AlignCenter);
    yamlStatusLabel->setToolTip("File status: ✓ = valid, ⚠ = needs attention");
    yamlLayout->addWidget(yamlStatusLabel);
    
    yamlLayout->addStretch();
    mainLayout->addLayout(yamlLayout);

    // Table
    snippetTable = new QTableWidget();
    snippetTable->setColumnCount(3);
    snippetTable->setHorizontalHeaderLabels({"Trigger", "Replace", "Description"});
    snippetTable->horizontalHeader()->setStretchLastSection(true);
    snippetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    snippetTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    snippetTable->setAlternatingRowColors(true);
    snippetTable->setGridStyle(Qt::SolidLine);
    snippetTable->setShowGrid(true);
    snippetTable->setFrameStyle(QFrame::StyledPanel);
    snippetTable->setToolTip("Snippets in the selected YAML file. Use Ctrl+Click or Shift+Click for multiple selection.");
    
    // Set column widths
    snippetTable->setColumnWidth(0, 150);  // Trigger
    snippetTable->setColumnWidth(1, 300);  // Replace
    
    mainLayout->addWidget(snippetTable);

    // Buttons - all side by side
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    addButton = new QPushButton("Add");
    addButton->setMinimumWidth(80);
    addButton->setToolTip("Add a new snippet (Ctrl+N)");
    buttonLayout->addWidget(addButton);
    
    editButton = new QPushButton("Edit");
    editButton->setMinimumWidth(80);
    editButton->setToolTip("Edit selected snippet (F2)");
    buttonLayout->addWidget(editButton);
    
    deleteButton = new QPushButton("Delete");
    deleteButton->setObjectName("deleteButton");
    deleteButton->setMinimumWidth(80);
    deleteButton->setToolTip("Delete selected snippets (Del)");
    buttonLayout->addWidget(deleteButton);
    
    buttonLayout->addStretch();
    
    saveButton = new QPushButton("Save");
    saveButton->setMinimumWidth(80);
    saveButton->setToolTip("Save changes to file (Ctrl+S)");
    buttonLayout->addWidget(saveButton);
    
    reloadButton = new QPushButton("Reload");
    reloadButton->setMinimumWidth(80);
    reloadButton->setToolTip("Reload file and refresh file list (F5)");
    buttonLayout->addWidget(reloadButton);
    
    mainLayout->addLayout(buttonLayout);
}

void MainWindow::createConnections()
{
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addNewSnippet);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editSnippet);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSnippet);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveSnippets);
    connect(reloadButton, &QPushButton::clicked, this, [this]() {
        QString currentFile = yamlFileComboBox->currentText();
        scanYamlFiles();
        // Restore the previously selected file
        int index = yamlFileComboBox->findText(currentFile);
        if (index >= 0) {
            yamlFileComboBox->setCurrentIndex(index);
        }
        loadSnippets();
    });
    connect(yamlFileComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &MainWindow::onYamlFileChanged);
    connect(openDirButton, &QPushButton::clicked, this, &MainWindow::openMatchDirectory);
    connect(newFileButton, &QPushButton::clicked, this, &MainWindow::createNewYamlFile);
    connect(deleteFileButton, &QPushButton::clicked, this, &MainWindow::deleteCurrentFile);
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
    QList<QTableWidgetItem*> selectedItems = snippetTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select snippets to delete!");
        return;
    }

    // Get unique rows (items can be from different columns of the same row)
    QSet<int> rowsToDelete;
    for (QTableWidgetItem* item : selectedItems) {
        rowsToDelete.insert(item->row());
    }

    if (rowsToDelete.isEmpty()) {
        return;
    }

    QString message;
    if (rowsToDelete.size() == 1) {
        int row = *rowsToDelete.begin();
        QString trigger = snippetTable->item(row, 0)->text();
        message = QString("Are you sure you want to delete the snippet with trigger '%1'?").arg(trigger);
    } else {
        message = QString("Are you sure you want to delete %1 selected snippets?").arg(rowsToDelete.size());
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", 
        message, QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Delete rows in reverse order to avoid index shifting
        QList<int> sortedRows = rowsToDelete.values();
        std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
        
        for (int row : sortedRows) {
            snippetTable->removeRow(row);
        }
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

    // Check for matches:
    QStringList lines = content.split('\n');
    int matchesIndex = -1;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].trimmed() == "matches:") {
            matchesIndex = i;
            break;
        }
    }
    if (matchesIndex == -1) {
        // Auto-repair: wrap all found snippets under matches:
        QString repaired = "# Espanso snippets\n\nmatches:\n";
        bool inSnippet = false;
        for (const QString &line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.startsWith("- trigger:")) {
                repaired += "  " + line.trimmed() + "\n";
                inSnippet = true;
            } else if (inSnippet && !trimmed.isEmpty()) {
                repaired += "    " + line.trimmed() + "\n";
            } else if (trimmed.isEmpty()) {
                repaired += "\n";
                inSnippet = false;
            }
        }
        // Save repaired file
        QFile repairFile(filePath);
        if (repairFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&repairFile);
            stream << repaired;
            repairFile.close();
        }
        // Reload with repaired file
        loadSnippets();
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
        // Validate filename - only letters, numbers, and underscores allowed
        QRegularExpression validName("^[a-zA-Z0-9_]+$");
        if (!validName.match(fileName).hasMatch()) {
            QMessageBox::warning(this, "Invalid Filename", 
                               "Filename can only contain letters, numbers, and underscores!");
            return;
        }
        
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
    // Check for matches: only, and file must not be empty
    bool hasMatches = false;
    if (yamlFileComboBox->currentText().isEmpty()) {
        qDebug() << "No file selected";
        yamlStatusLabel->setText("⚠");
        yamlStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
        return;
    }
    QString filePath = matchDirectory + "/" + yamlFileComboBox->currentText();
    qDebug() << "Checking file for matches:" << filePath;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = file.readAll();
        file.close();
        hasMatches = !content.trimmed().isEmpty() && content.contains("matches:");
        qDebug() << "File contains 'matches:':" << hasMatches;
        qDebug() << "File content preview:" << content.left(200);
    } else {
        qDebug() << "Could not open file for status check";
    }
    if (hasMatches) {
        qDebug() << "Setting green checkmark";
        yamlStatusLabel->setText("✓");
        yamlStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        qDebug() << "Setting orange warning";
        yamlStatusLabel->setText("⚠");
        yamlStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    }
}

void MainWindow::deleteCurrentFile()
{
    if (yamlFileComboBox->currentText().isEmpty()) {
        QMessageBox::warning(this, "Error", "No file selected!");
        return;
    }

    QString fileName = yamlFileComboBox->currentText();
    QString filePath = matchDirectory + "/" + fileName;
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete the file '%1'?\n\nThis action cannot be undone!").arg(fileName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QFile file(filePath);
        if (file.remove()) {
            QMessageBox::information(this, "Success", "File deleted successfully!");
            scanYamlFiles();
            loadSnippets();
        } else {
            QMessageBox::critical(this, "Error", "Could not delete file: " + file.errorString());
        }
    }
}