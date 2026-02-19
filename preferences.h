
// Preferences functions

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QTabWidget>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QScreen>
#include <QGuiApplication>

void MainWindow::on_actionShowHideTabs_triggered() {
    // Open the comprehensive preferences dialog
    showPreferencesDialog();
}

void MainWindow::showPreferencesDialog() {
    // Create comprehensive preferences dialog with tabs
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Präferenzen");
    dialog->setMinimumSize(600, 500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    
    // Create tab widget for different preference categories
    QTabWidget *tabWidget = new QTabWidget(dialog);
    
    // Tab 1: General (Window size, auto-save)
    QWidget *generalTab = new QWidget();
    QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);
    
    QGroupBox *windowGroup = new QGroupBox("Fenstereinstellungen", generalTab);
    QVBoxLayout *windowLayout = new QVBoxLayout(windowGroup);
    
    QCheckBox *rememberSizeCheck = new QCheckBox("Fenstergröße und -position beachten", windowGroup);
    rememberSizeCheck->setChecked(loadWindowSizeEnabled());
    windowLayout->addWidget(rememberSizeCheck);
    
    QGroupBox *autoSaveGroup = new QGroupBox("Autom. Speichern/Laden", generalTab);
    QVBoxLayout *autoSaveLayout = new QVBoxLayout(autoSaveGroup);
    
    QCheckBox *autoSaveCheck = new QCheckBox("Einstellungen bei Änderung automatisch speichern", autoSaveGroup);
    autoSaveCheck->setChecked(loadAutoSaveEnabled());
    autoSaveLayout->addWidget(autoSaveCheck);
    
    generalLayout->addWidget(windowGroup);
    generalLayout->addWidget(autoSaveGroup);
    generalLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    
    tabWidget->addTab(generalTab, "Allgemein");
    
    // Tab 2: Tab Visibility (existing functionality)
    QWidget *tabsTab = new QWidget();
    QVBoxLayout *tabsLayout = new QVBoxLayout(tabsTab);
    
    QLabel *tabsLabel = new QLabel("Anzuzeigende Tabs auswählen:", tabsTab);
    tabsLayout->addWidget(tabsLabel);
    
    QMap<QString, QCheckBox*> checkboxes;
    QMap<QString, QString> tabNames;
    tabNames["driveToolsTab"] = "Laufwerk-Tools";
    tabNames["shellToolsTab"] = "Shell Tools";
    tabNames["servicesTab"] = "Dienste/Startup";
    tabNames["tweaksTab"] = "Tweaks";
    tabNames["kvmTab"] = "DUAL GPU KVM";
    tabNames["configTab"] = "Systemkonfiguration";
    tabNames["logsTab"] = "Systemprotokolle";
    tabNames["backupTab"] = "Sicherung";
    tabNames["restoreTab"] = "Wiederherstellen";
    tabNames["isoCreatorTab"] = "System ISO Creator";
    tabNames["packageManagerTab"] = "Paketmanager";
    tabNames["networkTab"] = "Netzwerk";
    
    for (auto it = tabNames.begin(); it != tabNames.end(); ++it) {
        QCheckBox *checkbox = new QCheckBox(it.value(), tabsTab);
        checkbox->setChecked(tabVisibilityMap.value(it.key(), true));
        checkbox->setProperty("tabName", it.key());
        checkboxes[it.key()] = checkbox;
        tabsLayout->addWidget(checkbox);
    }
    
    tabsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    tabWidget->addTab(tabsTab, "Tab-Sichtbarkeit");
    
    // Add more tabs for each main tab's preferences (placeholder for now)
    // Drive Tools Tab
    QWidget *driveToolsPrefTab = new QWidget();
    QVBoxLayout *driveToolsPrefLayout = new QVBoxLayout(driveToolsPrefTab);
    QLabel *driveToolsLabel = new QLabel("Drive Tools-Einstellungen werden hier hinzugefügt.", driveToolsPrefTab);
    driveToolsPrefLayout->addWidget(driveToolsLabel);
    driveToolsPrefLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    tabWidget->addTab(driveToolsPrefTab, "Laufwerk-Tools");
    
    // Shell Tools Tab
    QWidget *shellToolsPrefTab = new QWidget();
    QVBoxLayout *shellToolsPrefLayout = new QVBoxLayout(shellToolsPrefTab);
    QLabel *shellToolsLabel = new QLabel("Shell Tools-Einstellungen werden hier hinzugefügt", shellToolsPrefTab);
    shellToolsPrefLayout->addWidget(shellToolsLabel);
    shellToolsPrefLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    tabWidget->addTab(shellToolsPrefTab, "Shell Tools");
    
    // Package Manager Tab
    QWidget *packageManagerPrefTab = new QWidget();
    QVBoxLayout *packageManagerPrefLayout = new QVBoxLayout(packageManagerPrefTab);
    QLabel *packageManagerLabel = new QLabel("Einstellungen des Paketmanagers werden hier hinzugefügt.", packageManagerPrefTab);
    packageManagerPrefLayout->addWidget(packageManagerLabel);
    packageManagerPrefLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    tabWidget->addTab(packageManagerPrefTab, "Pakete-Manager");
    
    // Network Tab
    QWidget *networkPrefTab = new QWidget();
    QVBoxLayout *networkPrefLayout = new QVBoxLayout(networkPrefTab);
    QLabel *networkLabel = new QLabel("Netzwerkeinstellungen werden hier hinzugefügt.", networkPrefTab);
    networkPrefLayout->addWidget(networkLabel);
    networkPrefLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    tabWidget->addTab(networkPrefTab, "Netzwerk");
    
    mainLayout->addWidget(tabWidget);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", dialog);
    QPushButton *cancelButton = new QPushButton("Abbrechen", dialog);
    QPushButton *applyButton = new QPushButton("Anwenden", dialog);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(applyButton);
    mainLayout->addLayout(buttonLayout);
    
    // Save function
    auto savePreferences = [this, dialog, checkboxes, rememberSizeCheck, autoSaveCheck]() {
        // Save tab visibility
        for (auto it = checkboxes.begin(); it != checkboxes.end(); ++it) {
            QString tabName = it.key();
            QCheckBox *checkbox = it.value();
            if (checkbox) {
                bool visible = checkbox->isChecked();
                tabVisibilityMap[tabName] = visible;
            }
        }
        
        // Save window size preference
        saveWindowSizeEnabled(rememberSizeCheck->isChecked());
        
        // Save auto-save preference
        saveAutoSaveEnabled(autoSaveCheck->isChecked());
        
        // Apply tab visibility
        QTimer::singleShot(0, this, [this]() {
            applyTabVisibility();
        });
        
        // Save all preferences
        saveAllPreferences();
    };
    
    connect(okButton, &QPushButton::clicked, [savePreferences, dialog]() {
        savePreferences();
        dialog->accept();
    });
    
    connect(applyButton, &QPushButton::clicked, savePreferences);
    
    connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);
    
    // Auto-save on change if enabled
    if (loadAutoSaveEnabled()) {
        for (auto it = checkboxes.begin(); it != checkboxes.end(); ++it) {
            connect(it.value(), &QCheckBox::toggled, savePreferences);
        }
        connect(rememberSizeCheck, &QCheckBox::toggled, savePreferences);
        connect(autoSaveCheck, &QCheckBox::toggled, savePreferences);
    }
    
    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::on_actionAbout_triggered() {
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("About Arch OS Tools");
    
    QString aboutText = QString(
        "<h2>Arch OS Tools</h2>"
        "<p><b>Entwickelt von Xetal mit ❤️ für Arch Linux und CachyOS</b></p>"
        "<p>Eine bescheidene Sammlung von Systemverwaltungswerkzeugen, die dazu entwickelt wurden, "
        "dir dein Linux-Erlebnis reibungsloser und angenehmer zu gestalten.</p>"
        "<p>Diese Anwendung wurde mit Sorgfalt entwickelt, um dir bei der Verwaltung deines "
        "System mühelos, von der Laufwerksverwaltung bis zur Paketabwicklung, "
        "Systemoptimierungen und mehr zu helfen.</p>"
        "<p><i>Vielen Dank, dass du Arch S Tools verwendest!</i></p>"
        "<hr>"
        "<p style='color: #666; font-size: 10px;'>"
        "Entwickelt mit Qt, C++ und Liebe zur Linux-Community"
        "</p>"
    );
    
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText(aboutText);
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}

