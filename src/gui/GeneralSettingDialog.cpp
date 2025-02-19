#include <QApplication>
#include <QSettings>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>
#include <qstandardpaths.h>
#include <QFileDialog>
#include <QMessageBox>
#include "MainWindow.h"
#include "gui/GeneralSettingDialog.h"
#include "util/NetworkUtil.h"

namespace {

static const int kLanguageTypeCount = 4;
static const int kTimeFormatTypeCount = 6;
static const int kEasingTypeCount = 12;
static const int kRangeTypeCount = 3;

int languageToIndex(const QString& aLanguage) {
    if (aLanguage == "Auto")
        return 0;
    else if (aLanguage == "English")
        return 1;
    else if (aLanguage == "Japanese")
        return 2;
    else if (aLanguage == "Chinese")
        return 3;
    else
        return 0;
}

int easingToIndex(const QString& aEasing) {
    if (aEasing == "None")
        return 0;
    else if (aEasing == "Linear")
        return 1;
    else if (aEasing == "Sine")
        return 2;
    else if (aEasing == "Quad")
        return 3;
    else if (aEasing == "Cubic")
        return 4;
    else if (aEasing == "Quart")
        return 5;
    else if (aEasing == "Quint")
        return 6;
    else if (aEasing == "Expo")
        return 7;
    else if (aEasing == "Circ")
        return 8;
    else if (aEasing == "Back")
        return 9;
    else if (aEasing == "Elastic")
        return 10;
    else if (aEasing == "Bounce")
        return 11;
    else
        return 1; // Default easing is Linear
}

int rangeToIndex(const QString& aRange) {
    if (aRange == "In")
        return 0;
    else if (aRange == "Out")
        return 1;
    else if (aRange == "All")
        return 2;
    else
        return 2; // Default range is InOut, it is referenced as "All" by Hidefuku
}

QString indexToEasing(int aIndex, bool translated = true) {
    if (translated) {
        switch (aIndex) {
        case 0:
            return QCoreApplication::translate("GeneralSettingsDialog", "None");
        case 1:
            return QCoreApplication::translate("GeneralSettingsDialog", "Linear");
        case 2:
            return QCoreApplication::translate("GeneralSettingsDialog", "Sine");
        case 3:
            return QCoreApplication::translate("GeneralSettingsDialog", "Quad");
        case 4:
            return QCoreApplication::translate("GeneralSettingsDialog", "Cubic");
        case 5:
            return QCoreApplication::translate("GeneralSettingsDialog", "Quart");
        case 6:
            return QCoreApplication::translate("GeneralSettingsDialog", "Quint");
        case 7:
            return QCoreApplication::translate("GeneralSettingsDialog", "Expo");
        case 8:
            return QCoreApplication::translate("GeneralSettingsDialog", "Circ");
        case 9:
            return QCoreApplication::translate("GeneralSettingsDialog", "Back");
        case 10:
            return QCoreApplication::translate("GeneralSettingsDialog", "Elastic");
        case 11:
            return QCoreApplication::translate("GeneralSettingsDialog", "Bounce");
        default:
            return QCoreApplication::translate("GeneralSettingsDialog", "Linear");
        }
    } else {
        switch (aIndex) {
        case 0:
            return QString("None");
        case 1:
            return QString("Linear");
        case 2:
            return QString("Sine");
        case 3:
            return QString("Quad");
        case 4:
            return QString("Cubic");
        case 5:
            return QString("Quart");
        case 6:
            return QString("Quint");
        case 7:
            return QString("Expo");
        case 8:
            return QString("Circ");
        case 9:
            return QString("Back");
        case 10:
            return QString("Elastic");
        case 11:
            return QString("Bounce");
        default:
            return QString("Linear");
        }
    }
}

QString indexToRange(int aRange, bool translated = true) {
    if (translated) {
        switch (aRange) {
        case 0:
            return QCoreApplication::translate("GeneralSettingsDialog", "In");
        case 1:
            return QCoreApplication::translate("GeneralSettingsDialog", "Out");
        case 3:
            return QCoreApplication::translate("GeneralSettingsDialog", "All");
        default:
            return QCoreApplication::translate("GeneralSettingsDialog", "All");
        }
    } else {
        switch (aRange) {
        case 0:
            return QString("In");
        case 1:
            return QString("Out");
        case 3:
            return QString("All");
        default:
            return QString("All");
        }
    }
}

QString indexToLanguage(int aIndex, bool translated = true) {
    if (translated) {
        switch (aIndex) {
        case 0:
            return QCoreApplication::translate("GeneralSettingsDialog", "Auto");
        case 1:
            return QCoreApplication::translate("GeneralSettingsDialog", "English");
            ;
        case 2:
            return QCoreApplication::translate("GeneralSettingsDialog", "Japanese");
            ;
        case 3:
            return QCoreApplication::translate("GeneralSettingsDialog", "Chinese");
            ;
        default:
            return QCoreApplication::translate("GeneralSettingsDialog", "Auto");
            ;
        }
    } else {
        switch (aIndex) {
        case 0:
            return QString("Auto");
        case 1:
            return QString("Enslish");
        case 2:
            return QString("Japanese");
        case 3:
            return QString("Chinese");
        default:
            return QString("Auto");
        }
    }
}
} // namespace