void MainWindow::loadTabVisibilityPreferences() {
    QSettings settings("CachyOsTools", "CachyOsTools");
    
    // Load visibility for each tab (default to true/visible)
    QStringList tabNames = {
        "driveToolsTab", "shellToolsTab", "servicesTab", "tweaksTab",
        "kvmTab", "configTab", "logsTab", "backupTab", "restoreTab",
        "isoCreatorTab", "packageManagerTab", "networkTab"
    };
    
    for (const QString &tabName : tabNames) {
        bool visible = settings.value(QString("tabs/%1").arg(tabName), true).toBool();
        tabVisibilityMap[tabName] = visible;
    }
    
    // Apply visibility after a short delay to ensure all widgets are initialized
    QTimer::singleShot(100, this, [this]() {
        applyTabVisibility();
    });
}

void MainWindow::saveTabVisibilityPreferences() {
    QSettings settings("CachyOsTools", "CachyOsTools");
    
    // Save visibility for each tab
    for (auto it = tabVisibilityMap.begin(); it != tabVisibilityMap.end(); ++it) {
        settings.setValue(QString("tabs/%1").arg(it.key()), it.value());
    }
    
    settings.sync();
}

void MainWindow::applyTabVisibility() {
    // Safety check - ensure UI is initialized
    if (!ui || !ui->tabWidget) {
        return;
    }
    
    // Apply visibility to each tab
    QMap<QString, QWidget*> tabs;
    tabs["driveToolsTab"] = ui->driveToolsTab;
    tabs["shellToolsTab"] = ui->shellToolsTab;
    tabs["servicesTab"] = ui->servicesTab;
    tabs["tweaksTab"] = ui->tweaksTab;
    tabs["kvmTab"] = ui->kvmTab;
    tabs["configTab"] = ui->configTab;
    tabs["logsTab"] = ui->logsTab;
    tabs["backupTab"] = ui->backupTab;
    tabs["restoreTab"] = ui->restoreTab;
    tabs["isoCreatorTab"] = ui->isoCreatorTab;
    tabs["packageManagerTab"] = ui->packageManagerTab;
    tabs["networkTab"] = ui->networkTab;
    
    // Verify all tabs exist before proceeding
    for (auto it = tabs.begin(); it != tabs.end(); ++it) {
        if (!it.value()) {
            qWarning() << "Tab widget not found:" << it.key();
            return; // Don't proceed if any tab is missing
        }
    }
    
    // Store current tab name (not widget pointer) to restore after visibility changes
    QString currentTabName;
    if (ui->tabWidget->count() > 0) {
        QWidget *currentWidget = ui->tabWidget->currentWidget();
        if (currentWidget) {
            for (auto it = tabs.begin(); it != tabs.end(); ++it) {
                if (it.value() == currentWidget) {
                    currentTabName = it.key();
                    break;
                }
            }
        }
    }
    
    // Define tab order and titles
    QMap<QString, QString> tabTitles;
    tabTitles["driveToolsTab"] = "Laufwerk";
    tabTitles["shellToolsTab"] = "Shell Tools";
    tabTitles["servicesTab"] = "Dienste/Startup";
    tabTitles["tweaksTab"] = "Tweaks";
    tabTitles["kvmTab"] = "DUAL GPU KVM";
    tabTitles["configTab"] = "Systemkonfiguration";
    tabTitles["logsTab"] = "Systemprotokolle";
    tabTitles["backupTab"] = "Sicherung";
    tabTitles["restoreTab"] = "Wiederherstellen";
    tabTitles["isoCreatorTab"] = "System ISO Creator";
    tabTitles["packageManagerTab"] = "Paketmanager";
    tabTitles["networkTab"] = "Netzwerk";
    
    QStringList tabOrder = {
        "driveToolsTab", "shellToolsTab", "servicesTab", "tweaksTab",
        "kvmTab", "configTab", "logsTab", "backupTab", "restoreTab",
        "isoCreatorTab", "packageManagerTab", "networkTab"
    };
    
    // Store all widgets before removing tabs to prevent deletion
    // Qt will delete widgets when tabs are removed unless we keep references
    QList<QWidget*> widgetsToKeep;
    int tabCount = ui->tabWidget->count();
    for (int i = 0; i < tabCount; i++) {
        QWidget *widget = ui->tabWidget->widget(i);
        if (widget) {
            // Reparent widgets to main window temporarily to prevent deletion
            widget->setParent(this);
            widgetsToKeep.append(widget);
        }
    }
    
    // Remove all tabs (widgets are kept alive because they're reparented)
    ui->tabWidget->blockSignals(true);
    while (ui->tabWidget->count() > 0) {
        ui->tabWidget->removeTab(0);
    }
    ui->tabWidget->blockSignals(false);
    
    // Re-add tabs in order, only if visible
    for (const QString &tabName : tabOrder) {
        if (tabVisibilityMap.value(tabName, true)) {
            QWidget *tab = tabs.value(tabName);
            if (tab) {
                QString title = tabTitles.value(tabName);
                // Reparent widget back to tab widget
                tab->setParent(ui->tabWidget);
                ui->tabWidget->addTab(tab, title);
            }
        }
    }
    
    // Restore current tab if it's still visible
    if (!currentTabName.isEmpty() && tabVisibilityMap.value(currentTabName, true)) {
        QWidget *tabToSelect = tabs.value(currentTabName);
        if (tabToSelect) {
            int index = ui->tabWidget->indexOf(tabToSelect);
            if (index >= 0 && index < ui->tabWidget->count()) {
                ui->tabWidget->setCurrentIndex(index);
                return; // Successfully restored
            }
        }
    }
    
    // Current tab was hidden or not found, switch to first visible tab
    if (ui->tabWidget->count() > 0) {
        ui->tabWidget->setCurrentIndex(0);
    }
}