QString indexToTimeFormat(int aIndex) {
    switch (aIndex) {
    case core::TimeFormatType::TimeFormatType_Frames_From0:
        return QCoreApplication::translate("GeneralSettingsDialog", "Frame number (from 0)");
    case core::TimeFormatType::TimeFormatType_Frames_From1:
        return QCoreApplication::translate("GeneralSettingsDialog", "Frame number (from 1)");
    case core::TimeFormatType::TimeFormatType_Relative_FPS:
        return QCoreApplication::translate("GeneralSettingsDialog", "Relative to FPS (1.0 = 60.0)");
    case core::TimeFormatType::TimeFormatType_Seconds_Frames:
        return QCoreApplication::translate("GeneralSettingsDialog", "Seconds : Frame");
    case core::TimeFormatType::TimeFormatType_Timecode_SMPTE:
        return QCoreApplication::translate("GeneralSettingsDialog", "Timecode (SMPTE) (HH:MM:SS:FF)");
    case core::TimeFormatType::TimeFormatType_Timecode_HHMMSSmmm:
        return QCoreApplication::translate("GeneralSettingsDialog", "Timecode (HH:MM:SS:mmm)");
    default:
        return "";
    }
}


namespace gui {

GeneralSettingDialog::GeneralSettingDialog(GUIResources& aGUIResources, QWidget* aParent):
    EasyDialog(tr("General Settings"), aParent),
    mTabs(new QTabWidget(this)),
    mInitialLanguageIndex(),
    mLanguageBox(),
    mInitialTimeFormatIndex(),
    mTimeFormatBox(),
    mInitialThemeKey("breeze_dark"),
    mThemeBox(),
    mGUIResources(aGUIResources) {
    // read current settings
    {
        QSettings settings;
        auto language = settings.value("generalsettings/language");

        if (language.isValid()) {
            mInitialLanguageIndex = languageToIndex(language.toString());
        }

        auto easing = settings.value("generalsettings/easing");
        mInitialEasingIndex = easing.isValid() ? easingToIndex(easing.toString()) : easingToIndex("Linear");

        auto range = settings.value("generalsettings/range");
        mInitialRangeIndex = range.isValid() ? rangeToIndex(range.toString()) : rangeToIndex("All");

        auto timeScale = settings.value("generalsettings/ui/timeformat");
        if (timeScale.isValid()) {
            mInitialTimeFormatIndex = timeScale.toInt();
        }

        auto theme = settings.value("generalsettings/ui/theme");
        if (theme.isValid()) {
            mInitialThemeKey = theme.toString();
        }

        auto donationAllowed = settings.value("generalsettings/ui/donationAllowed");
        bDonationAllowed = donationAllowed.isValid()? donationAllowed.toBool() : true;

        auto ignoreWarnings = settings.value("export_ignore_warnings");
        bIgnoreWarnings = ignoreWarnings.isValid()? false : ignoreWarnings.toBool();

        auto isAutoSave = settings.value("generalsettings/projects/autosaveEnabled");
        bAutoSave = isAutoSave.isValid()? isAutoSave.toBool() : false;

        auto isAutoSaveDelay = settings.value("generalsettings/projects/autosaveDelay");
        mAutoSaveDelay = isAutoSaveDelay.isValid() ? isAutoSaveDelay.toInt() : 5;

        auto isAutoCbCopy = settings.value("generalsettings/keys/autocb");
        bAutoCbCopy = !isAutoCbCopy.isValid()? isAutoCbCopy.toBool() : false;

        auto isAutoFFmpegCheck = settings.value("ffmpeg_check");
        mAutoFFmpegCheck = isAutoFFmpegCheck.isValid()? isAutoFFmpegCheck.toBool() : true;

        auto isResIDCheck = settings.value("res_id_check");
        bResIDCheck = isResIDCheck.isValid()?  isResIDCheck.toBool() : true;

        auto isKeyDelay = settings.value("generalsettings/keybindings/keyDelay");
        mKeyDelay = isKeyDelay.isValid() ? isKeyDelay.toInt() : 125;

        auto isAutoShowMesh = settings.value("generalsettings/tools/autoshowmesh");
        bAutoShowMesh = isAutoShowMesh.isValid()? isAutoShowMesh.toBool() : false;
    }

    auto form = new QFormLayout();

    // create inner widgets
    {
        mLanguageBox = new QComboBox();
        for (int i = 0; i < kLanguageTypeCount; ++i) {
            mLanguageBox->addItem(indexToLanguage(i));
        }
        mLanguageBox->setCurrentIndex(mInitialLanguageIndex);
        form->addRow(tr("Language (needs restarting) :"), mLanguageBox);

        mEasingBox = new QComboBox();
        for (int i = 0; i < kEasingTypeCount; ++i) {
            mEasingBox->addItem(indexToEasing(i));
        }
        mEasingBox->setCurrentIndex(mInitialEasingIndex);
        form->addRow(tr("Default keyframe easing :"), mEasingBox);

        mRangeBox = new QComboBox();
        for (int i = 0; i < kRangeTypeCount; ++i) {
            mRangeBox->addItem(indexToRange(i));
        }
        mRangeBox->setCurrentIndex(mInitialRangeIndex);
        form->addRow(tr("Default keyframe range :"), mRangeBox);

        mTimeFormatBox = new QComboBox();
        for (int i = 0; i < kTimeFormatTypeCount; ++i) {
            mTimeFormatBox->addItem(indexToTimeFormat(i));
        }
        mTimeFormatBox->setCurrentIndex(mInitialTimeFormatIndex);
        form->addRow(tr("Timeline format :"), mTimeFormatBox);


        mThemeBox = new QComboBox();
        QStringList themeList = mGUIResources.themeList();
        for (int i = 0; i < themeList.size(); ++i) {
            mThemeBox->addItem(themeList[i], themeList[i]);
        }
        mThemeBox->setCurrentIndex(mThemeBox->findData(mInitialThemeKey));
        form->addRow(tr("Theme :"), mThemeBox);
    }

    auto projectSaving = new QFormLayout();
    {
        mAutoSave = new QCheckBox();
        mAutoSave->setChecked(bAutoSave);
        projectSaving->addRow(tr("Automatically save your project : "), mAutoSave);

        mAutoSaveDelayBox = new QSpinBox();
        mAutoSaveDelayBox->setValue(mAutoSaveDelay);
        projectSaving->addRow(tr("Time (in minutes) between autosaves : "), mAutoSaveDelayBox);

        mAutoShowMesh = new QCheckBox();
        mAutoShowMesh->setChecked(bAutoShowMesh);
        connect(mAutoShowMesh, &QPushButton::clicked, [=]() {
            QSettings settings;
            settings.setValue("generalsettings/tools/autoshowmesh", mAutoShowMesh->isChecked());
        });
        projectSaving->addRow(tr("Show mesh when selecting FFD : "), mAutoShowMesh);

        mAutoCbCopy = new QCheckBox();
        mAutoCbCopy->setChecked(bAutoCbCopy);
        projectSaving->addRow(tr("On copy send keys to the clipboard : "), mAutoCbCopy);

        mAutoFFmpegBox = new QCheckBox();
        mAutoFFmpegBox->setChecked(mAutoFFmpegCheck);
        projectSaving->addRow(tr("Always check for FFmpeg on export : "), mAutoFFmpegBox);

        bResIDBox = new QCheckBox();
        bResIDBox->setChecked(bResIDCheck);
        projectSaving->addRow(tr("Enforce ID check on asset download : "), bResIDBox);

        mIgnoreWarnings = new QCheckBox();
        mIgnoreWarnings->setChecked(bIgnoreWarnings);
        projectSaving->addRow(tr("Ignore export warnings :"), mIgnoreWarnings);

        mDonationAllowed = new QCheckBox();
        mDonationAllowed->setChecked(bDonationAllowed);
        projectSaving->addRow(tr("Allow donation menu : "), mDonationAllowed);

        mResetButton = new QPushButton(tr("Reset recent files list"));
        mResetButton->setToolTip(tr("Deletes all project entries from your recents"));
        connect(mResetButton, &QPushButton::clicked, [=]() {
            QSettings settings;
            settings.remove("projectloader/recents");
            MainWindow::showInfoPopup(tr("Success"), tr("All entries have been successfully removed"), "Info");
        });
        projectSaving->addRow(mResetButton);
    }

    auto keybindingSettings = new QFormLayout();
    {
        mKeyDelayBox = new QSpinBox();
        mKeyDelayBox->setRange(0, 10000);
        mKeyDelayBox->setValue(mKeyDelay);
        keybindingSettings->addRow(tr("Global keybind delay (ms) : "), mKeyDelayBox);

        mResetKeybindsButton = new QPushButton(tr("Reset keybinds"));
        mResetKeybindsButton->setToolTip(tr("Reset all keybinds, a restart is required."));
        connect(mResetKeybindsButton, &QPushButton::clicked, [=]() {
            QSettings settings;
            settings.setValue("keybindReset",
                !settings.value("keybindReset").isValid() || !settings.value("keybindReset").toBool()
            );
            MainWindow::showInfoPopup(
                tr("Keybinds reset status"),
                settings.value("keybindReset").toBool() ? tr("The keybinds will be reset when you restart.")
                                                        : tr("The keybinds will not be reset when you restart."),
                "Info"
            );
        });
        keybindingSettings->addRow(mResetKeybindsButton);
    }

    auto ffmpegSettings = new QFormLayout();
    {

        ffmpegTroubleshoot = new QPushButton(tr("Troubleshoot FFmpeg"));
        connect(ffmpegTroubleshoot, &QPushButton::clicked, [=]() {
            util::NetworkUtil networking;
            QFileInfo ffmpeg_file;
            QString ffmpeg;
            QMessageBox ffmpegNotif;

            if (util::NetworkUtil::os() == "win") {
                ffmpeg_file = QFileInfo("./tools/ffmpeg.exe");
            } else {
                ffmpeg_file = QFileInfo("./tools/ffmpeg");
            }
            if (!ffmpeg_file.exists() || !ffmpeg_file.isExecutable()) {
                ffmpeg = "ffmpeg";
            } else {
                ffmpeg = ffmpeg_file.absoluteFilePath();
            }

            // Exists?
            bool fExists = util::NetworkUtil::libExists(ffmpeg, "-version");

            if (!fExists) {
                ffmpegNotif.setWindowTitle(tr("FFmpeg error"));
                ffmpegNotif.setText(tr("FFmpeg is either missing, corrupted or otherwise doesn't work."));
                ffmpegNotif.addButton(QMessageBox::Ok);
                ffmpegNotif.exec();
                return;
            }
            // Test file
            QString testFile = QFileInfo("./data/themes/classic/icon/filew.png").absoluteFilePath();

            // Sample gif test
            QProcess gif;
            gif.start(ffmpeg, {"-i", testFile, "gif.gif"}, QProcess::ReadWrite);
            gif.waitForFinished();
            bool exportSuccess = gif.exitStatus() == 0 && QFileInfo::exists("gif.gif");
            qDebug() << "Gif exists: " << QFileInfo::exists("gif.gif")
                     << "| Gif remove: " << QFile("gif.gif").remove();
            gif.deleteLater();

            if (!exportSuccess) {
                ffmpegNotif.setWindowTitle(tr("FFmpeg doesn't export"));
                ffmpegNotif.setText(tr("FFmpeg was unable to export, please check if it's a valid FFmpeg executable."));
                ffmpegNotif.addButton(QMessageBox::Ok);
                ffmpegNotif.exec();
                return;
            }

            // Palettegen test
            QProcess palettegen;
            palettegen.start(ffmpeg, {"-i", testFile, "-vf", "palettegen", "palette.png"}, QProcess::ReadWrite);
            palettegen.waitForFinished();
            bool pGenSuccess = palettegen.exitStatus() == 0 && QFileInfo::exists("palette.png");
            if (!pGenSuccess) {
                ffmpegNotif.setWindowTitle(tr("FFmpeg doesn't generate palettes"));
                ffmpegNotif.setText(
                    tr("FFmpeg was unable to generate palettes, please check if it's a valid FFmpeg executable.")
                );
                ffmpegNotif.addButton(QMessageBox::Ok);
                ffmpegNotif.exec();
                return;
            }
            qDebug() << "Palette exists: " << QFileInfo::exists("palette.png")
                     << "| Palette remove: " << QFile("palette.png").remove();
            palettegen.deleteLater();


            ffmpegNotif.setWindowTitle(tr("FFmpeg test success"));
            ffmpegNotif.setText(tr("All tests have passed, FFmpeg is working correctly."));
            ffmpegNotif.setDetailedText(
                tr("Tests:\nCheck FFmpeg version ... 🗸\nCheck FFmpeg exporting ... 🗸\n"
                   "Check FFmpeg palette generation ... 🗸")
            );
            ffmpegNotif.addButton(QMessageBox::Ok);
            ffmpegNotif.exec();
            return;
        });

        selectFromExe = new QPushButton(tr("Select from executable and automatically setup"));
        selectFromExe->setToolTip(
            tr("This will remove previous instances of FFmpeg from your tools directory"
               " and replace them with your custom executable, please make sure this is a valid FFmpeg executable.")
        );
        connect(selectFromExe, &QPushButton::clicked, [=]() {
            util::NetworkUtil net;
            QDir dir = QDir("./tools");
            if (!dir.exists()) {dir.mkpath(dir.absolutePath());}
            QString file = util::NetworkUtil::os() == "win" ? "/ffmpeg.exe" : "/ffmpeg";

            if (QFileInfo::exists(dir.absolutePath() + file)) {
                QFile(dir.absolutePath() + file).moveToTrash();
            }

            QFileInfo ffmpeg = QFileInfo(QFileDialog::getOpenFileName(nullptr, "Select FFmpeg executable"));
            qDebug() << "Selected exe: " << ffmpeg.fileName() << " | " << ffmpeg.absoluteFilePath();
            QFile(ffmpeg.absoluteFilePath()).copy(dir.absolutePath() + file);

            QMessageBox success;
            success.setText(tr("Operation successful"));
            success.setWindowTitle(tr("Success"));
            success.addButton(QMessageBox::Ok);
            success.exec();
        });


        autoSetup = new QPushButton(tr("Download and automatically setup"));
        connect(autoSetup, &QPushButton::clicked, [=]() {
            QDir dir = QDir("./tools");
            if (!dir.exists()) { dir.mkpath(dir.absolutePath()); }
            QString file = util::NetworkUtil::os() == "win" ? "/ffmpeg.exe" : "/ffmpeg";
            if (QFileInfo::exists(dir.absolutePath() + file)) {
                QFile(dir.absolutePath() + file).moveToTrash();
            }
            QString os = util::NetworkUtil::os();
            QString arch = util::NetworkUtil::arch();
            QString gitFile;
            // ID checking is probably overkill, but security is important!
            int id;
            if (os == "win") {
                gitFile = "ffmpeg-win64.exe";
                id = 222521524;
            } else if (os == "linux") {
                gitFile = "ffmpeg-linux64";
                id = 222521700;
            } else {
                gitFile = "ffmpeg-macos";
                id = 222521413;
            }

            QFileInfo ffmpeg = util::NetworkUtil::downloadGithubFile(
                "https://api.github.com/repos/AnimeEffectsDevs/ffmpeg-bin/releases/latest", gitFile, id, this
                );
            qDebug() << "Download name : " << ffmpeg.fileName() << "\n" << "Download is executable : " << ffmpeg.isExecutable();
            bool pathSetAttempted = false;
            bool ffmpegMoveAttempted = false;
            if (ffmpeg.isExecutable()) {
                if(!dir.exists() || !dir.isReadable() || !QFileInfo(dir.absolutePath()).isWritable()){
                    // We'll attempt a global installation as a fallback in cases where the folder is write protected
                    QDir appdata = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
                    auto anieFolder = QDir(appdata.absolutePath() + "/AnimeEffects");
                    if (!anieFolder.exists() || !QFileInfo(anieFolder.absolutePath()).isWritable()) {
                        if (!appdata.mkdir("AnimeEffects")) {
                            qDebug(
                                "AnimeEffects folder creation failed in AppData, attempting to locate in Documents..."
                            );
                            appdata = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
                            anieFolder = QDir(appdata.absolutePath() + "/AnimeEffects");
                            if (!anieFolder.exists() || !QFileInfo(anieFolder.absolutePath()).isWritable()) {
                                if (!appdata.mkdir("AnimeEffects")) {
                                    qDebug("AnimeEffects folder creation failed, skipping...");
                                }
                            } else {
                                appdata = anieFolder;
                            }
                        } else {
                            appdata = anieFolder;
                        }
                    }
                    else {
                        appdata = anieFolder;
                    }
                    const QString fileInAppDataLoc = appdata.absolutePath() + file;
                    QFile(ffmpeg.absoluteFilePath()).rename(fileInAppDataLoc);
                    qDebug() << "FFmpeg moved : " << QFile(fileInAppDataLoc).exists();
                    if (QFile(fileInAppDataLoc).exists()) {
                        QProcess pathSet;
                        QString console;
                        QStringList instruct;
                        if(os == "linux" || os == "mac"){
                            console = "sh";
                            instruct.append("export");
                            instruct.append("PATH=$PATH:" + fileInAppDataLoc);
                        }
                        else{
                            console = "cmd";
                            instruct.append("set");
                            instruct.append("PATH=%PATH%;" + fileInAppDataLoc);
                        }
                        pathSet.start(console, instruct);
                        pathSet.waitForFinished();
                        if (pathSet.exitCode() == 0) {
                            QMessageBox success;
                            success.setWindowTitle(tr("Success"));
                            success.setText(tr("FFmpeg was successfully setup. Please restart AnimeEffects"));
                            success.exec();
                            return;
                        }
                        pathSetAttempted = true;
                        qDebug("FFmpeg path setup errored, continuing...");
                    }
                    else {
                        ffmpegMoveAttempted = true;
                        qDebug("FFmpeg move errored, continuing...");
                    }
                }
                QFile(ffmpeg.absoluteFilePath()).rename(dir.absolutePath() + file);
                qDebug() << "FFmpeg moved : " << QFile(dir.absolutePath() + file).exists();
                if (QFile(dir.absolutePath() + file).exists()) {
                    QMessageBox success;
                    success.setWindowTitle(tr("Success"));
                    success.setText(tr("FFmpeg was successfully setup."));
                    success.exec();
                    return;
                }
            }
            QMessageBox error;
            error.setWindowTitle(tr("Error"));
            error.setText(tr("While setting up FFmpeg an unexpected error has occurred, please send the below information to our devs."));
            QString platform = QSysInfo::productType(); platform[0] = platform[0].toUpper();
            error.setDetailedText(
                QString("Operating system: ") + platform + " " + QSysInfo::productVersion() +
                "\nCPU: " + QSysInfo::currentCpuArchitecture() +
                "\nFile requested: " + gitFile + " - Hardcoded file ID: " + QString::number(id) +
                "\nFile received: " + ffmpeg.absoluteFilePath() +
                "\nFile is executable: " + (ffmpeg.isExecutable()? "True" : "False") +
                "\nFile is readable: " + (ffmpeg.isReadable()? "True": "False") +
                "\nFile is writable: " + (ffmpeg.isWritable()? "True": "False") +
                "\nRequested base folders : " + dir.absolutePath() + " | " + QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + " | " + QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
                "\nFallback path set errored: " + (pathSetAttempted? "True" : "False") +
                "\nFallback FFmpeg move errored: " + (ffmpegMoveAttempted? "True" : "False")
            );
            error.exec();
        });

        ffmpegSettings->addRow(ffmpegTroubleshoot);
        ffmpegSettings->addRow(selectFromExe);
        ffmpegSettings->addRow(autoSetup);
    }

    createTab(tr("General"), form);
    createTab(tr("QoL"), projectSaving);
    createTab(tr("FFmpeg"), ffmpegSettings);
    createTab(tr("Keybindings"), keybindingSettings);


    this->setMainWidget(mTabs, false);

    this->setOkCancel([=](int aResult) -> bool {
        if (aResult == 0) {
            this->saveSettings();
        }
        return true;
    });
}

QFormLayout* GeneralSettingDialog::createTab(const QString& aTitle, QFormLayout* aForm) {
    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);

    auto frame = new QFrame();
    scroll->setWidget(frame);

    auto form = new QFormLayout();
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    form->setLabelAlignment(Qt::AlignRight);
    frame->setLayout(aForm);
    frame->adjustSize();
    mTabs->addTab(scroll, aTitle);

    return form;
}

bool GeneralSettingDialog::languageHasChanged() { return mInitialLanguageIndex != mLanguageBox->currentIndex(); }

bool GeneralSettingDialog::easingHasChanged() { return mInitialEasingIndex != mEasingBox->currentIndex(); }

bool GeneralSettingDialog::rangeHasChanged() { return mInitialRangeIndex != mRangeBox->currentIndex(); }

bool GeneralSettingDialog::timeFormatHasChanged() { return mInitialTimeFormatIndex != mTimeFormatBox->currentIndex(); }

bool GeneralSettingDialog::themeHasChanged() { return mInitialThemeKey != mThemeBox->currentData(); }

bool GeneralSettingDialog::donationHasChanged() {return bDonationAllowed != mDonationAllowed->isChecked(); }

bool GeneralSettingDialog::ignoreWarningsHasChanged() {return bIgnoreWarnings != mIgnoreWarnings->isChecked(); }

bool GeneralSettingDialog::autoSaveHasChanged() { return bAutoSave != mAutoSave->isChecked(); }