void MainWindow::saveAllPreferences() {
    QSettings settings("CachyOsTools", "CachyOsTools");
    
    // Save tab visibility
    saveTabVisibilityPreferences();
    
    // Save window geometry if enabled
    if (loadWindowSizeEnabled()) {
        saveWindowGeometry();
    }
    
    settings.sync();
}

void MainWindow::loadAllPreferences() {
    loadTabVisibilityPreferences();
    if (loadWindowSizeEnabled()) {
        loadWindowGeometry();
    }
}

bool MainWindow::loadWindowSizeEnabled() {
    QSettings settings("CachyOsTools", "CachyOsTools");
    return settings.value("preferences/windowSizeEnabled", true).toBool();
}

void MainWindow::saveWindowSizeEnabled(bool enabled) {
    QSettings settings("CachyOsTools", "CachyOsTools");
    settings.setValue("preferences/windowSizeEnabled", enabled);
    settings.sync();
}

bool MainWindow::loadAutoSaveEnabled() {
    QSettings settings("CachyOsTools", "CachyOsTools");
    return settings.value("preferences/autoSaveEnabled", true).toBool();
}

void MainWindow::saveAutoSaveEnabled(bool enabled) {
    QSettings settings("CachyOsTools", "CachyOsTools");
    settings.setValue("preferences/autoSaveEnabled", enabled);
    settings.sync();
}

void MainWindow::saveWindowGeometry() {
    QSettings settings("CachyOsTools", "CachyOsTools");
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
    settings.sync();
}

void MainWindow::loadWindowGeometry() {
    QSettings settings("CachyOsTools", "CachyOsTools");
    QByteArray geometry = settings.value("window/geometry").toByteArray();
    QByteArray state = settings.value("window/state").toByteArray();
    
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    if (!state.isEmpty()) {
        restoreState(state);
    }
    
    // Clear any minimum size constraints that might have been restored
    setMinimumSize(0, 0);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (loadWindowSizeEnabled()) {
        saveWindowGeometry();
    }
    saveAllPreferences();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (loadWindowSizeEnabled() && loadAutoSaveEnabled()) {
        // Debounce saves to avoid too many writes
        static QTimer *saveTimer = nullptr;
        if (!saveTimer) {
            saveTimer = new QTimer(this);
            saveTimer->setSingleShot(true);
            saveTimer->setInterval(500); // Wait 500ms after last resize
            connect(saveTimer, &QTimer::timeout, this, &MainWindow::saveWindowGeometry);
        }
        saveTimer->start();
    }
}

void MainWindow::moveEvent(QMoveEvent *event) {
    QMainWindow::moveEvent(event);
    if (loadWindowSizeEnabled() && loadAutoSaveEnabled()) {
        // Debounce saves to avoid too many writes
        static QTimer *saveTimer = nullptr;
        if (!saveTimer) {
            saveTimer = new QTimer(this);
            saveTimer->setSingleShot(true);
            saveTimer->setInterval(500); // Wait 500ms after last move
            connect(saveTimer, &QTimer::timeout, this, &MainWindow::saveWindowGeometry);
        }
        saveTimer->start();
    }
}