bool GeneralSettingDialog::autoSaveDelayHasChanged() { return mAutoSaveDelay != mAutoSaveDelayBox->value(); }

bool GeneralSettingDialog::autoFFmpegHasChanged() { return mAutoFFmpegCheck != mAutoFFmpegBox->isChecked(); }

bool GeneralSettingDialog::resIDCheckHasChanged() { return bResIDCheck != bResIDBox->isChecked(); }

bool GeneralSettingDialog::cbCopyHasChanged() { return bAutoCbCopy != mAutoCbCopy->isChecked(); }

QString GeneralSettingDialog::theme() { return mThemeBox->currentData().toString(); }

bool GeneralSettingDialog::keyDelayHasChanged() { return mKeyDelay != mKeyDelayBox->value(); }

void GeneralSettingDialog::saveSettings() {
    QSettings settings;
    if (languageHasChanged())
        settings.setValue("generalsettings/language", indexToLanguage(mLanguageBox->currentIndex(), false));

    if (easingHasChanged()) {
        settings.setValue("generalsettings/easing", indexToEasing(mEasingBox->currentIndex(), false));
    }
    if (rangeHasChanged()) {
        settings.setValue("generalsettings/range", indexToRange(mRangeBox->currentIndex(), false));
    }
    if (timeFormatHasChanged()) {
        settings.setValue("generalsettings/ui/timeformat", mTimeFormatBox->currentIndex());
    }
    if (themeHasChanged()) {
        settings.setValue("generalsettings/ui/theme", mThemeBox->currentData());
    }
    if (donationHasChanged()){
        settings.setValue("generalsettings/ui/donationAllowed", mDonationAllowed->isChecked());
    }
    if (ignoreWarningsHasChanged()) {
        settings.setValue("export_ignore_warnings", mIgnoreWarnings->isChecked());
    }
    if (autoSaveHasChanged()) {
        settings.setValue("generalsettings/projects/autosaveEnabled", mAutoSave->isChecked());
    }
    if (autoSaveDelayHasChanged()) {
        settings.setValue("generalsettings/projects/autosaveDelay", mAutoSaveDelayBox->value());
    }
    if (autoFFmpegHasChanged()){
        settings.setValue("ffmpeg_check", mAutoFFmpegBox->isChecked());
    }
    if (resIDCheckHasChanged()) {
        settings.setValue("res_id_check", bResIDBox->isChecked());
    }
    if (cbCopyHasChanged()) {
        settings.setValue("generalsettings/keys/autocb", mAutoCbCopy->isChecked());
    }
    if (keyDelayHasChanged()) {
        settings.setValue("generalsettings/keybindings/keyDelay", mKeyDelayBox->value());
    }
}
} // namespace gui
